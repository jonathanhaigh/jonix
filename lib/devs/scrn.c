/* ---------------------------------------------------
* Jonix
*
* scrn.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "devs/scrn.h"
#include "c/string.h"
#include "io.h"

#define VIDEO_MEMORY_BASE 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

volatile uint8_t attribute = 0x1F;
volatile uint8_t tab_width = 8;
volatile uint8_t xpos      = 0;
volatile uint8_t ypos      = 0;

/*  ----------------------------------------------------
 *  Function:       scrn_clear
 *  --------------------------------------------------*/
void scrn_clear(){
    uint16_t *vidmem_ptr = (uint16_t *) VIDEO_MEMORY_BASE;

    uint16_t i;

    for( i=0 ; i<SCREEN_WIDTH * SCREEN_HEIGHT ; i++, vidmem_ptr++){
        *vidmem_ptr = ' ' | (attribute << 8);
    }
}

/*  ----------------------------------------------------
 *  Function:       scrn_move_hw_cursor
 *  --------------------------------------------------*/
void scrn_move_hw_cursor(){

    unsigned int offset = ypos * 80 + xpos;

    outb(0x3D4, 14);
    outb(0x3D5, offset >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, offset);
}


/*  ----------------------------------------------------
 *  Function:       scrn_scroll
 *  --------------------------------------------------*/
void scrn_scroll(){

    uint16_t *vidmem_ptr = (uint16_t *) VIDEO_MEMORY_BASE;

    memmove(
        vidmem_ptr, 
        vidmem_ptr+80, 
        SCREEN_WIDTH * (SCREEN_HEIGHT-1) * 2
    );

    memset_w(
        vidmem_ptr + SCREEN_WIDTH * (SCREEN_HEIGHT-1),
        ' ' | (attribute << 8),
        SCREEN_WIDTH * 2
    ); 

    ypos--;
   
    scrn_move_hw_cursor(); 
}


/*  ----------------------------------------------------
 *  Function:       scrn_putc
 *  --------------------------------------------------*/
void scrn_putc(char c){

    unsigned char cu    = (unsigned char) c;

    /*
     * Horizontal Tab
     */
    if(cu == '\t'){
        xpos = (xpos + tab_width) & ~(tab_width - 1);
    }

    /*
     * Carriage Return
     */
    else if(cu == '\r'){
        xpos    = 0;
    }

    /*
     * Line Feed
     */
    else if(cu == '\n'){
        xpos    = 0;
        ypos    += 1;
    }

    /*
     * Backspace
     */
    else if(cu == 0x08){
        if(xpos > 0){
            xpos--;
        }
    }

    /*
     * Printable character
     */
    else if(cu >= ' '){

        uint16_t *vidmem_ptr  = (uint16_t *) VIDEO_MEMORY_BASE;
        vidmem_ptr  += xpos + SCREEN_WIDTH*ypos;

        *vidmem_ptr = cu | (attribute << 8);

        xpos    += 1;
    }

    if(xpos >= SCREEN_WIDTH){
        xpos    = 0;
        ypos    += 1;
    }

    if(ypos >= SCREEN_HEIGHT){
        scrn_scroll();
    }

    scrn_move_hw_cursor();
}

/*  ----------------------------------------------------
 *  Function:       scrn_puts
 *
 *  Prints a string on the screen.
 *  --------------------------------------------------*/
void scrn_puts(char *str){

    unsigned char *stru  = (unsigned char *) str;
    while(*stru != 0){
        scrn_putc(*stru);
        stru++;
    }
}

/*  ----------------------------------------------------
 *  Function:       scrn_puta
 *
 *  Prints an array of strings on the screen.
 *  --------------------------------------------------*/
void scrn_puta(char **str, char *delim, int len){

    int i;
    for( i=0; i<len-1; i++){
        scrn_puts(str[i]);
        scrn_puts(delim);
    }
    scrn_puts(str[i]);
}

/*  ----------------------------------------------------
 *  Function:       scrn_putn
 *
 *  Prints a number to the screen
 *  --------------------------------------------------*/
void scrn_putn(int num){
    printf("%d", num);
}

/*  ----------------------------------------------------
 *  Function:       scrn_putn_16
 *
 *  Prints a number to the screen in hex.
 *  --------------------------------------------------*/
void scrn_putn_16(int num){
    printf("0x%X", num);
}

/*  ----------------------------------------------------
 *  Function:       scrn_putn_h
 *
 *  Prints a number to the screen in human readable form.
 *  --------------------------------------------------*/
void scrn_putn_h(int num){
    printf("%H", num);
}

/*  ----------------------------------------------------
 *  Function:       scrn_put_var
 *
 *  Prints a variable name and its value on the screen.
 *  --------------------------------------------------*/
void scrn_put_var(char *name, int num){
    printf("%s\t= %d\n", name, num);
}


/*  ----------------------------------------------------
 *  Function:       scrn_put_var_16
 *
 *  Prints a variable name and its value in base 16 on
 *  the screen.
 *  --------------------------------------------------*/
void scrn_put_var_16(char *name, int num){
    printf("%s\t= 0x%X\n", name, num);
}

/*  ----------------------------------------------------
 *  Function:       scrn_put_var_h
 *
 *  Prints a variable name and its value on the screen
 *  in human readable form.
 *  --------------------------------------------------*/
void scrn_put_var_h(char *name, int num){
    printf("%s\t= %H\n", name, num);
}

/*  ----------------------------------------------------
 *  Function:       scrn_put_var_str
 *
 *  Prints a string variable name and its string 
 *  representation.
 *  --------------------------------------------------*/
void scrn_put_var_str(char *name, char *str){
    printf("%s\t= %s\n", name, str);
}
