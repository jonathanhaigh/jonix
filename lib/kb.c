/* ---------------------------------------------------
* Jonix
*
* kb.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "kb.h"
#include "scrn.h"
#include "irq.h"
#include "io.h"
#include "string.h"

bool            kb_print_to_screen;

char            kb_buffer[KB_BUFFER_SIZE+1];
unsigned char   buffer_pos;

char            kb_status;
char            kb_err[ERR_STR_SIZE];

char            kb_flags;

char kb_layout[256];
char kb_layout_sh[256];


/*  ----------------------------------------------------
 *  Function:       kb_read
 *  --------------------------------------------------*/
unsigned char kb_read(){
    unsigned char c  = inb(0x60);

    /* Acknowledge receipt of scancode by disabling and reenabling keyboard.
     * This is done through bit 7 of port 0x61*/
    unsigned char p61    = inb(0x61);
    outb(0x61, p61 | 0x80);
    outb(0x61, p61);

    if(c == 0xE0){
        /* 'extended' key has been pressed. Get the extended code from next int. */
        return 128 + kb_read();
    }
    else{
        /* regular key has been pressed */
        return c;
    }
}


/*  ----------------------------------------------------
 *  Function:       kb_handler
 *  --------------------------------------------------*/
void kb_handler(isr_stack_t *r){

    unsigned char c  = kb_read();

    if(c & 0x80){
        /* key release */

        c &= 0x7F; /* get rid of the key release bit */

        /* Shift */ 
        if(c == 42 || c == 54){
            kb_flags    &= ~KBF_SHIFT;
        }
        /* CTL */
        else if(c == 29 || c == 128+29){
            kb_flags    &= ~KBF_CTL;
        }
        /* ALT */
        else if(c == 56 || c == 128+56){
            kb_flags    &= ~KBF_ALT;
/* TODO: separate Alt and AltGr 
 * */
        }
        /* CAPSLOCK */
        else if(c == 58){
            kb_flags    &= ~KBF_CAPS;
        }
        /* TODO: SUP (Logo Key) */
        
    }
    /* key press */

    /* Shift */ 
    else if(c == 42 || c == 54){
        kb_flags    |= KBF_SHIFT;
    }
    /* CTL */
    else if(c == 29 || c == 128+29){
        kb_flags    |= KBF_CTL;
    }
    /* ALT */
    else if(c == 56 || c == 128+56){
        kb_flags    |= KBF_ALT;
    }
    /* CAPSLOCK */
    else if(c == 58){
        kb_flags    |= KBF_CAPS;
    }
    /* TODO: SUP (Logo Key) */
    else{

        if(kb_flags & KBF_SHIFT){
            c   = kb_layout_sh[c];
        }
        else{
            c   = kb_layout[c];
        }

        unsigned char i;

        if(kb_print_to_screen){
            for( i=0 ; i<buffer_pos ; i++){
                scrn_putc(kb_buffer[i]);
                kb_buffer[i]    = 0;
            }
            buffer_pos  = 0;
            scrn_putc((char) c);
        }
        else{
           if(buffer_pos >= KB_BUFFER_SIZE){
               strcpy(kb_err, "Keyboard buffer full.");
           }
           else{
               kb_buffer[buffer_pos]    = (char) c;
               buffer_pos++;
           }
        }

    }

}


/*  ----------------------------------------------------
 *  Function:       kb_init
 *  --------------------------------------------------*/
void kb_init(){

    kb_print_to_screen = 0;

    *kb_err     = NULL;

    kb_flags    = 0;

    int i;
    for( i=0 ; i<KB_BUFFER_SIZE+1 ; i++){
        kb_buffer[i]  = 0;
    }

    buffer_pos      = 0;

    irq_set_handler(1, kb_handler);

    /* Map scan codes to ascii chars 
    */
    kb_layout[0] = 0; kb_layout[1] = 27; /* ESC */ 
    kb_layout[2] = '1'; kb_layout[3] = '2'; kb_layout[4] = '3';
    kb_layout[5] = '4'; kb_layout[6] = '5'; kb_layout[7] = '6';
    kb_layout[8] = '7'; kb_layout[9] = '8'; kb_layout[10] = '9';
    kb_layout[11] = '0'; kb_layout[12] = '-'; kb_layout[13] = '=';
    kb_layout[14] = '\b'; kb_layout[15] = '\t'; kb_layout[16] = 'q';
    kb_layout[17] = 'w'; kb_layout[18] = 'e'; kb_layout[19] = 'r';
    kb_layout[20] = 't'; kb_layout[21] = 'y'; kb_layout[22] = 'u';
    kb_layout[23] = 'i'; kb_layout[24] = 'o'; kb_layout[25] = 'p';
    kb_layout[26] = '['; kb_layout[27] = ']'; kb_layout[28] = '\n';
    kb_layout[29] = 0; /* Left Control */
    kb_layout[30] = 'a'; kb_layout[31] = 's'; kb_layout[32] = 'd';
    kb_layout[33] = 'f'; kb_layout[34] = 'g'; kb_layout[35] = 'h';
    kb_layout[36] = 'j'; kb_layout[37] = 'k'; kb_layout[38] = 'l';
    kb_layout[39] = ';'; kb_layout[40] = '\''; kb_layout[41] = '`';
    kb_layout[42] = 0; /* Left Shift */
    kb_layout[43] = '#'; kb_layout[44] = 'z'; kb_layout[45] = 'x';
    kb_layout[46] = 'c'; kb_layout[47] = 'v'; kb_layout[48] = 'b';
    kb_layout[49] = 'n'; kb_layout[50] = 'm'; kb_layout[51] = ',';
    kb_layout[52] = '.'; kb_layout[53] = '/';
    kb_layout[54] = 0; /* Right Shift */
    kb_layout[55] = '*'; kb_layout[57]  = ' ';

    /*
     * Extended keys. The indeces are actually 128 + extended code 
     */
    kb_layout[156]    = '\n'; kb_layout[181]    = '/';

    /* 
     * Map for when shift is down 
     */
    kb_layout_sh[0] = 0; kb_layout_sh[1] = 27; /* ESC */ 
    kb_layout_sh[2] = '!'; kb_layout_sh[3] = '"'; kb_layout_sh[4] = '$';
    kb_layout_sh[5] = '$'; kb_layout_sh[6] = '%'; kb_layout_sh[7] = '^';
    kb_layout_sh[8] = '&'; kb_layout_sh[9] = '*'; kb_layout_sh[10] = '(';
    kb_layout_sh[11] = ')'; kb_layout_sh[12] = '_'; kb_layout_sh[13] = '+';
    kb_layout_sh[14] = '\b'; kb_layout_sh[15] = '\t'; kb_layout_sh[16] = 'Q';
    kb_layout_sh[17] = 'W'; kb_layout_sh[18] = 'E'; kb_layout_sh[19] = 'R';
    kb_layout_sh[20] = 'T'; kb_layout_sh[21] = 'Y'; kb_layout_sh[22] = 'U';
    kb_layout_sh[23] = 'I'; kb_layout_sh[24] = 'O'; kb_layout_sh[25] = 'P';
    kb_layout_sh[26] = '{'; kb_layout_sh[27] = '}'; kb_layout_sh[28] = '\n';
    kb_layout_sh[29] = 0; /* Left Control */
    kb_layout_sh[30] = 'A'; kb_layout_sh[31] = 'S'; kb_layout_sh[32] = 'D';
    kb_layout_sh[33] = 'F'; kb_layout_sh[34] = 'G'; kb_layout_sh[35] = 'H';
    kb_layout_sh[36] = 'J'; kb_layout_sh[37] = 'K'; kb_layout_sh[38] = 'L';
    kb_layout_sh[39] = ':'; kb_layout_sh[40] = '@'; kb_layout_sh[41] = ' ';
    kb_layout_sh[42] = 0; /* Left Shift */
    kb_layout_sh[43] = '~'; kb_layout_sh[44] = 'Z'; kb_layout_sh[45] = 'X';
    kb_layout_sh[46] = 'C'; kb_layout_sh[47] = 'V'; kb_layout_sh[48] = 'B';
    kb_layout_sh[49] = 'N'; kb_layout_sh[50] = 'M'; kb_layout_sh[51] = '<';
    kb_layout_sh[52] = '>'; kb_layout_sh[53] = '?';
    kb_layout_sh[54] = 0; /* Right Shift */
    kb_layout_sh[55] = '*'; kb_layout_sh[57] = ' ';
}


/*  ----------------------------------------------------
 *  Function:       kb_reset
 *  --------------------------------------------------*/
void kb_reset(){

    *kb_err     = NULL;

    kb_flags    = 0;

    int i;
    for( i=0 ; i<KB_BUFFER_SIZE+1 ; i++){
        kb_buffer[i]  = 0;
    }

    buffer_pos      = 0;
}

