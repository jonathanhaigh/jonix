/* ---------------------------------------------------
* Jonix
*
* thr.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _THR_H
#define _THR_H

#include "system.h"
#include "isr.h"

extern void resched(isr_stack_t regs);

#endif
