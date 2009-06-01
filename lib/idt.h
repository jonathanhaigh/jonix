/* ---------------------------------------------------
* Jonix
*
* idt.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _IDT_H
#define _IDT_H

#include "system.h"

/*
    Adapted from http://osdever.net/bkerndev/Docs/idt.htm
*/

/*  ----------------------------------------------------
 *  Typedef struct:       idt_entry_t
 *  --------------------------------------------------*/
typedef struct {
    uint16_t base_low;
    uint16_t sel;        /* Our kernel segment goes here! */
    uint8_t always0;     /* This will ALWAYS be set to 0! */
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;


/*  ----------------------------------------------------
 *  Typedef struct:       idt_ptr_t
 *  --------------------------------------------------*/
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;


/*
 * Functions
 */
extern void idt_init();
extern void idt_set_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
