/* ---------------------------------------------------
* Jonix - C Standard Library
*
* stddef.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
*
* Complete implementation of
* The Open Group Base Specifications Issue 6 stddef.h
* --------------------------------------------------*/

#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef NULL
#define NULL 0
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef int ptrdiff_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned int wchar_t;
#endif

#ifndef offsetof
#define offsetof(type, member) ((size_t)(&((type *)0)->member))
#endif


#endif
