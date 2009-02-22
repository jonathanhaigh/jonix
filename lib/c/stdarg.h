/* ---------------------------------------------------
* Jonix - C Standard Library
*
* stdarg.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/
#ifndef STDARG_H
#define STDARG_H

typedef void * va_list;

#define va_start(ap, p)     (ap = (char *) (&(p)+1))
#define va_arg(ap, type)    ((type *) (ap += sizeof(type)))[-1]
#define va_end(ap)

#endif
