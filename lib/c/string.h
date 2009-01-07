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

#include "stddef.h"

extern void *   memcpy(void *dest, const void *src, size_t count);
extern void *   memmove(void *dest, const void *src, size_t count);
extern void *   memset(void *dest, int val, size_t count);
extern void *   memset_w(void *dest, int val, size_t count);

extern size_t   strlen(const char *cs);
extern char *   strcpy(char *s1, const char *s2);

#endif
