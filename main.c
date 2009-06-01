/* ---------------------------------------------------
* Jonix
*
* main.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "c/stdio.h"
#include "system.h"
#include "system_boot.h"
#include "devs/ata.h"
#include "devs/pci.h"
#include "devs/scrn.h"
#include "mem.h"
#include "isr.h"
#include "gdt.h"
#include "irq.h"
#include "idt.h"
#include "devs/kb.h"
#include "time.h"
#include "thr.h"
#include "syscall.h"
#include "c/fcntl.h"
#include "filesystems/vfs.h"

/*  ----------------------------------------------------
 *  Function:       main
 *  --------------------------------------------------*/
int main(multiboot_info_t *mb_info){

    mem_total = 1024 + mb_info->mem_upper;

    scrn_clear();
    printf("Booting Jonix...\n\n");

    printf("Total system memory: %H\n\n", mem_total*1024);

    printf("Installing GDT...");
    gdt_init();
    printf("Done\n");

    printf("Installing IDT...");
    idt_init();
    printf("Done\n");

    printf("Installing CPU exception ISRs...");
    isr_init();
    printf("Done\n");

    printf("Installing IRQ framework...");
    irq_init();
    printf("Done\n");

    printf("Installing system call framework...");
    syscall_init();
    printf("Done\n");

    printf("Initialising memory management structures...");
    mem_init();
    printf("Done\n");

    printf("Initialising Time Functions...");
    time_init();
    printf("Done\n");

    printf("Initialising Keyboard...");
    kb_init();
    printf("Done\n");

    printf("Scanning PCI Bus...");
    pci_init();
    printf("Done\n");

    printf("Initialising ATA Devices...");
    ata_init();
    printf("Done\n");

    printf("Initialising threads module...");
    thr_init();
    printf("Done\n");

    printf("Initialising VFS...");
    vfs_init();
    printf("Done\n");


    kb_print_to_screen  = 1;

    printf("Enabling Interrupts...");
    asm volatile("sti");
    printf("Done\n");

    // A timer interrupt will start the scheduler.
    // 

    

    while(1) asm("hlt");

    return 1;
}

