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
#include "list.h"

/*
 * Thread state constants
 */
#define THR_CURR  0
#define THR_READY 1
#define THR_SEM   2
#define THR_SUSP  3
#define THR_SLEEP 4


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

    uint32_t    sem;        // Pointer to semaphore process is waiting on.

    bool        has_msg;    // Does this proc/thread have msg waiting 
                            // to be read?
                            
    uint32_t    stack_base; // User mode stack for this thr
    uint32_t    stack_size;

    uint32_t    kstack_base; // Kernel mode stack for this thr
    uint32_t    kstack_size;

    uint32_t    page_dir;   // Address of this thread's page dir

    char        name[THR_NAME_LEN+1]; // process name

    /*
     * Register contents
     */
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t cs;
    uint16_t ss;
    uint16_t ds;
    uint16_t fs;
    uint16_t gs;

} thread_t;

/* 
 * Functions from thread.c
 */
extern void thr_init();
extern thread_t * thr_create(uint16_t pid, void * start, uint32_t stack_size, uint8_t priority, char *name, uint8_t argc, uint32_t *args);
extern void resched();
extern void thr_null();
extern void print_thread_t(char *var_name, thread_t *thread);

/*
 * Currently running thread.
 */
extern thread_t    *thr_current;

/*
 * Thread Lists
 */
extern list_head_t thr_threads;   // List of all threads.
extern list_head_t thr_ready;     // List of threads ready to run.
extern list_head_t thr_sem;       // List of threads waiting on a semaphore.
extern list_head_t thr_msg;       // List of threads waiting on a message.
extern list_head_t thr_sleep;     // List of threads sleeping.

#endif
