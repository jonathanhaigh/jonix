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
#include "sched.h"
#include "util/list.h"

/*  ----------------------------------------------------
 *  Function:       sem_new
 *  --------------------------------------------------*/
sem_t sem_new(uint32_t init_cnt){
    sem_t sem;
    sem.count       = init_cnt;
    sem.wait_list   = list_new();
    return sem;
}


/*  ----------------------------------------------------
 *  Function:       sem_wait
 *  --------------------------------------------------*/
void sem_wait(sem_t *sem){
    if(sem->count > 0){
        sem->count--;
    }
    else{
        // Add the current thread to the wait list
        //
        list_add(&sem->wait_list, thr_current);
        thr_current->state  = THR_STATE_SEM;
        resched_no_int();
    }
}


/*  ----------------------------------------------------
 *  Function:       sem_release
 *  --------------------------------------------------*/
void sem_release(sem_t *sem){
    if(sem->wait_list.first){
        thread_t *thread    = list_del(&sem->wait_list, 0);
        olist_add(&thr_ready, thread);
        thread->state   = THR_STATE_READY;
    }
    else sem->count++;
}
