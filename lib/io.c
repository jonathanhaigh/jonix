/* ---------------------------------------------------
* Jonix
*
* io.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "io.h"

/*  ----------------------------------------------------
 *  Function:       outb
 *  --------------------------------------------------*/
void outb(uint16_t port, unsigned char data){
    asm volatile(
        "outb %1, %0"
        :
        : "dN" (port), "a" (data)
    );
}


/*  ----------------------------------------------------
 *  Function:       inb
 *  --------------------------------------------------*/
unsigned char inb(uint16_t port){

    unsigned char retval;

    asm volatile(
        "inb %1, %0"
        : "=a" (retval)
        : "dN" (port)
    );

    return retval;
}

/*  ----------------------------------------------------
 *  Function:       outw
 *
 *  Sends a word to a port.
 *  --------------------------------------------------*/
void outw(uint16_t port, uint16_t data){
    asm volatile(
        "outw %1, %0"
        :
        : "dN" (port), "a" (data)
    );
}


/*  ----------------------------------------------------
 *  Function:       inw
 *
 *  Gets a word from a port.
 *  --------------------------------------------------*/
uint16_t inw(uint16_t port){

    uint16_t retval;

    asm volatile(
        "inw %1, %0"
        : "=a" (retval)
        : "dN" (port)
    );

    return retval;
}

/*  ----------------------------------------------------
 *  Function:       outl
 *
 *  Sends two words to a port.
 *  --------------------------------------------------*/
void outl(uint16_t port, uint32_t data){
    asm volatile(
        "outl %1, %0"
        :
        : "dN" (port), "a" (data)
    );
}

/*  ----------------------------------------------------
 *  Function:       inl
 *
 *  Gets two words from a port.
 *  --------------------------------------------------*/
uint32_t inl(uint16_t port){

    uint32_t retval;

    asm volatile(
        "inl %1, %0"
        : "=a" (retval)
        : "dN" (port)
    );

    return retval;
}
