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
 *  typedef struct:         isr_stack_t
 *
 *  This defines what the stack looks like after an ISR 
 *  was running 
 *  --------------------------------------------------*/
typedef struct{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((__packed__)) isr_stack_t;

/*
 * Functions
 */
extern void isr_init();

#endif
