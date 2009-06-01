/* ---------------------------------------------------
* Jonix
*
* idt.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

/*
    Taken from http://osdever.net/bkerndev/Docs/idt.htm
*/
#include "idt.h"
#include "c/string.h"
#include "system_boot.h"

/*
 * Declare the IDT.
 * Create our IDT pointer.
 */
idt_entry_t idt[256];
idt_ptr_t idtp;


/*  ----------------------------------------------------
 *  Function:       idt_set_entry
 *  --------------------------------------------------*/
void idt_set_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){
    idt[num].base_low   = base & 0xFFFF;
    idt[num].base_high  = base >> 16;
    idt[num].sel        = sel;
    idt[num].flags      = flags;
    idt[num].always0    = 0;
}


/*  ----------------------------------------------------
 *  Function:       idt_init
 *  --------------------------------------------------*/
void idt_init(){
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (idt_entry_t) * 256) - 1;
    idtp.base = (uint32_t) &idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(idt_entry_t) * 256);

    /* Add any new ISRs to the IDT here using idt_set_entry */

    /* Points the processor's internal register to the new IDT */
    idt_load();
}

