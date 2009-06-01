/* ---------------------------------------------------
* Jonix
*
* mem.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _MEM_H
#define _MEM_H

#include "system.h"
#include "util/oarray.h"

#define MALLOC_PANIC(var) if(var == NULL){ PANIC("Out of memory."); }

// KEEP THIS CONSISTENT WITH CONSTANT IN boot.asm
#define KERNEL_STACK_SIZE 0x4000

/*
 * Some heap constants.
 */
#define HEAP_INDEX_SIZE 0x20000     // 8k entries in a heap
#define MIN_HEAP_SIZE 0x10000       // 4k min heap
#define HEAP_HDR_MAGIC 0x8e99da12   // Magic number for error checking heap headers
#define HEAP_FTR_MAGIC 0x981a7cb4   // Magic number for error checking heap footers

/*
 * Typedefs for paging structures.
 */
typedef uint32_t * page_dir_t;
typedef uint32_t * page_tbl_t;

/*
 * Heap structures.
 */

/*  ----------------------------------------------------
 *  Typedef struct:     heap_t
 *  --------------------------------------------------*/
typedef struct {
    void        *start;
    void        *end;
    oarray_t    index;
    bool        user;
    bool        rw;
} heap_t;


/*  ----------------------------------------------------
 *  Typedef struct:     heap_hdr_t
 *  --------------------------------------------------*/
typedef struct {
    bool        hole;
    uint32_t    size;
    uint32_t    magic;
} heap_hdr_t;


/*  ----------------------------------------------------
 *  Typedef struct:     heap_ftr_t
 *  --------------------------------------------------*/
typedef struct {
    heap_hdr_t  *hdr;
    uint32_t     magic;
} heap_ftr_t;


/*
 * Variables from mem.c
 */
extern char         mem_err[ERR_STR_SIZE];  // mem.c error string

extern page_dir_t   kpage_dir;          // The kernel's page directory (Physical)
extern page_dir_t   cpage_dir;          // Page directory currently in use (Physical)
extern page_dir_t   vpage_dir;          // virt addr of current page directory
extern void         *kmem_start;         
extern void         *kmem_text_start; 
extern void         *kmem_rodata_start;
extern void         *kmem_data_start;
extern void         *kmem_bss_start;
extern void         *kmem_end;
extern void         *kmem_stack;
extern uint32_t     mem_total;

/*
 * Functions from mem.c
 */
extern void     mem_init();
extern void     set_page_dir(page_dir_t pd);
extern uint32_t ms1_ppalloc();
extern uint32_t ms2_ppalloc();
extern uint32_t addr_v2p(void  *v);
extern uint32_t ppalloc();
extern void     ppfree(uint32_t addr);
extern bool     pmap(uint32_t phys, uint32_t virt, bool rw, bool user);
extern bool     palloc_at_v(uint32_t virt, bool rw, bool user);
extern bool     kpalloc_at_v(uint32_t virt);
extern bool     psalloc_at_v(uint32_t virt, bool rw, bool user, int num);
extern bool     kpsalloc_at_v(uint32_t virt, int num);
extern bool     kpfree_at_v(uint32_t addr);
extern void *   kmalloc(uint32_t size);
extern void *   kpalloc();
extern void *   kpsalloc(uint16_t pages);
extern bool     kfree(void *addr);
extern uint32_t get_page_table_entry(uint32_t addr);
extern bool     set_page_table_entry_flags(page_dir_t pd, uint32_t addr, uint32_t flags);

/* 
 * Variables from heap.c
 */
extern char     heap_err[ERR_STR_SIZE];
extern heap_t   *kheap; // The kernel's heap.

/*
 * Functions from heap.c
 */
extern heap_t *   heap_place(void *start, int no_pages, bool rw, bool user);
extern bool       heap_extend(heap_t *heap, int no_pages);
extern void *     heap_alloc(heap_t *heap, uint32_t size, bool page_align);
extern void *     heap_palloc(heap_t *heap);
extern void *     heap_psalloc(heap_t *heap, uint16_t pages);
extern bool       heap_free(heap_t *heap, void *addr);
extern char       heap_hdr_t_cmp_size(list_item_t a, list_item_t b);
extern void       heap_print_info(heap_t * heap);
extern void       heap_print_index(heap_t * heap);

#endif
