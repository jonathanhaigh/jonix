/* ---------------------------------------------------
* Jonix
*
* time.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _TIME_H
#define _TIME_H

#include "system.h"
#include "isr.h"

/*
 * Functions
 */
extern void time_init();
extern void time_irq_handler(isr_stack_t *r);
extern void sleep(int no, char units);

#endif
