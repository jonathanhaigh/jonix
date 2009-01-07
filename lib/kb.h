/* ---------------------------------------------------
* Jonix
*
* kb.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _KB_H
#define _KB_H

#include "system.h"

#define KB_BUFFER_SIZE 255

#define KBF_SHIFT   0x01
#define KBF_CAPS    0x02
#define KBF_CTL     0x04
#define KBF_ALT     0x08
#define KBF_SUP     0x10
#define KRB_ALTGR   0x20


/* 
 * Variables
 */
extern bool kb_print_to_screen;
extern char kb_buffer[KB_BUFFER_SIZE+1];
extern char kb_status;
extern char kb_err[ERR_STR_SIZE];


/*
 * Functions
 */
extern void kb_init();
extern void kb_reset();

#endif
