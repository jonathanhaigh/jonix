/* ---------------------------------------------------
* Jonix - Utilities
*
* util.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _UTIL_H
#define _UTIL_H

#include "stddef.h"
#include "stdint.h"

int _i;

#define MAP(dest_type, dest, src, count, expression) (  \
    dest_type dest[(count)];                            \
    for(_i=0; _i<(count); _i){                          \
        dest[_i]   = (expression);                      \
    }                                                   \
)


/*
 * Symbols from util/string.c
 */
extern void char2str_10(char *dest, char num);
extern void int2str_10(char *dest, int num);
extern void long2str_10(char *dest, long num);
extern void long2str_10h(char *dest, unsigned long num, unsigned char precision);

extern void char2str_16(char *dest, char num);
extern void int2str_16(char *dest, int num);
extern void long2str_16(char *dest, long num);
extern char uc(const char c);

#endif
