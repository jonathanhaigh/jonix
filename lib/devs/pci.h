/* ---------------------------------------------------
* Jonix
*
* pci.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _PCI_H
#define _PCI_H

#include "system.h"
#include "util/list.h"

/*  ----------------------------------------------------
 *  Typedef struct:     pci_dev_t
 *  --------------------------------------------------*/
typedef struct {
    uint8_t     bus;
    uint8_t     slot;
    uint8_t     func;
    uint16_t    dev_id;
    char        **name;
    char        **desc;
    uint16_t    vendor_id;
    char        **vendor_short;
    char        **vendor_long;
    uint8_t     class_id;
    char        **class_desc;
    uint8_t     subclass_id;
    char        **subclass_desc;
    uint8_t     revision_id;
    uint8_t     prog_if;
    char        **prog_if_desc;
    uint8_t     bist;
    uint8_t     header_type;
    uint8_t     latency;
    uint8_t     cache_line_size;
    uint32_t    bar0;
    uint32_t    bar1;
    uint32_t    bar2;
    uint32_t    bar3;
    uint32_t    bar4;
    uint32_t    bar5;
    uint16_t    subsys_id;
    uint16_t    subsys_vendor_id;
    uint8_t     int_pin;
    uint8_t     int_line;
} pci_dev_t;


/*
 * Functions
 */
extern void pci_init();
extern list_head_t * pci_search_bus(uint8_t bus);
extern pci_dev_t * pci_dev_get_info(uint8_t bus, uint8_t dev, uint8_t func);
extern uint32_t pci_config_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);
extern void pci_dev_t_print(pci_dev_t * dev);
extern void pci_found_dev(pci_dev_t *dev);
extern void pci_dev_lookup_ids(pci_dev_t *dev);

#endif

