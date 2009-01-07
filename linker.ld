/* ---------------------------------------------------
* Jonix
*
* linker.ld
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

ENTRY (loader)

SECTIONS{
    . = 0x00100000;
    kmem_start_var  = .;

    .text :{
        kmem_text_start_var = .;
        *(.text)
    }

    .rodata ALIGN (0x1000) : {
        kmem_rodata_start_var   = .;
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
        kmem_data_start_var = .;
        *(.data)
    }

    .bss ALIGN (0x1000) : {
        kmem_bss_start_var = .;
        *(COMMON)
        *(.bss)
    }
    kmem_end_var   = .;
}
