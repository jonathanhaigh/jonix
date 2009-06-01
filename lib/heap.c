/* ---------------------------------------------------
* Jonix
*
* heap.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "mem.h"
#include "c/string.h"
#include "devs/scrn.h"
#include "util/sort.h"
#include "util/oarray.h"

char heap_err[ERR_STR_SIZE];

/*  ----------------------------------------------------
 *  Function:           heap_place
 *
 *  Creates a heap at a given address. Requires that the
 *  pages for the heap are present.
 *  --------------------------------------------------*/
heap_t *heap_place(void *start, int no_pages, bool rw, bool user){

    void *end    = start + 4096*no_pages;

    /* We must be page aligned */
    if(((uint32_t) start) % 4096 != 0){
        strcpy(heap_err, "Start of heap must be page aligned.");
        return 0;
    }

    /* Heap must be big enough */
    if( 4096*no_pages < 
            (sizeof(heap_t)                     // heap data structure
            + HEAP_INDEX_SIZE*sizeof(list_item_t))  // heap index
            + 4096                              // extra page for page table if needed
            + MIN_HEAP_SIZE                     // the actual heap 
    ){
        strcpy(heap_err, "Heap is too small.");
        return 0;
    }

    heap_t *heap    = (heap_t *) start;
    start          += sizeof(heap_t);
    heap->index     = oarray_place(start, heap_hdr_t_cmp_size, HEAP_INDEX_SIZE);
    start          += HEAP_INDEX_SIZE*(sizeof(list_item_t));

    heap->start     = start;
    heap->end       = end;
    heap->user      = user;
    heap->rw        = rw;

    /*
     * Create a header and footer for our first (and only) block.
     */
    heap_hdr_t *hdr = heap->start;
    hdr->hole       = 1;
    hdr->size       = (uint32_t) heap->end - (uint32_t) heap->start;
    hdr->magic      = HEAP_HDR_MAGIC;

    heap_ftr_t *ftr = heap->end - sizeof(heap_ftr_t);
    ftr->hdr        = hdr;
    ftr->magic      = HEAP_FTR_MAGIC;

    /* Add block to the heap index */
    oarray_add(&heap->index, (list_item_t) hdr);

    return heap;
}


/*  ----------------------------------------------------
 *  Function:           heap_extend
 *
 *  Extends a heap. Pages should not already be present.
 *  --------------------------------------------------*/
bool heap_extend(heap_t *heap, int no_pages){

    /* Reserve the memory.
     */
    if(!psalloc_at_v((uint32_t) heap->end, heap->rw, heap->user, no_pages)){
        strcpy(heap_err, "Failed to reserve new memory to extend heap.");
        return 0;
    }

    /* Make the new memory a block - with header and footer.
     */
    heap_hdr_t *hdr = heap->end;
    hdr->hole       = 1;
    hdr->size       = 4096*no_pages;
    hdr->magic      = HEAP_HDR_MAGIC;

    heap_ftr_t *ftr = heap->end + 4096*no_pages - sizeof(heap_ftr_t);
    ftr->hdr        = hdr;
    ftr->magic      = HEAP_FTR_MAGIC;

    /* Add block to the heap index
     */
    if(!oarray_add(&heap->index, (list_item_t) hdr)){
        strcpy(heap_err, "Failed to add new block to heap index.");
        return 0;    
    }

    /* Update heap's 'end' pointer
     */
    heap->end   = heap->end + 4096*no_pages;

    return 1;
}


/*  ----------------------------------------------------
 *  Function:           heap_alloc
 *
 *  Allocates memory from heap.
 *  --------------------------------------------------*/
void * heap_alloc(heap_t *heap, uint32_t size, bool page_align){

    /* Account for the size of the header and footer
     */
    uint32_t size_hf = size + sizeof(heap_hdr_t) + sizeof(heap_ftr_t);

    int i;
    heap_hdr_t *hdr;
    heap_ftr_t *ftr;
    for(i=0; i<heap->index.size; i++){
        hdr   = (heap_hdr_t *) heap->index.array[i];
        if(hdr->size >= size_hf){

            //printf("Found block of suitable size. start=%H, size=%H, end=%H.\n", (uint32_t) hdr, hdr->size, (uint32_t)hdr + hdr->size);

            // Save the block's footer address for later
            //
            ftr = (heap_ftr_t *)((uint32_t) hdr + hdr->size - sizeof(heap_ftr_t));


            if(
                // Do we need to page align?
                //
                page_align  
                &&
                // Are we not already page aligned?
                // Note that we need the useable space (after the header)
                // to be aligned, not the header.
                //
                (((uint32_t) hdr + sizeof(heap_hdr_t)) & 0xFFFFF000) != 0
            ){

                // Where the next page starts.
                //
                uint32_t next_page = ((uint32_t) hdr + sizeof(heap_hdr_t) + 4096) & 0xFFFFF000;

                // Check the block is still big enough after we page align.
                // 
                if(next_page + size +sizeof(heap_ftr_t) >= (uint32_t) hdr + hdr->size){
                    continue;
                }

                // Delete this block from the heap index.
                // 
                oarray_del(&heap->index, i);

                heap_hdr_t *new_hdr    = (heap_hdr_t *) (next_page - sizeof(heap_hdr_t));

                /* We need to remove the chunk of the block that
                 * is before the page boundary. If it is too small
                 * to be a block by itself then we append it to the
                 * previous block (if there is one).
                 */
                uint32_t offset  = (uint32_t) new_hdr - (uint32_t) hdr;
                uint32_t new_size = hdr->size - offset;

                // If spare space is too small to create new block.
                //
                if(offset <= sizeof(heap_ftr_t) + sizeof(heap_hdr_t)){

                    // Check if there actually is a previous block
                    //
                    if((uint32_t)hdr - sizeof(heap_ftr_t) - sizeof(heap_hdr_t) < (uint32_t) heap->start){

                        // There is no previous block. We will just
                        // discard the spare memory (until the whole heap is freed).
                        // This is not too bad because this should not happen very often.
                        //
                        memset(hdr, 0, offset);
                        
                        // We can now continue as normal (as if we did not need
                        // to page align), so long as we set hdr to point to new_hdr
                        // and create a new header for the block.
                        // We also need to tell the footer of the block to point to 
                        // the new header (new_hdr)
                        //
                        ftr->hdr        = new_hdr;
                        hdr             = new_hdr;
                        hdr->size       = new_size;
                        hdr->hole       = 1;
                        hdr->magic      = HEAP_HDR_MAGIC;
                    }
                    else{

                        // There is a previous block. We will attach spare space
                        // to previous block. The previous block should be in use
                        // (not a hole), so the space cannot be claimed until the
                        // previous block is freed.
                        //
                        heap_ftr_t *prev_ftr  = (heap_ftr_t *)((uint32_t) hdr - sizeof(heap_ftr_t));

                        // Check the prev ftr's magic number.
                        //
                        if(prev_ftr->magic != HEAP_FTR_MAGIC){
                            PANIC("Kernel heap corrupted.");
                        }

                        // Update the previous header's size.
                        //
                        prev_ftr->hdr->size += offset;

                        // Move the prev blocks footer.
                        // 
                        memmove(new_hdr - sizeof(heap_ftr_t), prev_ftr, sizeof(heap_ftr_t));
                        //
                        // We can now continue as normal (as if we did not need
                        // to page align), so long as we set hdr to point to *new_hdr
                        // and create a new header for the block. We also need to
                        // update the block's footer to point to new_hdr.
                        //
                        ftr->hdr    = new_hdr;
                        hdr         = new_hdr;
                        hdr->size   = new_size;
                        hdr->hole   = 1;
                        hdr->magic  = HEAP_HDR_MAGIC;
                    
                    }
                }
                else{
                    // The spare space is enough to form its own block.
                    // There is already a header at the start (*hdr),
                    // so we just need to adjust the size and make a
                    // footer.
                    //
                    hdr->size   = offset;

                    heap_ftr_t * new_prev_ftr = (heap_ftr_t *)((uint32_t)new_hdr - sizeof(heap_ftr_t));
                    new_prev_ftr->hdr  = hdr;
                    new_prev_ftr->magic = HEAP_FTR_MAGIC;

                    // Add the new block to the heap index.
                    //
                    oarray_add(&heap->index, hdr);

                    // We can now continue as normal (as if we did not need
                    // to page align), so long as we set hdr to point to *new_hdr
                    // and create a new header for the block. We also need to 
                    // update the footer for the block to point to new_header.
                    //
                    ftr->hdr    = new_hdr;
                    hdr         = new_hdr;
                    hdr->size   = new_size;
                    hdr->hole   = 1;
                    hdr->magic  = HEAP_HDR_MAGIC;
                }
            }
            else{
                // We did not have to page align, so this block is definitely
                // suitable. We need to delete it from the heap index.
                //
                oarray_del(&heap->index, i);
            }
            
            /*
             * We have found a suitable block. We now have two options:
             *
             * 1. Give this whole block away.
             * 2. Reduce the block size and create a new block(s) after
             * this one.
             *
             * The adjacent blocks should never be holes so there's little
             * point in merging extra free space into adjacent blocks.
             */

            void * hdr_v  = (void *) hdr;

            // If the spare space is not big enough to fit a header and footer
            // and some extra space, then we give the whole block away.
            //
            if(hdr->size <= size_hf + sizeof(heap_hdr_t) + sizeof(heap_ftr_t)){
                //printf("Allocating whole block\n");
                hdr->hole   = 0;
                return hdr_v + sizeof(heap_hdr_t);
            }

            // Reduce size of block
            //
            int old_size    = hdr->size;
            hdr->size       = size_hf;

            hdr->hole       = 0;

            //printf("Reducing block size to %H. New end=%H\n", hdr->size, (uint32_t)hdr + hdr->size);

            // Create new footer
            //
            heap_ftr_t *ftr1    = (heap_ftr_t *) (hdr_v + size + sizeof(heap_hdr_t));
            ftr1->hdr           = hdr;
            ftr1->magic         = HEAP_FTR_MAGIC;

            // Create header for new block
            //
            heap_hdr_t *hdr2    = (heap_hdr_t *) (hdr_v + size_hf);
            hdr2->hole          = 1;
            hdr2->size          = old_size - size_hf;
            hdr2->magic         = HEAP_HDR_MAGIC;
            //printf("Creating new block. start=%H, size=%H, end=%H.\n", (uint32_t)hdr2, hdr2->size, (uint32_t)hdr2 + hdr2->size);

            // Update old footer
            //
            ftr->hdr    = hdr2;

            // Update heap index
            //
            oarray_add(&heap->index, hdr2);

            return hdr_v + sizeof(heap_hdr_t);
        }
    }

    // There are no suitable blocks.
    // We can try to extend the heap.
    //
    int no_new_pages    = (size_hf / 4096) + ((size_hf % 4096)? 1 : 0);

    //printf("No suitable blocks. Extending heap by %d pages\n", no_new_pages);

    if(!heap_extend(heap, no_new_pages)) return NULL;

    // Heap was extended so try again.
    //
    return heap_alloc(heap, size, page_align);
}

/*  ----------------------------------------------------
 *  Function:           heap_palloc
 *
 *  Allocates a page aligned page of memory from heap.
 *  --------------------------------------------------*/
void * heap_palloc(heap_t *heap){
    return heap_alloc(heap, 4096, 1);
}

/*  ----------------------------------------------------
 *  Function:           heap_psalloc
 *
 *  Allocates page aligned pagex of memory from heap.
 *  --------------------------------------------------*/
void * heap_psalloc(heap_t *heap, uint16_t pages){
    return heap_alloc(heap, 4096*pages, 1);
}


/*  ----------------------------------------------------
 *  Function:           heap_free
 *
 *  Returns allocated memory back to a heap.
 *  --------------------------------------------------*/
bool heap_free(heap_t *heap, void *addr){

    addr    -= sizeof(heap_hdr_t);

    heap_hdr_t *hdr = (heap_hdr_t *) addr;

    if(hdr->magic != HEAP_HDR_MAGIC){
        strcpy(heap_err, "Heap header magic number is wrong - heap is corrupted.");
        return 0;
    }

    heap_ftr_t *ftr = (heap_ftr_t *) (addr + hdr->size - sizeof(heap_ftr_t));

    if(ftr->magic != HEAP_FTR_MAGIC){
        strcpy(heap_err, "Heap footer magic number is wrong - heap is corrupted.");
        return 0;
    }

    hdr->hole   = 1;

    /*
     * We have four options:
     *
     * 1. Just give this block back to the heap.
     * 2. Merge this block with previous block.
     * 3. Merge this block with next block.
     * 4. Both 2 and 3.
     */

    /* Check if previous block is a hole
     */
    if(addr > heap->start){

        heap_ftr_t *prev_ftr    = (heap_ftr_t *) (addr - sizeof(heap_ftr_t));

        if(prev_ftr->magic != HEAP_FTR_MAGIC){
            strcpy(heap_err, "Heap footer magic number is wrong - heap is corrupted.");
            return 0;
        }

        heap_hdr_t *prev_hdr    = prev_ftr->hdr;

        if(prev_hdr->magic != HEAP_HDR_MAGIC){
            strcpy(heap_err, "Heap header magic number is wrong - heap is corrupted.");
            return 0;
        }

        if(prev_hdr->hole){
            prev_hdr->size += hdr->size;
            ftr->hdr        = prev_hdr;

            /* Update heap index
             */
            oarray_del_value(&heap->index, (list_item_t) prev_hdr);

            hdr     = prev_hdr;
            ftr     = prev_ftr;
            addr    = (void *) hdr;

        }
    }

    /* Check if next block is a hole
     */
    if(addr + hdr->size < heap->end){

        heap_hdr_t *next_hdr    = (heap_hdr_t *) (addr + hdr->size);

        if(next_hdr->magic != HEAP_HDR_MAGIC){
            strcpy(heap_err, "Heap header magic number is wrong - heap is corrupted.");
            return 0;
        }

        if(next_hdr->hole){

            heap_ftr_t *next_ftr    = (heap_ftr_t *) ((void *) next_hdr + next_hdr->size - sizeof(heap_ftr_t));

            if(next_ftr->magic != HEAP_FTR_MAGIC){
                strcpy(heap_err, "Heap footer magic number is wrong - heap is corrupted.");
                return 0;
            }

            hdr->size += next_hdr->size;
            next_ftr->hdr   = hdr;

            /* Update heap index
             */
            oarray_del_value(&heap->index, (list_item_t) next_hdr);
            ftr     = next_ftr;
        }
    }

    /* Add block to heap index
     */
    if(!oarray_add(&heap->index, hdr)){
        strcpy(heap_err, "Failed to insert new block into heap index.");
        return 0;
    }

    return 1;
}

/*  ----------------------------------------------------
 *  Function:           heap_hdr_t_cmp_size
 *
 *  Comparison function to sort the heap index.
 *  --------------------------------------------------*/
DEFINE_CMP_MEMBER(heap_hdr_t, size, uint32_t, 1)

/*  ----------------------------------------------------
 *  Function:           heap_print_info
 *
 *  Debug function. prints info about a heap to screen.
 *  --------------------------------------------------*/
void heap_print_info(heap_t * heap){
    scrn_puts("HEAP INFO\n\n");
    scrn_put_var_h("Header Start", (long) heap);
    scrn_put_var_h("Index Start", (long) &heap->index);
    scrn_put_var_h("No Blocks", (long) heap->index.size);
    scrn_put_var_h("Main Heap Start", (long) heap->start);
    scrn_put_var_h("Heap End", (long) heap->end);
    scrn_put_var_h("Read/Write", (long) heap->rw);
    scrn_put_var_h("User", (long) heap->user);
}

/*  ----------------------------------------------------
 *  Function:           heap_print_index
 *
 *  Debug function. prints heap index to screen.
 *  --------------------------------------------------*/
void heap_print_index(heap_t * heap){
    scrn_puts("HEAP INDEX\n\n");
    scrn_puts("HOLE START\tHOLE END\tHOLE SIZE\n");

    int i;
    heap_hdr_t *hdr;
    for(i=0; i<heap->index.size; i++){
        scrn_putn_h((long) heap->index.array[i]);
        scrn_puts("\t");
        hdr = (heap_hdr_t *) heap->index.array[i];
        scrn_putn_h((long) heap->index.array[i] + hdr->size);
        scrn_puts("\t");
        scrn_putn_h((long) hdr->size);
        scrn_puts("\n");
    }
}
