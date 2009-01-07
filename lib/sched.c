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

void resched(isr_stack_t regs){

    return;

    /* Save the proc registers into the current thread's
     * descriptor.
     */
    thr_current->cs     = regs.cs;
    thr_current->ds     = regs.ds;
    thr_current->es     = regs.es;
    thr_current->fs     = regs.fs;
    thr_current->gs     = regs.gs;
    thr_current->ss     = regs.ss;
    thr_current->esp    = regs.useresp;
    thr_current->eip    = regs.eip;
    thr_current->edi    = regs.edi;
    thr_current->esi    = regs.esi;
    thr_current->ebp    = regs.ebp;
    thr_current->ebx    = regs.ebx;
    thr_current->edx    = regs.edx;
    thr_current->ecx    = regs.ecx;
    thr_current->eax    = regs.eax;
    thr_current->eflags = regs.eflags;

    list_add(&thr_ready, thr_current);

    thr_current = list_del(&thr_ready, 0);

}
