/* ---------------------------------------------------
* Jonix - C Standard Library
*
* stdbool.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
*
* Complete implementation of
* The Open Group Base Specifications Issue 6 stdbool.h
* --------------------------------------------------*/

#ifndef _STDBOOL_H
#define _STDBOOL_H

#ifndef bool
#define bool _Bool
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifdef __bool_true_false_are_defined
#undef __bool_true_false_are_defined
#define __bool_true_false_are_defined 1
#endif


#endif
