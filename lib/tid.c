/* ---------------------------------------------------
* Jonix
*
* tid.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "tid.h"
#include "sem.h"
#include "thr.h"
#include "mem.h"

uint16_t tid_last;  // Last TID allocated.


/*  ----------------------------------------------------
 *  Function:       tid_init
 *  --------------------------------------------------*/
void tid_init(){

    // First TID allocated will be 0.
    //
    tid_last    = -1;
}


/*  ----------------------------------------------------
 *  Function:       tid_alloc
 *  --------------------------------------------------*/
bool tid_alloc(thread_t *thread){

    // We just loop through possible TIDs to see if they
    // are in use, starting at the last allocated TID + 1.
    //
    uint16_t tid;
    
    for(tid=tid_last+1; tid != tid_last; tid++){

        if(tid > TID_MAX){
            tid = -1;
            continue;
        }

        if(thr_descriptors[tid] == 0){
            thread->tid = tid;
            return 1;
        }
    }

    return 0;
}

