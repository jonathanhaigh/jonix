/* ---------------------------------------------------
* Jonix
*
* scrn.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _SCRN_H
#define _SCRN_H

#include "system.h"

/*
 * Functions
 */
extern void scrn_clear();
extern void scrn_putc(char c);
extern void scrn_puts(char *str);
extern void scrn_putn(int num);
extern void scrn_putn_16(int num);
extern void scrn_putn_h(int num);
extern void scrn_put_var(char *name, int num);
extern void scrn_put_var_16(char *name, int num);
extern void scrn_put_var_h(char *name, int num);
extern void scrn_put_var_str(char *name, char *str);

#endif
