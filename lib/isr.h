/* ---------------------------------------------------
* Jonix
*
* isr.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _ISR_H
#define _ISR_H

#include "system.h"

/*  ----------------------------------------------------
 *  typedef struct:         isr_stack_user_t
 *
 *  This defines what the stack looks like after an ISR 
 *  was running after a usermode thread was interrupted.
 *  --------------------------------------------------*/
typedef struct{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((__packed__)) isr_stack_user_t;


/*  ----------------------------------------------------
 *  typedef struct:         isr_stack_kernel_t
 *
 *  This defines what the stack looks like after an ISR 
 *  was running after a kernel model thread was interrupted.
 *  --------------------------------------------------*/
typedef struct{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} __attribute__((__packed__)) isr_stack_kernel_t;


/*  ----------------------------------------------------
 *  typedef struct:         isr_handler_t
 *  --------------------------------------------------*/
typedef void (*isr_handler_t)(void *);


/*
 * Functions
 */
extern void isr_init();
extern void isr_stack_user_print(isr_stack_user_t *stack);
extern void isr_stack_kernel_print(isr_stack_kernel_t *stack);
extern void isr_handler(isr_stack_kernel_t *stack);

#endif
