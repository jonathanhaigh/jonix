/* ---------------------------------------------------
* Jonix
*
* isr.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "isr.h"
#include "idt.h"
#include "devs/scrn.h"

/* The isr_n routines are written in assembly, but they call the c function isr_handler
 */
extern void isr_0(); extern void isr_1(); extern void isr_2(); extern void isr_3();
extern void isr_4(); extern void isr_5(); extern void isr_6(); extern void isr_7();
extern void isr_8(); extern void isr_9(); extern void isr_10(); extern void isr_11();
extern void isr_12(); extern void isr_13(); extern void isr_14(); extern void isr_15();
extern void isr_16(); extern void isr_17(); extern void isr_18(); extern void isr_19();
extern void isr_20(); extern void isr_21(); extern void isr_22(); extern void isr_23();
extern void isr_24(); extern void isr_25(); extern void isr_26(); extern void isr_27();
extern void isr_28(); extern void isr_29(); extern void isr_30(); extern void isr_31();
extern void isr_32(); extern void isr_33(); extern void isr_34(); extern void isr_35();
extern void isr_36(); extern void isr_37(); extern void isr_38(); extern void isr_39();
extern void isr_30(); extern void isr_31();


char *exception_messages[] = {
    "Division By Zero Exception",
    "Debug Exception",
    "Non Maskable Interrupt Exception",
    "Breakpoint Exception",
    "Into Detected Overflow Exception",
    "Out of Bounds Exception",
    "Invalid Opcode Exception",
    "No Coprocessor Exception",
    "Double Fault Exception",
    "Coprocessor Segment Overrun Exception",
    "Bad TSS Exception",
    "Segment Not Present Exception",
    "Stack Fault Exception",
    "General Protection Fault Exception",
    "Page Fault Exception",
    "Unknown Interrupt Exception",
    "Coprocessor Fault Exception",
    "Alignment Check Exception",
    "Machine Check Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception",
    "Reserved Exception"
};

/*  ----------------------------------------------------
 *  Function:       isr_init
 *  --------------------------------------------------*/
void isr_init(){
    idt_set_entry(0, (uint32_t) isr_0, 0x08, 0x8E);
    idt_set_entry(1, (uint32_t) isr_1, 0x08, 0x8E);
    idt_set_entry(2, (uint32_t) isr_2, 0x08, 0x8E);
    idt_set_entry(3, (uint32_t) isr_3, 0x08, 0x8E);
    idt_set_entry(4, (uint32_t) isr_4, 0x08, 0x8E);
    idt_set_entry(5, (uint32_t) isr_5, 0x08, 0x8E);
    idt_set_entry(6, (uint32_t) isr_6, 0x08, 0x8E);
    idt_set_entry(7, (uint32_t) isr_7, 0x08, 0x8E);
    idt_set_entry(8, (uint32_t) isr_8, 0x08, 0x8E);
    idt_set_entry(9, (uint32_t) isr_9, 0x08, 0x8E);
    idt_set_entry(10, (uint32_t) isr_10, 0x08, 0x8E);
    idt_set_entry(11, (uint32_t) isr_11, 0x08, 0x8E);
    idt_set_entry(12, (uint32_t) isr_12, 0x08, 0x8E);
    idt_set_entry(13, (uint32_t) isr_13, 0x08, 0x8E);
    idt_set_entry(14, (uint32_t) isr_14, 0x08, 0x8E);
    idt_set_entry(15, (uint32_t) isr_15, 0x08, 0x8E);
    idt_set_entry(16, (uint32_t) isr_16, 0x08, 0x8E);
    idt_set_entry(17, (uint32_t) isr_17, 0x08, 0x8E);
    idt_set_entry(18, (uint32_t) isr_18, 0x08, 0x8E);
    idt_set_entry(19, (uint32_t) isr_19, 0x08, 0x8E);
    idt_set_entry(20, (uint32_t) isr_20, 0x08, 0x8E);
    idt_set_entry(21, (uint32_t) isr_21, 0x08, 0x8E);
    idt_set_entry(22, (uint32_t) isr_22, 0x08, 0x8E);
    idt_set_entry(23, (uint32_t) isr_23, 0x08, 0x8E);
    idt_set_entry(24, (uint32_t) isr_24, 0x08, 0x8E);
    idt_set_entry(25, (uint32_t) isr_25, 0x08, 0x8E);
    idt_set_entry(26, (uint32_t) isr_26, 0x08, 0x8E);
    idt_set_entry(27, (uint32_t) isr_27, 0x08, 0x8E);
    idt_set_entry(28, (uint32_t) isr_28, 0x08, 0x8E);
    idt_set_entry(29, (uint32_t) isr_29, 0x08, 0x8E);
    idt_set_entry(30, (uint32_t) isr_30, 0x08, 0x8E);
    idt_set_entry(31, (uint32_t) isr_31, 0x08, 0x8E);
}

/*  ----------------------------------------------------
 *  Function:       isr_handler
 *  --------------------------------------------------*/
void isr_handler(isr_stack_kernel_t *stack){

    if (stack->int_no < 32){
        printf("\n\n!!! KERNEL PANIC: %s\n", exception_messages[stack->int_no]);
        printf("Error code: 0x%X\n", stack->err_code);
        asm("hlt");
    }
}


/*  ----------------------------------------------------
 *  Function:       isr_stack_user_print
 *
 *  Prints the contents of an isr_stack_user_t struct.
 *  --------------------------------------------------*/
void isr_stack_user_print(isr_stack_user_t *stack){
    printf("GS = 0x%X,", stack->gs);
    printf("FS = 0x%X, ", stack->fs);
    printf("ES = 0x%X, ", stack->es);
    printf("DS = 0x%X\n", stack->ds);

    printf("EDI = 0x%X, ", stack->edi);
    printf("ESI = 0x%X, ", stack->esi);
    printf("EBP = 0x%X, ", stack->ebp);
    printf("ESP = 0x%X, ", stack->esp);
    printf("EBX = 0x%X, ", stack->ebx);
    printf("EDX = 0x%X, ", stack->edx);
    printf("ECX = 0x%X, ", stack->ecx);
    printf("EAX = 0x%X\n", stack->eax);

    printf("Int No = 0x%X, ", stack->int_no);
    printf("Err Code = 0x%X\n", stack->err_code);

    printf("EIP = 0x%X, ", stack->eip);
    printf("CS = 0x%X, ", stack->cs);
    printf("EFLAGS = 0x%X, ", stack->eflags);
    printf("User ESP = 0x%X, ", stack->useresp);
    printf("SS = 0x%X\n", stack->ss);
}

/*  ----------------------------------------------------
 *  Function:       isr_stack_kernel_print
 *
 *  Prints the contents of an isr_stack_kernel_t struct.
 *  --------------------------------------------------*/
void isr_stack_kernel_print(isr_stack_kernel_t *stack){
    printf("GS = 0x%X,", stack->gs);
    printf("FS = 0x%X, ", stack->fs);
    printf("ES = 0x%X, ", stack->es);
    printf("DS = 0x%X\n", stack->ds);

    printf("EDI = 0x%X, ", stack->edi);
    printf("ESI = 0x%X, ", stack->esi);
    printf("EBP = 0x%X, ", stack->ebp);
    printf("ESP = 0x%X, ", stack->esp);
    printf("EBX = 0x%X, ", stack->ebx);
    printf("EDX = 0x%X, ", stack->edx);
    printf("ECX = 0x%X, ", stack->ecx);
    printf("EAX = 0x%X\n", stack->eax);

    printf("Int No = 0x%X, ", stack->int_no);
    printf("Err Code = 0x%X\n", stack->err_code);

    printf("EIP = 0x%X, ", stack->eip);
    printf("CS = 0x%X, ", stack->cs);
    printf("EFLAGS = 0x%X\n", stack->eflags);
}
