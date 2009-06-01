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
extern void irq_set_handler(int irq, isr_handler_t handler);
extern void irq_unset_handler(int irq);
extern void irq_handler(isr_stack_kernel_t *stack);
extern void irq_end_of_interrupt(uint32_t int_no);

#endif
