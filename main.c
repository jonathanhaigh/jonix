/* ---------------------------------------------------
* Jonix
*
* main.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "system.h"
#include "system_boot.h"
#include "ata.h"
#include "pci.h"
#include "util.h"
#include "scrn.h"
#include "mem.h"
#include "isr.h"
#include "gdt.h"
#include "irq.h"
#include "idt.h"
#include "kb.h"
#include "time.h"

/*  ----------------------------------------------------
 *  Function:       main
 *  --------------------------------------------------*/
int main(multiboot_info_t *mb_info){

    mem_total = 1024 + mb_info->mem_upper;

    scrn_clear();
    scrn_puts("Welcome to Jonix\n\n");

    char str[10];

    /*
    long2str_10h(str, mb_info->mem_lower * 1024, 4);
    scrn_puts("Total Memory Before 1MB: ");
    scrn_puts(str);

    long2str_10h(str, mb_info->mem_upper * 1024, 4);
    scrn_puts("\nTotal Memory After 1MB: ");
    scrn_puts(str);
    */

    long2str_10h(str, mem_total*1024, 5);
    scrn_puts("Total System Memory: ");
    scrn_puts(str);
    long2str_10h(str, (unsigned int)&kpage_dir, 4);
    scrn_puts("\nStart of page directory: ");
    scrn_puts(str);
    long2str_10h(str, (unsigned int)&kmem_stack, 4);
    scrn_puts("\nStart of kernel stack: ");
    scrn_puts(str);


    /*
    long2str_10h(str, (unsigned int)&kmem_start, 4);
    scrn_puts("\n\nStart of kernel in memory: ");
    scrn_puts(str);

    long2str_10h(str, (unsigned int)&kmem_text_start, 4);
    scrn_puts("\nStart of kernel text section in memory: ");
    scrn_puts(str);

    long2str_10h(str, (unsigned int)&kmem_rodata_start, 4);
    scrn_puts("\nStart of kernel rodata section in memory: ");
    scrn_puts(str);

    long2str_10h(str, (unsigned int)&kmem_data_start, 4);
    scrn_puts("\nStart of kernel data section in memory: ");
    scrn_puts(str);

    long2str_10h(str, (unsigned int)&kmem_bss_start, 4);
    scrn_puts("\nStart of kernel bss section in memory: ");
    scrn_puts(str);


    long2str_10h(str, (unsigned int)&kmem_end, 4);
    scrn_puts("\nEnd of kernel in memory: ");
    scrn_puts(str);
    */

    scrn_puts("\n\nInstalling GDT...");
    gdt_init();
    scrn_puts("Done\n");

    scrn_puts("Installing IDT...");
    idt_init();
    scrn_puts("Done\n");

    scrn_puts("Installing CPU Exception ISRs...");
    isr_init();
    scrn_puts("Done\n");

    scrn_puts("Installing IRQ ISR Framework...");
    irq_init();
    scrn_puts("Done\n");

    scrn_puts("Initialising memory management structures...");
    mem_init();
    scrn_puts("Done\n");

    /*
    scrn_puts("Initialising ATA Devices...\n");
    ata_init();
    scrn_puts("Done\n");
    */

    scrn_puts("Initialising Time Functions...");
    time_init();
    scrn_puts("Done\n");

    scrn_puts("Initialising Keyboard...");
    kb_init();
    scrn_puts("Done\n");

    scrn_puts("Enabling Interrupts...");
    __asm__ __volatile__("sti");
    scrn_puts("Done\n");

    scrn_puts("Scanning PCI Bus...");
    pci_init();
    scrn_puts("Done\n");

    scrn_puts("Initialising ATA Devices...");
    ata_init();
    scrn_puts("Done\n");

    /* scrn_puts("Initialising threads module...");
    thr_init();
    scrn_puts("Done\n");
    */

    kb_print_to_screen  = 1;

    for(;;);
    return 1;
}
