/* ---------------------------------------------------
* Jonix
*
* sem.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "sem.h"
#include "thr.h"

/*  ----------------------------------------------------
 *  Function:       sem_new
 *  --------------------------------------------------*/
sem_t sem_new(init_cnt){
    sem_t sem;
    sem.count       = init_cnt;
    sem.wait_list   = list_new();
    return sem;
}

/*  ----------------------------------------------------
 *  Function:       sem_wait
 *  --------------------------------------------------*/
void sem_wait(sem_t *sem){
    __asm__ ("cli");
    if(sem->count > 0){
        sem->count--;
        __asm__("sti");
    }
    else{
        // Add the current thread to the wait list
        //
        list_add(&sem->wait_list, thr_current);
        __asm__("sti");
        resched();
    }
}

/*  ----------------------------------------------------
 *  Function:       sem_release
 *  --------------------------------------------------*/
void sem_release(sem_t *sem){
    __asm__("cli");
    sem->count++;
    __asm__("sti");
}
