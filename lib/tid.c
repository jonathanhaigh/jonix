/* ---------------------------------------------------
* Jonix
*
* tid.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

/*
 * As in Linux, to get thread descriptor pointers from a
 * TID or PID, we have a hash table mapping TIDs to descriptors.
 * This hash table is also used to allocate TIDs.
 */

#include "tid.h"
#include "sem.h"
#include "thr.h"
#include "mem.h"

list_head_t * tid_hash_tbl[TID_HASH_SIZE];  // The hash table.
sem_t tid_sem;                              // Semaphore for hash table
uint16_t tid_last;                          // Last TID allocated.


/*  ----------------------------------------------------
 *  Function:       tid_init
 *  --------------------------------------------------*/
void tid_init(){

    // First TID allocated will be 0.
    //
    tid_last    = -1;

    // Initialise hash table to NULLs
    //
    int i;
    for(i = 0; i < TID_HASH_SIZE; i++){
        tid_hash_tbl[i] = NULL;
    }

    // Initialise hash table semaphore
    //
    tid_sem = sem_new(1);
}

/*  ----------------------------------------------------
 *  Function:       tid_alloc
 *  --------------------------------------------------*/
bool tid_alloc(thread_t *thread){

    // We just loop through possible TIDs to see if they
    // are in use, starting at the last allocated TID + 1.
    //
    uint16_t test_tid    = tid_last;
    uint16_t test_tid_hash;
    bool test_tid_ok;   // Used later
    thread_t *thread2;

    sem_wait(&tid_sem);

    while(++test_tid != tid_last){

        if(test_tid >= TID_MAX){

            if(tid_last == 1) return 0;

            test_tid    = 1;
        }

        test_tid_hash    = tid_hash(test_tid);

        if(tid_hash_tbl[test_tid_hash] == NULL){
            // We have found a suitable TID, but we must create a bucket
            // (a list) for this hash value in the table.
            //
            list_head_t *list = (list_head_t *) kmalloc(sizeof(list_head_t));
            *list = list_new();
            tid_hash_tbl[test_tid_hash] = list;

            // Now add the thread to the list and update the thread
            // descriptor with the new tid.
            //
            list_add(list, thread);
            thread->tid = test_tid;

            tid_last    = test_tid;
            sem_release(&tid_sem);
            return 1;
        }
        
        // Now we know that there is an existing bucket for this
        // table index. We loop through the threads in the bucket to
        // see if test_tid is taken.
        //
        test_tid_ok = 1;
        foreach(thread2, tid_hash_tbl[test_tid_hash]){
            if(thread2->tid == test_tid){
                test_tid_ok = 0;
                break;
            }
        }
        if(test_tid_ok){
            // We have found a suitable TID.
            // Add the thread to the list and update the thread
            // descriptor with the new tid.
            //
            list_add(tid_hash_tbl[test_tid_hash], thread);
            thread->tid = test_tid;

            tid_last    = test_tid;
            sem_release(&tid_sem);
            return 1;
        }
    }

    return 0;
}
