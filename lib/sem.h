/* ---------------------------------------------------
* Jonix
*
* sem.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _SEM_H
#define _SEM_H

#include "system.h"
#include "list.h"

/*  ----------------------------------------------------
 *  typedef struct:         sem_t
 *  --------------------------------------------------*/
typedef struct {
    signed int count;
    list_head_t wait_list;
} sem_t;


/*
 * Functions
 */
extern sem_t sem_new();
extern void sem_wait(sem_t *sem);
extern void sem_release(sem_t *sem);

#endif
