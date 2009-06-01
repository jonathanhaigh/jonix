/* ---------------------------------------------------
* Jonix
*
* unistd.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "user/unistd.h"
#include "c/stdint.h"

/*  ----------------------------------------------------
 *  Functions:         syscall_n
 *
 *  Makes a system call with n parameters.
 *  --------------------------------------------------*/

uint32_t syscall_0(uint32_t syscall){
    uint32_t retval;
    asm volatile(
        "int $0x80"
        : "=a" (retval)
        : "0" (syscall)
    );
    return retval;
}

uint32_t syscall_1(uint32_t syscall, uint32_t arg1){
    uint32_t retval;
    asm volatile(
        "int $0x80"
        : "=a" (retval)
        : "0" (syscall), "b" (arg1)
    );
    return retval;
}

uint32_t syscall_2(uint32_t syscall, uint32_t arg1, uint32_t arg2){
    uint32_t retval;
    asm volatile(
        "int $0x80"
        : "=a" (retval)
        : "0" (syscall), "b" (arg1), "c" (arg2)
    );
    return retval;
}

uint32_t syscall_3(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3){
    uint32_t retval;
    asm volatile(
        "int $0x80"
        : "=a" (retval)
        : "0" (syscall), "b" (arg1), "c" (arg2), "d" (arg3)
    );
    return retval;
}

uint32_t syscall_4(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4){
    uint32_t retval;
    asm volatile(
        "int $0x80"
        : "=a" (retval)
        : "0" (syscall), "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
    );
    return retval;
}

uint32_t syscall_5(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5){
    uint32_t retval;
    asm volatile(
        "int $0x80"
        : "=a" (retval)
        : "0" (syscall), "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)
    );
    return retval;
}


/*  ----------------------------------------------------
 *  Function:       sleep
 *  --------------------------------------------------*/
unsigned sleep(unsigned secs){
    return syscall_2(0, secs, 's');
}

