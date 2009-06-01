/* ---------------------------------------------------
* Jonix
*
* thr.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _THR_H
#define _THR_H

#include "system.h"
#include "util/list.h"
#include "mem.h"
#include "isr.h"

/*
 * Thread state constants
 */
#define THR_STATE_CURR  0
#define THR_STATE_READY 1
#define THR_STATE_SEM   2
#define THR_STATE_SUSP  3
#define THR_STATE_SLEEP 4

// Max TID
//
#define THR_MAX_TID 1000

// Length of process names (All threads for same proc have same name).
//
#define THR_NAME_LEN 20

// Size of kernel stack for threads
//
#define THR_KSTACK_SIZE 4096


// thread error string.
//
extern char thr_err[ERR_STR_SIZE];


/*  ----------------------------------------------------
 *  typedef struct:         thread_t
 *
 *  Contains information about a thread.
 *  --------------------------------------------------*/
typedef struct{
    uint16_t    tid;    // Thread id - unique id for this thread.

    uint16_t    pid;    // Process id - unique id for this process.

                        // Threads may share PIDs.

    /*
     * Maybe use this to make doing things to a whole process easier/more
     * efficient.
     *
    uint16_t    no_threads; // Number of threads in this thread's process
    */
    
    uint8_t     state;  // ready, sleeping, suspended, etc.

    uint8_t     priority;
    uint8_t     priv;

    uint32_t    sem;        // Pointer to semaphore process is waiting on.

    bool        has_msg;    // Does this proc/thread have msg waiting 
                            // to be read?
                            
    uint32_t    *stack_base; // User mode stack for this thr
    uint32_t    stack_size;

    uint32_t    *kstack_base; // Kernel mode stack for this thr
    uint32_t    kstack_size;
    uint32_t    *kstack_ptr;  // current kernel mode stack pointer

    page_dir_t  page_dir;   // Address of this thread's page dir

    char        name[THR_NAME_LEN+1]; // process name

} thread_t;

/* 
 * Functions from thread.c
 */
extern void thr_init();

extern thread_t * thr_new(
    uint16_t pid,
    void * start,
    uint32_t stack_size,
    uint8_t priority,
    uint8_t priv,
    char *name,
    uint8_t argc,
    uint32_t *args
);

extern void thr_switch(void *stack, thread_t *next_thr);
extern void thr_run(thread_t *thr);
extern void thr_null();
extern int  thr_next_tid();
extern void print_thread_t(char *var_name, thread_t *thread);
extern char thread_t_cmp_priority(list_item_t a, list_item_t b);
extern char sleep_entry_t_cmp_time(list_item_t a, list_item_t b);

/*
 * Currently running thread.
 */
extern thread_t    *thr_current;

/*
 * Thread Lists
 */
extern list_head_t  thr_threads;  // List of all threads.
extern olist_head_t thr_ready;    // List of threads ready to run.
extern list_head_t  thr_sem;      // List of threads waiting on a semaphore.
extern list_head_t  thr_msg;      // List of threads waiting on a message.
extern olist_head_t  thr_sleep;   // List of threads sleeping.

#endif
