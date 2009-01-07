/* ---------------------------------------------------
* Jonix
*
* mem.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "mem.h"
#include "string.h"
#include "scrn.h"

/* These are variables defined in linker.ld.
 * Their *address* is what we actually want.
 */
extern char kmem_start_var;
extern char kmem_text_start_var;
extern char kmem_rodata_start_var;
extern char kmem_data_start_var;
extern char kmem_bss_start_var;
extern char kmem_end_var;

void *kmem_start        = &kmem_start_var;        
void *kmem_text_start   = &kmem_text_start_var;
void *kmem_rodata_start = &kmem_rodata_start_var;
void *kmem_data_start   = &kmem_data_start_var;
void *kmem_bss_start    = &kmem_bss_start_var;
void *kmem_end          = &kmem_end_var;


/* Total amount of system memory (in kB) 
 * This is obtaied from the bootloader by main()
 */ 
uint32_t mem_total;     


page_dir_t      kpage_dir;       /* The kernel's page directory (Physical) */
page_dir_t      cpage_dir;       /* Page directory currently in use (Physical) */
page_dir_t      vpage_dir;       /* Virtual address of current page dir (Virtual) */

void            *vpage_tables;   /* Virtual address of array of page tables */
void            *memstack1_base; /* Stack of pages in memory at < 16M */
uint32_t        *memstack1_ptr;  /* Points to current position in memstack1*/

void            *memstack2_base; /* Stack of pages in memory at >= 16M */
uint32_t        *memstack2_ptr;  /* Points to current position in memstack2*/

heap_t          *kheap;     /* The kernel's heap */

char            mem_err[ERR_STR_SIZE];


/*  ----------------------------------------------------
 *  Function:       mem_init
 *  --------------------------------------------------*/
void mem_init(){

    /*
     * We are going to initialise two stacks of available
     * pages. One is for memory before 16M, the other for
     * after. The reason for having two stacks is that 
     * sometimes we may require some memory that is actually
     * before the 16M mark.
     *
     * The stacks will contain 32 bit pointers to physical
     * pages and will be called memstack1 (<16M)
     * and memstack2 (>=16M).
     */

    uint32_t i;
    uint32_t j;

    /* -----------------------
     *  memstack1
     * ---------------------*/
    
    /* Get a 4k aligned end of kernel memory (in 4k chunks)*/
    uint32_t kmem_end_4k = 1 + ((uint32_t) kmem_end)/4096;

    /* Find how many 4k pages we can have < 16MB */
    uint32_t mem_lt_16  = 4096 - kmem_end_4k;

    /* Find space required for stack (in 4k chunks) 
     * size in bytes = mem_lt_16 * 4, so
     * size in 4ks   = 1 + mem_lt_16/1024
     */
    unsigned char memstack1_size  = 1 + mem_lt_16/1024;

    /* But now we don't have room for as many pages */
    mem_lt_16   -= memstack1_size;

    /*
     * Make the stack!
     */
    memstack1_base = (void *) (4096 * (kmem_end_4k + memstack1_size));
    memstack1_ptr   = (uint32_t *) memstack1_base;

    /* Add the pages */
    for( i=0 ; i<mem_lt_16; i++ ){
        *(--memstack1_ptr)  = (uint32_t) memstack1_base + i*4096;
    }

    /* -----------------------
     *  memstack2
     * ---------------------*/

    /* Find how many 4k pages we can have >= 16MB.
     * note that mem_total is already in kB
     * */
    uint32_t mem_ge_16  = (mem_total - 0x4000) / 4;

    /* Find space required for stack (in 4k chunks) 
     * size in bytes = mem_ge_16 * 4, so
     * size in 4ks   = 1 + mem_ge_16/1024
     */
    uint32_t memstack2_size  = 1 + mem_ge_16/1024;

    /* But now we don't have room for as many pages 
     */
    mem_ge_16   -= memstack2_size;

    /*
     * Make the stack!
     */
    memstack2_base = (void *) (4096 * (4096 + memstack2_size));
    memstack2_ptr   = (uint32_t *) memstack2_base;

    /* Add the pages 
     */
    for( i=0 ; i<mem_ge_16; i++ ){
        *(--memstack2_ptr)  = (uint32_t) memstack2_base + i*4096;
    }

    /*
     * Now to make the page directory and some initial page tables.
     *
     * All the memory up to the end of the kernel (including memstack1)
     * will be identity mapped.
     *
     * memstack2 will then be mapped so that it appears to be straight
     * after memstack1.
     *
     * The total number of pages to be identity mapped is 
     * kmem_end_4k + memstack1_size.
     */

    kpage_dir           = (page_dir_t) *(memstack1_ptr++);
    int kpage_dir_ind   = 0;

    /* reserve an initial page table from memstack1 */
    page_tbl_t pt               = (page_tbl_t) *(memstack1_ptr++);
    kpage_dir[kpage_dir_ind++]  = (uint32_t) pt | 3;

    for( i=0, j=0; i < kmem_end_4k + memstack1_size; i++){

        if(j == 1024){
            /* New page table */
            pt  = (page_tbl_t) *(memstack1_ptr++);
            j   = 0;

            /*
             * Add the page table to the page directory
             *
             * Flag bits are as for page tables (below), except:
             * bit 6 = 0    => always zero
             * bit 7 = 0    => 4kb page size
             * bit 8 = 0    => ignored
             */
            kpage_dir[kpage_dir_ind++]  = (uint32_t) pt | 3;
        }

        /* The page table entry is the page address ORed with 11 flag bits.
         *
         * bit 0 = 1    => page present
         * bit 1 = 1    => read/write 
         * bit 2 = 0    => supervisor page
         * bit 3 = 0    => disable write through chaching
         * bit 4 = 0    => don't disable cache
         * bit 5 = 0    => 'accessed' bit
         * bit 6 = 0    => 'dirty' bit
         * bit 7 = 0    => always zero
         * bit 8 = 0    => 'global' bit - let the cpu use tlb.
         * bits 9-11 are ignored - available to os.
         */
        pt[j++]   = (4096 * i) | 3;
    }

    /* Make entries for memstack2 */
    for( i=0; i < memstack2_size; i++){

        if(j == 1024){
            /* New page table */
            pt  = (page_tbl_t) *(memstack1_ptr++);
            j   = 0;

            /* Add to page directory */
            kpage_dir[kpage_dir_ind++]  = (uint32_t) pt;
        }

        /* memstack2 starts at 16MB = 4096 * 4096 */
        pt[j++]   = (4096 * (4096 + i)) | 3;
    }

    /* Initialise the rest of the last page table to zeros */
    for(; j<1024; j++){
        pt[j]   = 0;
    }

    /* Initialise the rest of the page dir to zeros,
     * except the last entry which will point to the
     * page directory itself. This is so that we have
     * virtual addresses (the last 4MB of address space)
     * for the page tables.
     */
    for( i=kpage_dir_ind; i<1023; i++){
        kpage_dir[i]    = 0;
    }

    kpage_dir[1023] = (uint32_t) kpage_dir | 3;

    /* 
     * Enable Paging!
     */

    set_page_dir(kpage_dir);

    asm volatile(
        "mov %%cr0, %%eax      \n\t"
        "or 0x80000000, %%eax  \n\t" // bit 31 is the paging bit
        "mov %%eax, %%cr0      \n\t"
        : : : "eax"
    );

    /* We mapped memstack2 to be right after memstack1
     * so we have to update the pointers.
     * It has moved from 16M+something to memstack1_base+something
     */
    memstack2_base  += (uint32_t) memstack1_base - 4096*4096;

    /* remember memstack2_ptr is a uint32_t ptr */
    memstack2_ptr   += ((uint32_t) memstack1_base)/4 - 1024*4096;

    /* The page tables are now found in the last 4MB of virtual memory.
     * The page directory is the last 4kB of virtual memory
    */ 
    vpage_tables= (void *) 0xFFC00000 ;    // 4GB - 4MB
    vpage_dir   = (uint32_t *) 0xFFFFF000; // 4GB - 4kB

    /*
     * We are going to use everything after memstack 2 in the
     * virtual address space as the kernel heap.
     *
     * First we need to allocate some physical memory for it.
     */
    if(!kpsalloc_at_v((uint32_t) memstack2_base, 300)){
        PANIC("Failed to allocate memory for kernel heap");
    }

    kheap  = heap_place(memstack2_base, 300, 1, 0);

    if(kheap == NULL){
        PANIC(heap_err);
    }

}

/*  ----------------------------------------------------
 *  Function:       set_page_dir
 *  --------------------------------------------------*/
void set_page_dir(page_dir_t pd){

    cpage_dir   = pd;

    asm volatile(
        "mov %0, %%cr3"
        : : "r" (pd)
    );
}

/*  ----------------------------------------------------
 *  Function:       addr_v2p
 *
 *  Desc:           Converts a virtual address to a
 *                  physical address.
 *  --------------------------------------------------*/
//uint32_t addr_v2p(uint32_t v){
//
//    /*
//     * Erm...Draw a picture.
//     *
//     * Remember that the last entry in the page directory
//     * maps to itself 
//     */
//
//    uint16_t page_offset    = v % 4096;
//    v   >>= 10;
//    v    &= 0xFFFFFFFC;
//    v    |= 0xFFC00000;
//    return *((uint32_t *) v) + page_offset;
//}

/*  ----------------------------------------------------
 *  Function:       addr_v2p
 *
 *  Desc:           Converts a virtual address to a
 *                  physical address.
 *  --------------------------------------------------*/
uint32_t addr_v2p(void *v){

    uint32_t vi = (uint32_t) v;

    // TODO: Error checking - check whether page table/page is present before looking
    //       in the page table entry.

    uint32_t pd_index   = vi >> 22;           // Highest 10 bits of virtual address
    uint32_t pt_index   = (vi >> 12) & 0x3FF; // Next 10 bits of virtual address
    uint32_t p_index    = vi & 0xFFF;         // Lowest 12 bits of virtual address

    // The page directory is the last entry in the page directory,
    // so the last 4MB of virtual memory is mapped to the page tables.
    //
    //       *pte   =               start of pts + (size of pt * pd_index) + (size of pte * pt_index)
    uint32_t *pte   = (uint32_t *) ( 0xFFC00000 | (pd_index << 12) | (pt_index << 2) );

    //      phys addr of page  + offset into page
    return (*pte & 0xFFFFF000) + p_index;
}


/*  ----------------------------------------------------
 *  Function:       ms1_ppalloc
 *
 *  Desc:           Allocates a free page of pysical
 *                  memory from memstack1.
 *  --------------------------------------------------*/
uint32_t ms1_ppalloc(){

    if(memstack1_ptr == memstack1_base){
        /* Out of memstack1 memory! */
        return 0;
    }

    return *(memstack1_ptr++);
}


/*  ----------------------------------------------------
 *  Function:       ms2_ppalloc
 *
 *  Desc:           Allocates a free page of pysical
 *                  memory from memstack2.
 *  --------------------------------------------------*/
uint32_t ms2_ppalloc(){

    if(memstack2_ptr == memstack2_base){
        /* Out of memstack2 memory! */
        return 0;
    }

    return *(memstack2_ptr++);
}

/*  ----------------------------------------------------
 *  Function:       ppalloc
 *
 *  Desc:           Allocates a free page of physical
 *                  memory.
 *  --------------------------------------------------*/
uint32_t ppalloc(){

    uint32_t retval = ms2_ppalloc();

    if(retval){
        return retval;
    }

    retval  = ms1_ppalloc();

    if(retval){
        return retval;
    }

    return 0;
}


/*  ----------------------------------------------------
 *  Function:       ppfree
 *
 *  Desc:           Frees a page of physical memory.
 *  --------------------------------------------------*/
void ppfree(uint32_t addr){
    if(addr < 0x1000000){
        *(--memstack1_ptr)  = addr;
    }
    else{
        *(--memstack2_ptr)  = addr;
    }
}


/*  ----------------------------------------------------
 *  Function:       pmap
 *
 *  Desc:           Maps a virtual page to a physical
 *                  page.
 *  --------------------------------------------------*/
bool pmap(uint32_t phys, uint32_t virt, bool rw, bool user){
    
    /*
     * Check that addresses are page aligned
     */
    if(phys % 4096 || virt % 4096){
        return 0;
    }

    int pd_ind  = virt >> 22;           // First 10 bits of 32 bit address
    int pt_ind  = (virt >> 12) & 0x3FF; // Second 10 bits of 32 bit address

    uint32_t *pt        = vpage_tables + 4096*pd_ind;
    uint32_t *pt_entry  = pt + pt_ind;

    /* 
     * Check if the page table for this page needs to be created
     * Bit 0 of a page dir entry indicates presence of page table
     */
    if(!(vpage_dir[pd_ind] & 1)){

        /* Allocate physical page for page table
         */
        uint32_t phys_pt    = ppalloc();
        if(phys_pt == NULL){
            return 0;
        }

        /* Add entry in the page directory
         */
        char pt_flags   = 1 + (rw? 2 : 0) + (user? 4 : 0);
        vpage_dir[pd_ind]   = phys_pt | pt_flags;

        /* Initialise page table to zeros
         */
        memset(pt, 0, 4096);
    }

    /* Make sure that the page is not already mapped.
     */
    if(*pt_entry & 1){
        return 0;
    }

    /* Set the page table entry.
     */
    *pt_entry   = phys | (1 + (rw? 2 : 0) + (user? 4 : 0));

    return 1;
}


/*  ----------------------------------------------------
 *  Function:       palloc_at_v
 *
 *  Desc:           Assigns a new physical page to the
 *                  given virtual page address.
 *  --------------------------------------------------*/
bool palloc_at_v(uint32_t virt, bool rw, bool user){
    uint32_t phys   = ppalloc();
    if(!phys){
        return 0;
    }
    if(! pmap(phys, virt, rw, user)){
        return 0;
    }
    return 1;
}

/*  ----------------------------------------------------
 *  Function:       kpalloc_at_v
 *
 *  Desc:           palloc_at_v wrapper for kernel.
 *  --------------------------------------------------*/
bool kpalloc_at_v(uint32_t virt){
    return palloc_at_v(virt, 1, 0);
}

/*  ----------------------------------------------------
 *  Function:       psalloc_at_v
 *
 *  Desc:           Assigns new physical pages to the
 *                  given virtual page address.
 *  --------------------------------------------------*/
bool psalloc_at_v(uint32_t virt, bool rw, bool user, int num){
    int i;
    for(i=0; i<num; i++){
        if(!palloc_at_v(virt, rw, user)){
            return 0;
        }
        virt += 4096;
    }

    return 1;
}

/*  ----------------------------------------------------
 *  Function:       kpsalloc_at_v
 *
 *  Desc:           psalloc_at_v wrapper for kernel.
 *  --------------------------------------------------*/
bool kpsalloc_at_v(uint32_t virt, int num){
    return psalloc_at_v(virt, 1, 0, num);
}

/*  ----------------------------------------------------
 *  Function:       kpfree
 *
 *  Desc:           Frees a page of virtual/physical memory
 *                  refered to by virtual address.
 *                  
 *                  For the kernel - no permission checking.
 *  --------------------------------------------------*/
bool kpfree_at_v(uint32_t addr){

    /* Check that address is page aligned
     */
    if(addr % 4096){
        return 0;
    }

    int pd_ind  = addr >> 22;                // First 10 bits of 32 bit address
    int pt_ind  = (addr >> 12) & 0x0000FFFF; // Second 10 bits of 32 bit address

    uint32_t *pd_entry  = &vpage_dir[pd_ind];

    uint32_t *pt        = vpage_tables + 4096*pd_ind;
    uint32_t *pt_entry  = pt + pt_ind;

    /* Check that the address was actually in use
     */
    if(!(*pd_entry & 1  &&  *pt_entry & 1)){
        return 0;
    }

    /* Free the physical page
     */
    ppfree(*pt_entry & 0xFFFFF000);

    /* Remove the page table entry
     */
    *pt_entry   = 0;

    return 1;
}

/*  ----------------------------------------------------
 *  Function:       kmalloc
 *
 *  Allocate memory from kernel heap.
 *  --------------------------------------------------*/
void *kmalloc(uint32_t size){
    void *ptr   = heap_alloc(kheap, size, 0);
    if(ptr == NULL){
        strcpy(mem_err, heap_err);
    }

    return ptr;
}

/*  ----------------------------------------------------
 *  Function:       kpalloc
 *
 *  Allocate page of memory from the kernel heap.
 *  Returns virtual address as pointer.
 *  --------------------------------------------------*/
void *kpalloc(){
    void *ptr   = heap_palloc(kheap);
    if(ptr == NULL){
        strcpy(mem_err, heap_err);
    }
    return ptr;
}

/*  ----------------------------------------------------
 *  Function:       kpsalloc
 *
 *  Allocate pages of memory from the kernel heap.
 *  Returns virtual address as pointer.
 *  --------------------------------------------------*/
void *kpsalloc(uint16_t pages){
    void *ptr   = heap_psalloc(kheap, pages);
    if(ptr == NULL){
        strcpy(mem_err, heap_err);
    }
    return ptr;
}


/*  ----------------------------------------------------
 *  Function:       kfree
 *
 *  Free memory allocated from the kernel heap.
 *  --------------------------------------------------*/
bool kfree(void *addr){
    if(!heap_free(kheap, addr)){
        strcpy(mem_err, heap_err);
        return 0;
    }
    return 1;
}

/*  ----------------------------------------------------
 *  Function:       get_page_table_entry
 *  --------------------------------------------------*/
uint32_t get_page_table_entry_flags(uint32_t addr){

    int pd_index    = addr >> 22;
    int pt_index    = (addr >> 12) & 0x3FF;

    uint32_t pd_entry   = vpage_dir[pd_index];

    if(!(pd_entry & 1)){
        return 0;
    }

    return *((uint32_t *) (vpage_tables + 4096*pd_index + 4*pt_index));
}

/*  ----------------------------------------------------
 *  Function:       set_page_table_entry_flags
 *
 *  Sets flags of page table entry for a virtual
 *  address's page.
 *  --------------------------------------------------*/
bool set_page_table_entry_flags(page_dir_t pd, uint32_t addr, uint32_t flags){
    int pd_index    = addr >> 22;
    int pt_index    = (addr >> 12) & 0x3FF;

    uint32_t pd_entry   = pd[pd_index];

    // Check page table is present
    //
    if(!(pd_entry & 1)){
        return 0;
    }

    uint32_t *entry = (uint32_t *) (pd + 4096*pd_index + 4*pt_index);
    *entry  &= 0xFFFFF000;
    *entry  |= flags;
    return 1;
}
