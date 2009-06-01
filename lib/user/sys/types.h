/* ---------------------------------------------------
* Jonix
*
* types.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _TYPES_H
#define _TYPES_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef unsigned int ssize_t;
#endif

#ifndef _MODE_T
#define _MODE_T
typedef int mode_t;
#endif

#ifndef _OFF_T
#define _OFF_T
typedef unsigned int off_t;
#endif

#ifndef _ID_T
#define _ID_T
typedef unsigned int id_t;
#endif

#ifndef _PID_T
#define _PID_T
typedef id_t pid_t;
#endif

#ifndef _DEV_T
#define _DEV_T
typedef id_t dev_t;
#endif

#endif
