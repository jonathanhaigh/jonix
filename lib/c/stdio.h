/* ---------------------------------------------------
* Jonix - C Standard Library
*
* stdio.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _STDIO_H
#define _STDIO_H

#include "c/stdarg.h"
#include "c/stddef.h"

#define PRINTF_BUFFER_SIZE 100

extern int vsnprintf(char *buffer, size_t size, const char *format, va_list ap);
extern int snprintf(char *buffer, size_t size, const char *format, ...);
extern int vprintf(const char *format, va_list ap);
extern int printf(const char *format, ...);

#endif
