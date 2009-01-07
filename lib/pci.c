/* ---------------------------------------------------
* Jonix
*
* pci.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "pci.h"
#include "scrn.h"
#include "mem.h"
#include "util.h"
#include "io.h"

#define PCI_CONFIG_ADDRESS 0x0CF8
#define PCI_CONFIG_DATA     0x0CFC  

char pci_err[ERR_STR_SIZE];

list_head_t pci_devs;
list_head_t pci_tree;

list_head_t ide_ctlrs;

/*  ----------------------------------------------------
 *  Function:       pci_init
 *  --------------------------------------------------*/
void pci_init(){
    pci_tree    = *pci_search_bus(0);    

    pci_dev_t *dev;

    if(pci_devs.size > 0){
        scrn_puts("\nFound PCI Devices:\n\tBus,Slot,Func\tVendor\tDevice\tClass\tSubclass\n");
    }
    foreach(dev, &pci_devs){
        scrn_puts("\t");
        scrn_putn(dev->bus);
        scrn_puts(",");
        scrn_putn(dev->slot);
        scrn_puts(",");
        scrn_putn(dev->func);
        scrn_puts("\t\t");
        scrn_putn_16(dev->vendor_id);
        scrn_puts("\t");
        scrn_putn_16(dev->dev_id);
        scrn_puts("\t");
        scrn_putn_16(dev->class_id);
        scrn_puts("\t");
        scrn_putn_16(dev->subclass_id);
        scrn_puts("\n");
    }
}

/*  ----------------------------------------------------
 *  Function:       pci_search_bus
 *  --------------------------------------------------*/
// TODO: Make this function recurse when it finds a PCI-PCI bridge.
//
list_head_t * pci_search_bus(uint8_t bus){
    list_head_t *devs   = NULL;
    pci_dev_t   *dev;
    int i;
    int j;
    for(i=0; i<5; i++){
        for(j=0; j<8; j++){

            dev = pci_dev_get_info(bus, i, j);

            if(dev == NULL){
                break;
            }

            pci_found_dev(dev);

            if(devs == NULL){
                devs    = (list_head_t *) kmalloc(sizeof(list_head_t));
                *devs   = list_new();
            }
            list_add(devs, dev);
        }
    }
    return devs;
}

/*  ----------------------------------------------------
 *  Function:       pci_found_dev
 *  --------------------------------------------------*/
void pci_found_dev(pci_dev_t *dev){

    list_add(&pci_devs, dev);

    switch(dev->class_id){

        case 0x01:   // Mass Storage Controllers
            switch(dev->subclass_id){
                //case 0x00: list_add(&scsi_ctlrs, dev);      break; // SCSI
                case 0x01: list_add(&ide_ctlrs, dev);       break; // IDE
                //case 0x02: list_add(&floppy_ctlrs, dev);    break; // Floppy
                //case 0x03: list_add(&ipi_ctlrs, dev);       break; // IPI
                //case 0x04: list_add(&raid_ctlrs, dev);      break; // RAID
                //case 0x80:                                  break; // Other Mass Storage
            }
            break;

//        case 0x02: // Network Controllers
//            switch(dev->subclass_id){
//                case 0x00: list_add(&eth_ctlrs, dev);   break; // Ethernet
//                case 0x01:                              break; // Token Ring
//                case 0x02:                              break; // FDDI 
//                case 0x03:                              break; // ATM 
//                case 0x80:                              break; // Other Network 
//            }
//            break;

//        case 0x03: // Display Controllers
//            break;

    }
                    

}


/*  ----------------------------------------------------
 *  Function:       pci_dev_get_info
 *  --------------------------------------------------*/
pci_dev_t * pci_dev_get_info(uint8_t bus, uint8_t slot, uint8_t func){

    uint32_t tmp    = pci_config_read(bus, slot, func, 0);

    if((tmp & 0xFFFF) == 0xFFFF){
        return NULL; // Vendor ID of 0xFFFF signifies non-existent device.
    }

    pci_dev_t *dev  = (pci_dev_t *) kmalloc(sizeof(pci_dev_t));
    MALLOC_PANIC(dev);

    dev->dev_id          = tmp >> 16;
    dev->vendor_id       = tmp & 0xFFFF;

    tmp                  = pci_config_read(bus, slot, func, 2);
    dev->class_id        = tmp >> 24;
    dev->subclass_id     = tmp >> 16 & 0xFF;
    dev->revision_id     = tmp & 0xFFFF;

    dev->bar0            = pci_config_read(bus, slot, func, 4);
    dev->bar1            = pci_config_read(bus, slot, func, 5);
    dev->bar2            = pci_config_read(bus, slot, func, 6);
    dev->bar3            = pci_config_read(bus, slot, func, 7);
    dev->bar4            = pci_config_read(bus, slot, func, 8);
    dev->bar5            = pci_config_read(bus, slot, func, 9);

    tmp                  = pci_config_read(bus, slot, func, 11);
    dev->subsys_id       = tmp >> 16;
    dev->subsys_vendor_id= tmp & 0xFFFF;

    tmp                  = pci_config_read(bus, slot, func, 15);
    dev->int_pin         = (tmp >> 8) & 0xFF;
    dev->int_line        = tmp & 0xFF;

    dev->bus             = bus;
    dev->slot            = slot;
    dev->func            = func;

    return dev;
}

/*  ----------------------------------------------------
 *  Function:       pci_config_read
 *  --------------------------------------------------*/
uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t reg){

    /*
    The CONFIG_ADDRESS port is a 32 bit register to select what info is to be read from the CONFIG_DATA port.
    ----------------------------------------------------------------------------------------------------
    31          30 - 24     23 - 16     15 - 11         10 - 8              7 - 2               1 - 0
    Enable Bit  Reserved    Bus Number  Device Number   Function Number     Register Number     00 
    ----------------------------------------------------------------------------------------------------
    */

    outl(PCI_CONFIG_ADDRESS, (bus << 16) | (slot << 11) | (func << 8) | (reg << 2) | 0x80000000);

    return inl(PCI_CONFIG_DATA);
}

/*  ----------------------------------------------------
 *  Function:       pci_dev_t_print
 *
 *  Print contents of a pci_dev_t structure to the
 *  screen.
 *  --------------------------------------------------*/
void pci_dev_t_print(pci_dev_t * dev){
    scrn_puts("PCI DEVICE:\n");
    scrn_put_var("Bus\t", dev->bus);
    scrn_put_var("Slot\t", dev->slot);
    scrn_put_var("Function", dev->func);
    scrn_put_var_16("Device ID", dev->dev_id);
    scrn_put_var_16("Vendor ID", dev->vendor_id);
    scrn_put_var_16("Class ID", dev->class_id);
    scrn_put_var_16("Subclass ID", dev->subclass_id);
    scrn_put_var_16("Revision ID", dev->revision_id);
    scrn_put_var_16("BAR0\t", dev->bar0);
    scrn_put_var_16("BAR1\t", dev->bar1);
    scrn_put_var_16("BAR2\t", dev->bar2);
    scrn_put_var_16("BAR3\t", dev->bar3);
    scrn_put_var_16("BAR4\t", dev->bar4);
    scrn_put_var_16("BAR5\t", dev->bar5);
    scrn_put_var_16("Subsystem ID", dev->subsys_id);
    scrn_put_var_16("Subsystem Vendor ID", dev->subsys_vendor_id);
    scrn_put_var_16("Interrupt Pin", dev->int_pin);
    scrn_put_var_16("Interrupt Line", dev->int_line);
}
