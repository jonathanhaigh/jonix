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
#include "thr.h"

/*  ----------------------------------------------------
 *  typedef struct:         sleep_entry_t
 *  --------------------------------------------------*/
typedef struct{
    thread_t *thread;
    uint32_t time;
} sleep_entry_t;


/*
 * Functions
 */
extern void time_init();
extern void time_irq_handler(void *stack);
extern void syscall_sleep(void *stack);

#endif
