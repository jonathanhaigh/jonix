/* ---------------------------------------------------
* Jonix - C Standard Library
*
* stdarg.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
*
* NOTE: This does not conform to ANSI C standard
* --------------------------------------------------*/
#ifndef STDARG_H
#define STDARG_H

#ifndef _VA_LIST
#define _VA_LIST
typedef void * va_list;
#endif

#define va_start(ap, p)     (ap = (char *) (&(p)+1))
#define va_arg(ap, type)    ((type *) (ap += sizeof(type)))[-1]
#define va_end(ap)

#endif
