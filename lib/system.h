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

#include "c/stddef.h"
#include "c/stdint.h"
#include "c/stdio.h"
#include "c/stdbool.h"
#include "c/string.h"
#include "user/sys/types.h"
#include "debug.h"

#define ERR_STR_SIZE 256

// Adds string to some module's error string
//TODO: Change this - it doesn't check for buffer overflow!
//
#define ADD_ERR(mod, str) (strcat(mod##_err, (str)), strcat(mod##_err, "\n"))

#define PANIC(str)  \
printf("\n\n!!!KERNEL PANIC: %s\nFile: %s\nLine: %d\n, (str)", __FILE__, __LINE__); \
asm("hlt")

#define BIT(bit) (1 << (bit))
#define BYTE(byte) (0xFF << (byte))

// Turns a token into a string
//
#define TOKEN_TO_STR(tok) #tok

// Turns a token into a string (but with macro expansion)
//
#define TO_STR(tok) TOKEN_TO_STR(tok)
 
// Create's a unique variable name
//
#define CONCAT2(a,b) a##b
#define CONCAT(a,b) CONCAT2(a,b)
#define UNIQUE_VAR() CONCAT(xxx_unique_var, __COUNTER__)

#define set_errno(errno)

#endif
