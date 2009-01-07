/* ---------------------------------------------------
* Jonix
*
* tid.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _TID_H
#define _TID_H

#include "system.h"

#define TID_MAX 65535       // Largest possible TID
#define TID_HASH_SIZE 1000  // Size of TID hash table

// The hash function for the TID hash table. Taken from Linux.
//
#define tid_hash(tid) ((((tid) >> 8) ^ (tid)) & (TID_HASH_SIZE - 1))


extern void tid_init();
extern bool tid_alloc();

#endif
