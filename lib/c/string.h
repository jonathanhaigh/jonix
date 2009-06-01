/* ---------------------------------------------------
* Jonix - C Standard Library
*
* string.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _STRING_H
#define _STRING_H

#include "c/stddef.h"

extern void *   memcpy(void *dest, const void *src, size_t count);
extern void *   memmove(void *dest, const void *src, size_t count);
extern void *   memset(void *dest, int val, size_t count);
extern void *   memset_w(void *dest, int val, size_t count);

extern size_t   strlen(const char *str);
extern char *   strcpy(char *dest, const char *src);
extern char *   strcpy_n(char *dest, const char *src, size_t max_len);
extern char *   strcat(char *dest, const char *src);
extern int strcmp(const char *s1, const char *s2);

#endif
