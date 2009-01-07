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
#include "scrn.h"

/* The isr_n routines are written in assembly, but they call the c function fault_handler
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
    idt_set_entry(0, (unsigned int) isr_0, 0x08, 0x8E);
    idt_set_entry(1, (unsigned int) isr_1, 0x08, 0x8E);
    idt_set_entry(2, (unsigned int) isr_2, 0x08, 0x8E);
    idt_set_entry(3, (unsigned int) isr_3, 0x08, 0x8E);
    idt_set_entry(4, (unsigned int) isr_4, 0x08, 0x8E);
    idt_set_entry(5, (unsigned int) isr_5, 0x08, 0x8E);
    idt_set_entry(6, (unsigned int) isr_6, 0x08, 0x8E);
    idt_set_entry(7, (unsigned int) isr_7, 0x08, 0x8E);
    idt_set_entry(8, (unsigned int) isr_8, 0x08, 0x8E);
    idt_set_entry(9, (unsigned int) isr_9, 0x08, 0x8E);
    idt_set_entry(10, (unsigned int) isr_10, 0x08, 0x8E);
    idt_set_entry(11, (unsigned int) isr_11, 0x08, 0x8E);
    idt_set_entry(12, (unsigned int) isr_12, 0x08, 0x8E);
    idt_set_entry(13, (unsigned int) isr_13, 0x08, 0x8E);
    idt_set_entry(14, (unsigned int) isr_14, 0x08, 0x8E);
    idt_set_entry(15, (unsigned int) isr_15, 0x08, 0x8E);
    idt_set_entry(16, (unsigned int) isr_16, 0x08, 0x8E);
    idt_set_entry(17, (unsigned int) isr_17, 0x08, 0x8E);
    idt_set_entry(18, (unsigned int) isr_18, 0x08, 0x8E);
    idt_set_entry(19, (unsigned int) isr_19, 0x08, 0x8E);
    idt_set_entry(20, (unsigned int) isr_20, 0x08, 0x8E);
    idt_set_entry(21, (unsigned int) isr_21, 0x08, 0x8E);
    idt_set_entry(22, (unsigned int) isr_22, 0x08, 0x8E);
    idt_set_entry(23, (unsigned int) isr_23, 0x08, 0x8E);
    idt_set_entry(24, (unsigned int) isr_24, 0x08, 0x8E);
    idt_set_entry(25, (unsigned int) isr_25, 0x08, 0x8E);
    idt_set_entry(26, (unsigned int) isr_26, 0x08, 0x8E);
    idt_set_entry(27, (unsigned int) isr_27, 0x08, 0x8E);
    idt_set_entry(28, (unsigned int) isr_28, 0x08, 0x8E);
    idt_set_entry(29, (unsigned int) isr_29, 0x08, 0x8E);
    idt_set_entry(30, (unsigned int) isr_30, 0x08, 0x8E);
    idt_set_entry(31, (unsigned int) isr_31, 0x08, 0x8E);
}

/*  ----------------------------------------------------
 *  Function:       fault_handler
 *  --------------------------------------------------*/
void fault_handler(isr_stack_t *r){

    if (r->int_no < 32){
        scrn_puts("\n\n!!! KERNEL PANIC: ");
        scrn_puts(exception_messages[r->int_no]);
        scrn_puts("\n");
        asm("hlt");
    }
}

