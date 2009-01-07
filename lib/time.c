/* ---------------------------------------------------
* Jonix
*
* time.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "time.h"
#include "sched.h"
#include "io.h"
#include "irq.h"

#define TIME_PIT_0    0x40  /* PIT Channel 0 Data Port */
#define TIME_PIT_1    0x41  /* PIT Channel 1 Data Port */
#define TIME_PIT_2    0x42  /* PIT Channel 2 Data Port */
#define TIME_PIT_COMM 0x43  /* PIT Command Port*/

#define TIME_IRQ_FREQ 100   /* Default frequency of IRQ0s */

long time_ticks = 0; /* Incremented 1000 times per second */

/*  ----------------------------------------------------
 *  Function:       time_init
 *  --------------------------------------------------*/
void time_init(){

    /* 
     * The Programmable Interval Timer (PIT) (chanel 1)
     * will generate and IRQ0 at regular intervals. It
     * has a base frequency of 1.19MHz, but we can change
     * the frequency of the interrupts by specifying a
     * number to devide this by.
     * 
     * First we need to tell the command port what modes
     * we want to use.
     *
     * COMMAND PORT BITS: (from http://www.osdev.org/osfaq2/index.php/PIT).
     *
     *  Bit/s       Usage
        6 and 7     Select channel:
                        0 0 = Channel 0
                        0 1 = Channel 1
                        1 0 = Channel 2
                        1 1 = Read-back command (8254 only)
        4 and 5     Access mode:
                        0 0 = Latch count value command
                        0 1 = Access mode: lobyte only
                        1 0 = Access mode: hibyte only
                        1 1 = Access mode: lobyte/hibyte
        1 to 3      Operating mode:
                        0 0 0 = Mode 0 (interrupt on terminal count)
                        0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                        0 1 0 = Mode 2 (rate generator)
                        0 1 1 = Mode 3 (square wave generator)
                        1 0 0 = Mode 4 (software triggered strobe)
                        1 0 1 = Mode 5 (hardware triggered strobe)
                        1 1 0 = Mode 2 (rate generator, same as 010b)
                        1 1 1 = Mode 3 (square wave generator, same as 011b)
        0           BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
     * 
     * We will use chanel 1, lobyte/hibyte access mode, operating mode 3 and BCD mode 0.
     * i.e. 0b00110110 = 0x36.
     */
    outb(TIME_PIT_COMM, 0x36);

    /*
     * We now send the divisor to use (low byte then high byte) to
     * channel 1's data port.
     */
    uint16_t divisor    = 1193180 / 1000;
    outb(TIME_PIT_0, divisor & 0xFF); 
    outb(TIME_PIT_0, divisor >> 8); 

    irq_set_handler(0, &time_irq_handler);
}

/*  ----------------------------------------------------
 *  Function:       time_irq_handler
 *  --------------------------------------------------*/
void time_irq_handler(isr_stack_t *r){

    time_ticks++;

    resched(*r);
}


/*  ----------------------------------------------------
 *  Function:       sleep
 *  --------------------------------------------------*/
void sleep(int no, char units){
    
    int targ_ticks;

    switch(units){
        case 'm': targ_ticks    = time_ticks + no;      break;   /* miliseconds */
        case 'c': targ_ticks    = time_ticks + 10*no;   break;   /* centiseconds */
        case 'd': targ_ticks    = time_ticks + 100*no;  break;   /* deciseconds */
        case 's': targ_ticks    = time_ticks + 1000*no; break;   /* seconds */
    }
    while(time_ticks < targ_ticks);
}
