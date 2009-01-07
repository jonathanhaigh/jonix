/* ---------------------------------------------------
* Jonix
*
* ata.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/
#ifndef _ATA_H
#define _ATA_H

#include "system.h"
#include "sem.h"
#include "list.h"
#include "isr.h"

#define ATA_BASE_1       0x01F0
#define ATA_BASE_2       0x0170
#define ATA_BASE_3       0x01E8
#define ATA_BASE_4       0x0168

#define ATA_DATA         0x0000
#define ATA_ERR          0x0001
#define ATA_SEC_CNT      0x0002
#define ATA_SEC_NUM      0x0003
#define ATA_LBA_1        0x0003
#define ATA_CYL_LOW      0x0004
#define ATA_LBA_2        0x0004
#define ATA_CYL_HIGH     0x0005
#define ATA_LBA_3        0x0005
#define ATA_DRIVE_SELECT 0x0006
#define ATA_STATUS       0x0007
#define ATA_CMD          0x0007
#define ATA_ALT_STATUS   0x0206
#define ATA_DRIVE_ADDR   0x0207

#define ATA_BM_CMD       0x00
#define ATA_BM_STATUS    0x02
#define ATA_BM_PRDT_ADDR 0x04


/*  ----------------------------------------------------
 *  Typedef struct:     ata_dev_t
 *  --------------------------------------------------*/
typedef struct {
    sem_t       sem;            // semaphore lock on this device
    bool        atapi;          // Is this dev ATAPI?
    bool        use_dma;        // Can we use DMA?
    char        serial[21];     // serial number
    char        model[41];      // model
    uint8_t     dma_sup;        // Supported DMA modes
    uint8_t     dma_sel;        // Selected DMA modes
    uint8_t     udma_sup;       // Supported UDMA modes
    uint8_t     udma_sel;       // Selected UDMA modes
    uint64_t    sec_cnt_24;     // Number of addressable sectors with LBA24
    uint64_t    sec_cnt_48;     // Number of addressable sectors with LBA48 (if applicable);
    bool        lba48;          // Use LBA48 addressing (instead of LBA24).
} ata_dev_t;

/*  ----------------------------------------------------
 *  Typedef struct:     ata_prd_t
 *  --------------------------------------------------*/
typedef struct {
    uint32_t    buffer;     // This is the PHYSICAL address of the data buffer.
    uint16_t    byte_cnt;      // Transfer size in bytes.
    uint8_t     reserved;   // Set to 0.
    uint8_t     eot;        // Set to 0x80 for end of table, 0x00 otherwise.
} __attribute__((__packed__)) ata_prd_t;

/*  ----------------------------------------------------
 *  Typedef struct:     ata_ctlr_t
 *  --------------------------------------------------*/
typedef struct {
    sem_t       sem;            // Semaphore lock. You must acquire the lock on a device before
                                // locking its controller!!
    uint16_t    base;
    uint32_t    busmaster;
    bool        dma_simplex;    // Can use both devs at once?
    ata_prd_t   *prdt;          // Physical Region Descriptor Table
    ata_dev_t   *devs[2];       // Attached devices.
    bool        sel_dev;        // Which device is currently selected?
} ata_ctlr_t;


/*
 * Variables
 */
extern char ata_err[ERR_STR_SIZE];
extern list_head_t ata_devs;
extern list_head_t ide_ctlrs;

/*
 * Setup Functions
 */
extern void ata_init();
extern void ata_find_devs();
extern void ata_ctlr_find_devs(ata_ctlr_t *ctlr);

/*
 * Read Functions
 */
extern bool ata_dma_lba28_read(ata_ctlr_t *ctlr, bool dev, uint32_t lba, uint8_t sec_cnt, void *buffer);


/*
 * Misc Functions
 */
extern void ata_get_error(ata_ctlr_t *ctlr);
extern void ata_select_dev(ata_ctlr_t *ctlr, char drive);
extern void ata_irq_handler(isr_stack_t *r);

#endif

