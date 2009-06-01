/* ---------------------------------------------------
* Jonix
*
* sched.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _SCHED_H
#define _SCHED_H

#include "system.h"
#include "isr.h"

extern void resched(void *stack);
extern void resched_no_int();

#endif
