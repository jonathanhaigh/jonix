/* ---------------------------------------------------
* Jonix
*
* irq.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "irq.h"
#include "io.h"
#include "idt.h"

extern void irq_0(); extern void irq_1(); extern void irq_2(); extern void irq_3();
extern void irq_4(); extern void irq_5(); extern void irq_6(); extern void irq_7();
extern void irq_8(); extern void irq_9(); extern void irq_10(); extern void irq_11();
extern void irq_12(); extern void irq_13(); extern void irq_14(); extern void irq_15();

/* This is an array of function pointers */
void *irq_handlers[]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*  ----------------------------------------------------
 *  Function:       irq_set_handler
 *  --------------------------------------------------*/
void irq_set_handler(int irq, void (*handler)(isr_stack_t *r) ){
    irq_handlers[irq]   = handler;
}

/*  ----------------------------------------------------
 *  Function:       irq_unset_handler
 *  --------------------------------------------------*/
void irq_unset_handler(int irq){
    irq_handlers[irq]   = 0;
}

/*  ----------------------------------------------------
 *  Function:       irq_remap
 *
 *  Tells the programmable interrupt controller to map
 *  IRQs 0-15 to entries 32-47 in the IDT.
 *  --------------------------------------------------*/
void irq_remap(){
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}


/*  ----------------------------------------------------
 *  Function:       irq_init
 *  --------------------------------------------------*/
void irq_init(){

    irq_remap();

    idt_set_entry(32, (uint32_t) irq_0, 0x08, 0x8E);
    idt_set_entry(33, (uint32_t) irq_1, 0x08, 0x8E);
    idt_set_entry(34, (uint32_t) irq_2, 0x08, 0x8E);
    idt_set_entry(35, (uint32_t) irq_3, 0x08, 0x8E);
    idt_set_entry(36, (uint32_t) irq_4, 0x08, 0x8E);
    idt_set_entry(37, (uint32_t) irq_5, 0x08, 0x8E);
    idt_set_entry(38, (uint32_t) irq_6, 0x08, 0x8E);
    idt_set_entry(39, (uint32_t) irq_7, 0x08, 0x8E);
    idt_set_entry(41, (uint32_t) irq_8, 0x08, 0x8E);
    idt_set_entry(42, (uint32_t) irq_9, 0x08, 0x8E);
    idt_set_entry(43, (uint32_t) irq_10, 0x08, 0x8E);
    idt_set_entry(44, (uint32_t) irq_11, 0x08, 0x8E);
    idt_set_entry(45, (uint32_t) irq_12, 0x08, 0x8E);
    idt_set_entry(46, (uint32_t) irq_13, 0x08, 0x8E);
    idt_set_entry(47, (uint32_t) irq_14, 0x08, 0x8E);
    idt_set_entry(48, (uint32_t) irq_15, 0x08, 0x8E);
}


/*  ----------------------------------------------------
 *  Function:       irq_handler
 *  --------------------------------------------------*/
void irq_handler(isr_stack_t *r){

    void (*handler)(isr_stack_t *r);

    handler = irq_handlers[r->int_no];

    if(handler != NULL){
        handler(r);
    }

     /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if(r->int_no >= 40){
        outb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outb(0x20, 0x20);
}
 
