/* ---------------------------------------------------
* Jonix
*
* sched.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "thr.h"
#include "sched.h"
#include "isr.h"
#include "irq.h"
#include "gdt.h"

void resched(void *stack){

    thr_switch(stack, olist_del(&thr_ready, 0));
}

void resched_no_int(){
    asm volatile("                                          \
        pushf;                                              \
        push %%cs;                                          \
        push $1f;                                           \
        push $0;                                            \
        push $0;                                            \
        pusha;                                              \
        push %%ds;                                          \
        push %%es;                                          \
        push %%fs;                                          \
        push %%gs;                                          \
        mov %0, %%ax;                                       \
        mov %%ax, %%ds;                                     \
        mov %%ax, %%es;                                     \
        mov %%ax, %%fs;                                     \
        mov %%ax, %%gs;                                     \
        mov %%esp, %%eax;                                   \
        push %%eax;                                         \
        mov %1, %%eax;                                      \
        call %%eax;                                         \
        1:"
        :
        : "n" (GDT_SEG_DESCRIPTOR(GDT_INDEX_KERN_DATA, 0)),
          "r" (&resched)
        : "memory"
     );
}
