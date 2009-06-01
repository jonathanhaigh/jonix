/* ---------------------------------------------------
* Jonix
*
* thr.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "thr.h"
#include "mem.h"
#include "c/string.h"
#include "devs/scrn.h"
#include "gdt.h"
#include "isr.h"
#include "time.h"
#include "user/unistd.h"

char thr_err[ERR_STR_SIZE];// thread error string

list_head_t  thr_threads;   // List of all threads.
olist_head_t thr_ready;     // List of threads ready to run.
list_head_t  thr_sem;       // List of threads waiting on a semaphore.
list_head_t  thr_msg;       // List of threads waiting on a message.
olist_head_t thr_sleep;     // List of threads sleeping.

thread_t    *thr_current;  // Currently running thread.

idarray_t thr_tids;


/*  ----------------------------------------------------
 *  Function:       thr_init
 *  --------------------------------------------------*/
void thr_init(){

    // Initialise thread lists.
    //
    thr_threads = list_new();
    thr_ready   = olist_new(thread_t_cmp_priority);
    thr_sem     = list_new();
    thr_msg     = list_new();
    thr_sleep   = olist_new(sleep_entry_t_cmp_time);

    // Initialise TID array
    //
    thr_tids    = idarray_new(THR_MAX_TID-1);
    if(idarray_failed(&thr_tids)){
        PANIC("Failed to allocate array for TIDs.");
    }

    // Create the null thread
    //
    thread_t *null_thr  =  thr_new(
        0,             // PID
        &thr_null,     // start of code to run
        10,            // Size of userspace stack
        0,             // priority,
        0,             // priv level
        "Null Thread", // name,
        0,             // argc,
        0              // Array of 32 bit arguments.
    );
    MALLOC_PANIC(null_thr);

    if(!olist_add(&thr_ready, null_thr)){
        PANIC("Failed to add the null thread to the list of ready threads.");    
    }

    // Setup the main kernel thread.
    //
    thr_current = kmalloc(sizeof(thread_t));
    MALLOC_PANIC(thr_current);

    thr_current->tid         = idlist_add(&thr_tids, thr_current);
    thr_current->pid         = thr_current->tid;
    thr_current->state       = THR_STATE_CURR;
    thr_current->priority    = 0xFF;
    thr_current->priv        = 0;
    thr_current->sem         = 0;
    thr_current->has_msg     = 0;
    thr_current->stack_base  = 0;
    thr_current->stack_size  = 0;
    thr_current->kstack_base = kmem_stack;
    thr_current->kstack_size = KERNEL_STACK_SIZE;
    thr_current->kstack_ptr  = 0; // Temporary value.
    thr_current->page_dir    = vpage_dir;
    strcpy(thr_current->name, "Kernel Main");

    if(!list_add(&thr_threads, thr_current)){
        PANIC("Failed to add main kernel thread to list of threads.");
    }
                            
}


/*  ----------------------------------------------------
 *  Function:       thr_new
 *
 *  Creates a new suspended thread.
 *  --------------------------------------------------*/
thread_t * thr_new(
    uint16_t    pid,        // Process associated with this thread, else 0 to create new PID.
    void *      start,      // Pointer to start of code to run.
    uint32_t    stack_size, // Size of userspace stack
    uint8_t     priority,
    uint8_t     priv,
    char *      name,
    uint8_t     argc,       // Number of arguments (args must be 32 bit).
    uint32_t *  args        // Array of 32 bit arguments.
){

    // Allocate memory for thread descriptor
    //
    thread_t *thread    = (thread_t *) kmalloc(sizeof(thread_t));
    if(thread == 0){
        ADD_ERR(thr, "Failed to allocate memory for new thread_t structure.");
        return 0;
    }

    // Allocate a TID
    //
    thread->tid = idarray_add(&thr_tids, thread);
    if(thread->tid == -1){
        ADD_ERR(thr, "Failed to allocate new TID for thread.");
        kfree(thread);
        return 0;
    }

    thread->pid         = pid || thread->tid; // If the given PID was zero then set the PID equal to the TID.
    thread->state       = THR_STATE_SUSP;
    thread->priv        = priv;
    thread->priority    = priority;
    thread->sem         = NULL;
    thread->has_msg     = 0;


    // Allocate a kernel mode stack
    //
    thread->kstack_base   = (uint32_t *) kmalloc(THR_KSTACK_SIZE) + THR_KSTACK_SIZE/sizeof(uint32_t);

    if(thread->kstack_base == 0){
        ADD_ERR(thr, "Failed to allocate memory for new thread's kernel mode stack.");
        kfree(thread);
        return 0;
    }
    thread->kstack_size   = THR_KSTACK_SIZE;

    if(priv > 0){
        // Allocate a user mode stack. This should be page aligned so that
        // paging permissions work nicely.
        //
        uint32_t no_pages    = stack_size / 4096 + (stack_size % 4096 ? 1 : 0);
        thread->stack_base   = (uint32_t *) kpsalloc(no_pages) + no_pages*4096/sizeof(uint32_t);
        if(thread->stack_base == 0){
            ADD_ERR(thr, "Failed to allocate memory for new thread's usermode stack.");
            kfree(thread->kstack_base);
            kfree(thread);
            return 0;
        }
        thread->stack_size   = no_pages * 4096;
    }
    else{
        thread->stack_base  = 0;
        thread->stack_size  = 0;
    }

    // Set up the kernel mode stack so that it looks like the thread
    // has been interrupted. This is so that we can iret to the thread.
    //
    // If jumping back to a usermode thread then the CPU expects two 
    // more things on the stack than if jumping back to a kernel mode
    // thread: the usermode stack pointer and stack segment.
    //
    // If jumping back to a kernel mode thread then only the kernel
    // mode stack is used so the CPU does not require these.
    //
    isr_stack_kernel_t *kstack;

    kstack    = (isr_stack_kernel_t *)(
                    (uint32_t)thread->kstack_base - 
                    (priv == 0? sizeof(isr_stack_kernel_t) : sizeof(isr_stack_user_t))
                );

    // TODO: update this depending on the privelege level of the thread.
    //

    kstack->gs          = GDT_SEG_DESCRIPTOR(GDT_INDEX_USER_DATA, 0);
    kstack->fs          = GDT_SEG_DESCRIPTOR(GDT_INDEX_USER_DATA, 0);
    kstack->es          = GDT_SEG_DESCRIPTOR(GDT_INDEX_USER_DATA, 0);
    kstack->ds          = GDT_SEG_DESCRIPTOR(GDT_INDEX_USER_DATA, 0);
    kstack->edi         = 0;
    kstack->esi         = 0;
    kstack->ebp         = 0;
    kstack->ebx         = 0;
    kstack->edx         = 0;
    kstack->ecx         = 0;
    kstack->eax         = 0;
    kstack->esp         = (uint32_t)(&(kstack->int_no));
    kstack->int_no      = 0;
    kstack->err_code    = 0;
    kstack->eip         = (uint32_t) start;
    kstack->cs          = GDT_SEG_DESCRIPTOR(GDT_INDEX_USER_CODE, 0);
    kstack->eflags      = 0x202;

    if(priv > 0){
        ((isr_stack_user_t *)kstack)->useresp   = (uint32_t) thread->stack_base;
        ((isr_stack_user_t *)kstack)->ss        = GDT_SEG_DESCRIPTOR(GDT_INDEX_USER_DATA, 0);
    }

    *((uint32_t *)kstack - 1)  = 0; // space used to pass stack pointer to fault_handler().

    thread->kstack_ptr  = (uint32_t *)kstack - 1;

    // If the thread has kernel privileges then it can use the kernel's
    // page directory. Otherwise we create a new page dir for the thread.
    //
    // TODO: make this better. don't just copy kernel's page dir.
    // TODO: Last pde needs to point to THIS page dir, not the kernel's
    // TODO: make a function to clone page dirs
    //
    if(priv == 0){
        thread->page_dir    = vpage_dir;
    }
    else{
        page_dir_t page_dir = kpalloc();
        if(page_dir == 0){
            ADD_ERR(thr, "Failed to allocate memory for new threads page directory.");
            kfree(thread->kstack_base);
            kfree(thread->stack_base);
            kfree(thread);
            return 0;
        }

        memcpy((void *) page_dir, (void *) vpage_dir, 4096);
        thread->page_dir = page_dir;

        // Set the flags on the user mode stack PTE so that a userspace thread can
        // use it.
        //
        // TODO: Does this really work?

        int no_pages    = thread->stack_size/4096;
        int i;
        for(i = 1; i <= no_pages; i++){
            // Flags=7  => page present, read/write, usermode
            //
            set_page_table_entry_flags(page_dir, (uint32_t) thread->stack_base - i*4096, 7);
        }
    }

    // Set the thread's name
    //
    uint32_t name_len   = strlen(name);
    if(name_len > THR_NAME_LEN){
        name_len    = THR_NAME_LEN;
    }
    memcpy(thread->name, name, name_len);
    thread->name[name_len]  = '\0';

    // Add to list of threads
    //
    if(!list_add(&thr_threads, thread)){
        ADD_ERR(thr, "Failed to add thread to list of threads.");
    }

    //TODO:
    // Need to add argc and argv to usermode/kernel stack.
    //

    return thread;
}


/*  ----------------------------------------------------
 *  Function:       thr_switch
 *  
 *  Switch current thread.
 *  --------------------------------------------------*/
void thr_switch(void *curr_state, thread_t *next_thr){

    //printf("switching from thread \"%s\" to thread \"%s\".\n", thr_current->name, next_thr->name);

    // TODO:
    // Do we need to load the kernel's page directory to make
    // sure we can see the new thread's thread_t or kernel stack?

    // Save current thread's stack pointer as it was just before
    // fault_handler/irq_handler was called.
    //
    thr_current->kstack_ptr = (uint32_t *) curr_state - 1;

    thr_current = next_thr;

    thr_run(next_thr);
}
    

/*  ----------------------------------------------------
 *  Function:       thr_run
 *  
 *  Starts running a thread.
 *  --------------------------------------------------*/
void thr_run(thread_t *thr){

    // switch to new thread's page directory.
    //
    asm volatile("          \
        mov %0, %%eax;      \
        mov %%eax, %%cr3;"
        :
        : "r" (addr_v2p(thr->page_dir))
        : "eax"
    );


    // put new thread's kernel mode stack pointer in the kernel's TSS.
    //
    tss_kernel.esp0    = (uint32_t) thr->kstack_ptr;

    // switch to new thread's kernel stack.
    // We don't have to change SS because all kernel stack segments are the same.
    //
    asm volatile (
        "mov %0, %%esp"
        :
        : "r" ((uint32_t) thr->kstack_ptr)
    );

    // jump to end of interrupt handler
    asm("jmp end_of_isr");
}


/*  ----------------------------------------------------
 *  Function:       thr_null
 *  
 *  The null thread function. Runs when no other thread
 *  is ready.
 *  --------------------------------------------------*/
void thr_null(){
    while(1) asm("hlt");
}


/*  ----------------------------------------------------
 *  Function:       thread_t_cmp_priority
 *
 *  Comparison function to sort the list of ready 
 *  threads.
 *  --------------------------------------------------*/
DEFINE_CMP_MEMBER(thread_t, priority, uint8_t, -1)


/*  ----------------------------------------------------
 *  Function:       sleep_entry_t_cmp_time
 *
 *  Comparison function to sort the list of sleeping
 *  threads.
 *  --------------------------------------------------*/
DEFINE_CMP_MEMBER(sleep_entry_t, time, uint32_t, 1);
