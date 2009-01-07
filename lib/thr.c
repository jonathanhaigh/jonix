/* ---------------------------------------------------
* Jonix
*
* thr.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "thr.h"
#include "tid.h"
#include "mem.h"
#include "string.h"
#include "scrn.h"

char thr_err[ERR_STR_SIZE];// thread error string

list_head_t thr_threads;   // List of all threads.
list_head_t thr_ready;     // List of threads ready to run.
list_head_t thr_sem;       // List of threads waiting on a semaphore.
list_head_t thr_msg;       // List of threads waiting on a message.
list_head_t thr_sleep;     // List of threads sleeping.

thread_t    *thr_current;  // Currently running thread.

uint16_t thr_last_tid;     // tid most recently allocated.
uint16_t thr_last_pid;     // pid most recently allocated.

/*  ----------------------------------------------------
 *  Function:       thr_init
 *  --------------------------------------------------*/
void thr_init(){

    // Initialise the TID module
    //
    tid_init();

    // Initialise thread lists.
    //
    thr_threads = list_new();
    thr_ready   = list_new();
    thr_sem     = list_new();
    thr_msg     = list_new();
    thr_sleep   = list_new();

    // Create the null thread
    // TID=PID=0
    //
    thread_t *null_thr  =  thr_create(
        0,          // PID
        &thr_null,  // start of code to run
        10,         // Size of userspace stack
        0,          // priority,
        "Null",     // name,
        0,          //     argc,       // Number of arguments (args must be 32 bit).
        0           // Array of 32 bit arguments.
    );

    list_add(&thr_threads, null_thr);
    list_add(&thr_ready, null_thr);

    thr_current = null_thr;

    print_thread_t("thr", null_thr);
}

/*  ----------------------------------------------------
 *  Function:       thr_create
 *
 *  Creates a new suspended thread.
 *  --------------------------------------------------*/
thread_t * thr_create(
    uint16_t    pid,        // Process associated with this thread.
    void *      start,      // Pointer to start of code to run.
    uint32_t    stack_size, // Size of userspace stack
    uint8_t     priority,
    char *      name,
    uint8_t     argc,       // Number of arguments (args must be 32 bit).
    uint32_t *  args        // Array of 32 bit arguments.
){
    // Allocate memory for thread descriptor
    //
    thread_t *thread    = (thread_t *) kmalloc(sizeof(thread_t));
    MALLOC_PANIC(thread);

    // Allocate a TID
    //
    tid_alloc(thread);

    // If the given PID was zero then set the PID equal to the TID.
    //
    thread->pid = pid || thread->tid;

    // Thread is initially suspended
    //
    thread->state    = THR_SUSP;

    // Allocate a kernel mode stack
    //
    thread->kstack_base   = (uint32_t) kmalloc(THR_KSTACK_SIZE) + THR_KSTACK_SIZE;
    MALLOC_PANIC(thread->kstack_base);
    thread->kstack_size   = THR_KSTACK_SIZE;

    // Allocate a user mode stack. This should be page aligned so that
    // paging permissions work nicely.
    //
    uint32_t no_pages    = stack_size / 4096 + (stack_size % 4096 ? 1 : 0);
    thread->stack_base   = (uint32_t) kpsalloc(no_pages) + no_pages*4096;
    MALLOC_PANIC(thread->stack_base);
    thread->stack_size   = no_pages * 4096;


    // Create a page directory and copy entries from the kernel's page dir.
    //
    page_dir_t page_dir = kpalloc();

    MALLOC_PANIC(page_dir);

    memcpy(page_dir, (void *) vpage_dir, 4096);

    thread->page_dir = (uint32_t) page_dir;


    // Set the flags on the user mode stack PTE so that a userspace thread can
    // use it.
    //
    int i;
    for(i = 1; i <= no_pages; i++){
        // Flags=7  => page present, read/write, usermode
        //
        set_page_table_entry_flags(page_dir, thread->stack_base - i*4096, 7);
    }



    // Set priority
    //
    thread->priority     = priority;


    // Thread initially is not waiting on semaphore and has
    // no messages.
    //
    thread->sem          = NULL;
    thread->has_msg      = 0;


    // Set the thread's name
    //
    uint32_t name_len   = strlen(name);
    if(name_len > THR_NAME_LEN){
        name_len    = THR_NAME_LEN;
    }
    memcpy(thread->name, name, name_len);
    thread->name[name_len]  = '\0';


    // Set the initial register contents
    //
    thread->cr3     = (uint32_t) page_dir;
    thread->eip     = (uint32_t) start;
    thread->eflags  = 514;    // See processor manuals for details.
    thread->esp     = thread->stack_base;
    thread->ebp     = thread->stack_base;

    thread->eax     = 0;
    thread->ebx     = 0;
    thread->ecx     = 0;
    thread->edx     = 0;
    thread->esi     = 0;
    thread->edi     = 0;

    thread->cs      = 8;    // See boot.asm for segment selector details
    thread->ds      = 16;   
    thread->es      = 16;
    thread->fs      = 16;
    thread->gs      = 16;
    thread->ss      = 16;


    //thread->no_threads  = 1;
    //
    
    return thread;
}


/*  ----------------------------------------------------
 *  Function:       thr_switch
 *  
 *  Switch current thread.
 *  --------------------------------------------------*/
//bool thr_switch(tid){
//}


/*  ----------------------------------------------------
 *  Function:       thr_null
 *  
 *  The null thread function. Runs when no other thread
 *  is ready.
 *  --------------------------------------------------*/
void thr_null(){
    for(;;);
}


/*  ----------------------------------------------------
 *  Function:       print_thread_t
 *
 *  Prints contents of a thread_t struct.
 *  --------------------------------------------------*/
void print_thread_t (char *var_name, thread_t *thread){

    scrn_put_var_h(var_name, (uint32_t) thread); 
    scrn_put_var_str("name", thread->name); 
    scrn_put_var_h("tid", thread->tid);
    scrn_put_var_h("pid", thread->pid);
    scrn_put_var_h("state", thread->state);
    scrn_put_var_h("priority", thread->priority);
    scrn_put_var_h("sem", thread->sem);
    scrn_put_var_h("has_msg", thread->has_msg);
    scrn_put_var_h("stack_base", thread->stack_base);
    scrn_put_var_h("stack_size", thread->stack_size);
    scrn_put_var_h("kstack_base", thread->kstack_base);
    scrn_put_var_h("kstack_size", thread->kstack_size);
    scrn_put_var_h("page_dir", thread->page_dir);

    /*
     * Register contents
     */
//    uint32_t cr3;
//    uint32_t eip;
//    uint32_t eflags;
//    uint32_t eax;
//    uint32_t ecx;
//    uint32_t edx;
//    uint32_t ebx;
//    uint32_t esp;
//    uint32_t ebp;
//    uint32_t esi;
//    uint32_t edi;
//    uint16_t es;
//    uint16_t cs;
//    uint16_t ss;
//    uint16_t ds;
//    uint16_t fs;
//    uint16_t gs;

}
