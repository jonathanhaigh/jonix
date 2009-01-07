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

/*  ----------------------------------------------------
 *  typedef struct:         gdt_entry_t
 *  --------------------------------------------------*/
typedef struct{
   uint16_t limit_low;
   uint16_t base_low;
   uint8_t  base_mid;
   uint8_t  access;
   uint8_t  granularity;
   uint8_t  base_high;
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

extern void gdt_flush();
extern void gdt_init();
extern void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#endif
