/* ---------------------------------------------------
* Jonix
*
* system.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "stddef.h"
#include "stdint.h"

#define ERR_STR_SIZE 256

#define PANIC(str)                     \
scrn_puts("\n\n!!!KERNEL PANIC: ");    \
scrn_puts(str);                        \
scrn_put_var_str("File", __FILE__);    \
scrn_put_var("Line", __LINE__);        \
asm("hlt")


#endif
