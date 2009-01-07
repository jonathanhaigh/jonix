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
extern void scrn_putn(long num);
extern void scrn_putn_16(long num);
extern void scrn_putn_h(long num);
extern void scrn_put_var(char *name, long num);
extern void scrn_put_var_16(char *name, long num);
extern void scrn_put_var_h(char *name, long num);
extern void scrn_put_var_str(char *name, char *str);

#endif
