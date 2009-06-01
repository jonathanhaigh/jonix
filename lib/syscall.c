/* ---------------------------------------------------
* Jonix
*
* syscall.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "syscall.h"
#include "time.h"
#include "idt.h"

#define SYSCALL_NO 1

isr_handler_t syscall_handlers[SYSCALL_NO];

/*  ----------------------------------------------------
 *  Function:       syscall_init
 *  --------------------------------------------------*/
void syscall_init(){

    idt_set_entry(0x80, (uint32_t) syscall_enter, 0x08, 0x8E);
    
    syscall_handlers[0] = syscall_sleep;
}


/*  ----------------------------------------------------
 *  Function:       syscall_handler
 *  --------------------------------------------------*/
void syscall_handler(isr_stack_kernel_t *stack){
    
    if(stack->eax >= SYSCALL_NO){
        // Unrecognised system call
        //
        // TODO: Deal with this.
        //
        return;
    }

    (syscall_handlers[stack->eax])(stack);

}
