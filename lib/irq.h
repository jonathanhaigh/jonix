/* ---------------------------------------------------
* Jonix
*
* irq.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _IRQ_H
#define _IRQ_H

#include "system.h"
#include "isr.h"

/* 
 * Functions
 */
extern void irq_init();
extern void irq_set_handler(int irq, void (*handler)(isr_stack_t *r) );
extern void irq_unset_handler(int irq);

#endif
