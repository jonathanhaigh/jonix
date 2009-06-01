/* ---------------------------------------------------
* Jonix
*
* syscall.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "system.h"
#include "isr.h"

extern void syscall_init();
extern void syscall_enter(); // in boot.asm
extern void syscall_handler(isr_stack_kernel_t *stack);

#endif
