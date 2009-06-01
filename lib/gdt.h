/* ---------------------------------------------------
* Jonix
*
* gdt.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _GDT_H
#define _GDT_H

#include "system.h"

#define GDT_INDEX_KERN_CODE  1   // Index in GDT of the kernel's code segment
#define GDT_INDEX_KERN_DATA  2   // Index in GDT of the kernel's data segment
#define GDT_INDEX_KERN_TSS   3   // Index in GDT of the kernel's TSS
#define GDT_INDEX_USER_CODE  4   // Userland code segment
#define GDT_INDEX_USER_DATA  5   // Userland data segment
#define GDT_SIZE             6

// Creates a segment descriptor for the indexth entry in the GDT for privelege level priv
//
#define GDT_SEG_DESCRIPTOR(index, priv) (((index) << 3 | (priv) << 1) & 0xFFFF)


// These are offsets to the parts of the low and high 32-bits of a GDT entry
//
#define GDT_ENTRY_L_LIMIT_LOW   0   // Length of segment
#define GDT_ENTRY_L_BASE_LOW    16  // Start of segment
#define GDT_ENTRY_H_BASE_MID    0 
#define GDT_ENTRY_H_TYPE        8   // Type of segment
#define GDT_ENTRY_H_DPL         13  // Descriptor privelege level
#define GDT_ENTRY_H_PRESENT     15  // Segment present? 1 : 0
#define GDT_ENTRY_H_LIMIT_HIGH  16
#define GDT_ENTRY_H_AVAILABLE   20  // Available for use by OS
#define GDT_ENTRY_H_64_BIT      21  // is segment 64 bit code?
#define GDT_ENTRY_H_32_BIT      22  // is segment 16/32 bit code? 32: 1
#define GDT_ENTRY_H_GRANULARITY 23  // Scaling of seg limit field. No scaling: 0, 4K scaling: 1
#define GDT_ENTRY_H_BASE_HIGH   24

#define GDT_ENTRY_TYPE_DATA_RW  ((BIT(4) | BIT(1)) & 0x1F)
#define GDT_ENTRY_TYPE_DATA_RO  (BIT(4) & 0x1F)
#define GDT_ENTRY_TYPE_STACK    ((BIT(4) | BIT(1) | BIT(2)) & 0x1F)
#define GDT_ENTRY_TYPE_CODE_XO  ((BIT(4) | BIT(3)) & 0x1F)
#define GDT_ENTRY_TYPE_CODE_XR  ((BIT(4) | BIT(3) | BIT(1)) & 0x1F)
#define GDT_ENTRY_TYPE_TSS      ((BIT(3) | BIT(0)) & 0x1F)


/*  ----------------------------------------------------
 *  typedef struct:         gdt_entry_t
 *  --------------------------------------------------*/
typedef struct{
    uint32_t low;
    uint32_t high;
} __attribute__((__packed__)) gdt_entry_t;


/*  ----------------------------------------------------
 *  typedef struct:         gdt_ptr_t
 *  --------------------------------------------------*/
typedef struct{
    uint16_t    limit;
    uint32_t    base;
} __attribute__((__packed__)) gdt_ptr_t;


/*  ----------------------------------------------------
 *  typedef struct:         tss_t
 *  --------------------------------------------------*/
typedef struct{
    uint16_t link;
    uint16_t link_h;

    uint32_t esp0;
    uint16_t ss0;
    uint16_t ss0_h;

    uint32_t esp1;
    uint16_t ss1;
    uint16_t ss1_h;

    uint32_t esp2;
    uint16_t ss2;
    uint16_t ss2_h;

    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;

    uint32_t esp;
    uint32_t ebp;

    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t es_h;

    uint16_t cs;
    uint16_t cs_h;

    uint16_t ss;
    uint16_t ss_h;

    uint16_t ds;
    uint16_t ds_h;

    uint16_t fs;
    uint16_t fs_h;

    uint16_t gs;
    uint16_t gs_h;

    uint16_t ldt;
    uint16_t ldt_h;

    uint16_t trap;
    uint16_t iomap;
} __attribute__((__packed__)) tss_t;

extern tss_t tss_kernel;

extern void gdt_flush();
extern void gdt_init();
extern void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t type, bool dpl);

#endif
