/* ---------------------------------------------------
* Jonix
*
* gdt.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

/*
    Some of this taken from http://osdever.net/bkerndev/Docs/gdt.htm
*/

#include "gdt.h"
#include "string.h"
#include "mem.h"

// TSS for ring 0
//
tss_t tss_0;

// The GDT
//
gdt_entry_t gdt[4];

// The gdt_ptr_t struct
//
gdt_ptr_t   gdt_ptr;

/*  ----------------------------------------------------
 *  Function:       gdt_set_entry
 *  --------------------------------------------------*/
void gdt_set_entry(
    int index,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t granularity
){
    /* Setup the descriptor base address */
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_mid = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[index].granularity |= (granularity & 0xF0);
    gdt[index].access = access;
}

/*  ----------------------------------------------------
 *  Function:       gdt_init
 *  --------------------------------------------------*/
void gdt_init(){
    /* Setup the GDT pointer and limit */
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (uint32_t) &gdt;

    /* Our NULL descriptor */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* The second entry is our Code Segment. The base address
    *  is 0, the limit is 4GBytes, it uses 4KByte granularity,
    *  uses 32-bit opcodes, and is a Code Segment descriptor.
    *  Please check the table above in the tutorial in order
    *  to see exactly what each value means */
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* The third entry is our Data Segment. It's EXACTLY the
    *  same as our code segment, but the descriptor type in
    *  this entry's access byte says it's a Data Segment */
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* The fourth entry is a TSS descriptor (for ring 0)*/
    gdt_set_entry(3, (uint32_t) &tss_0, sizeof(tss_0)-1, 0x89, 0x00);

    /* Initialise the ring 0 TSS to zeros */
    memset(&tss_0, 0, sizeof(tss_0));

    /* Set stack info in the ring 0 TSS */

    tss_0.ss0    = 0x10;        // The descriptor used for the stack segment.

    tss_0.esp0   = (uint32_t) kmem_stack;   // The base of the kernel stack.
                                // This will be used as the stack pointer - 
                                // once we start multitasking this should be
                                // fine (I hope).

    /* Flush out the old GDT and install the new changes! */
    gdt_flush();

}
