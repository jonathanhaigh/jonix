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
#include "c/string.h"
#include "mem.h"
#include "thr.h"

// TSS for kernel
//
tss_t tss_kernel;

// The GDT
//
// TODO: put this on an 8 byte boundary for better performance.
//
gdt_entry_t gdt[GDT_SIZE];

// The gdt_ptr_t struct
//
gdt_ptr_t   gdt_ptr;

/*  ----------------------------------------------------
 *  Function:       gdt_set_entry
 *  --------------------------------------------------*/
void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t type, bool dpl){

    gdt[index].low  =   (base & 0xFFFF)     << GDT_ENTRY_L_BASE_LOW
                      | (limit & 0xFFFF)    << GDT_ENTRY_L_LIMIT_LOW;

    gdt[index].high =   (base >> 16 & 0xFF) << GDT_ENTRY_H_BASE_MID
                      | (base >> 24 & 0xFF) << GDT_ENTRY_H_BASE_HIGH
                      | (limit >> 16 & 0xF) << GDT_ENTRY_H_LIMIT_HIGH
                      | (type & 0x1F)       << GDT_ENTRY_H_TYPE
                      | (dpl & 3)           << GDT_ENTRY_H_DPL
                      | 1                   << GDT_ENTRY_H_PRESENT
                      | 0                   << GDT_ENTRY_H_AVAILABLE
                      | 0                   << GDT_ENTRY_H_64_BIT
                      | 1                   << GDT_ENTRY_H_32_BIT
                      | 1                   << GDT_ENTRY_H_GRANULARITY;

}

/*  ----------------------------------------------------
 *  Function:       gdt_init
 *  --------------------------------------------------*/
void gdt_init(){

    // Setup the GDT pointer and limit
    //
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_SIZE) - 1;
    gdt_ptr.base = (uint32_t) &gdt;

    // Null descriptor
    //
    // Put this in unused data segment registers to make sure exceptions
    // are created when accessing non-existent data segments
    //
    gdt_set_entry(0, 0, 0, 0, 0);

    // Code segments - the whole 4G of memory.
    //
    gdt_set_entry(GDT_INDEX_KERN_CODE, 0, 0xFFFFFFFF, GDT_ENTRY_TYPE_CODE_XR, 0);
    gdt_set_entry(GDT_INDEX_USER_CODE, 0, 0xFFFFFFFF, GDT_ENTRY_TYPE_CODE_XR, 0);

    // Data segments - the whole 4G of memory.
    // These are also used as the stack segments.
    //
    gdt_set_entry(GDT_INDEX_KERN_DATA, 0, 0xFFFFFFFF, GDT_ENTRY_TYPE_DATA_RW, 0);
    gdt_set_entry(GDT_INDEX_USER_DATA, 0, 0xFFFFFFFF, GDT_ENTRY_TYPE_DATA_RW, 0);

    // TSS descriptor for kernel
    //
    gdt_set_entry(GDT_INDEX_KERN_TSS, (uint32_t) &tss_kernel, sizeof(tss_t)-1, GDT_ENTRY_TYPE_TSS, 0);

    // Initialise the kernel TSS to zeros
    //
    memset(&tss_kernel, 0, sizeof(tss_t));

    // Set kernel stack info in the kernel TSS 

    tss_kernel.ss0  = GDT_SEG_DESCRIPTOR(GDT_INDEX_KERN_TSS, 0);
    
    // We don't set the stack pointer until we are just about to load the TSS descriptor
    // into the processor (done in gdt_flush()).


    // Install the GDT
    //
    gdt_flush();

}
