/* ---------------------------------------------------
* Jonix
*
* ata.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "devs/ata.h"
#include "io.h"
#include "devs/pci.h"
#include "sem.h"
#include "irq.h"
#include "time.h"
#include "c/string.h"
#include "mem.h"
#include "devs/ptbl.h"


char ata_err[ERR_STR_SIZE];

list_head_t ata_devs;       // List of ATA devices.

ata_ctlr_t *ata_ctlrs[4];   // Array of pointers to ATA controllers.

/*  ----------------------------------------------------
 *  Function:       ata_init
 *  --------------------------------------------------*/
void ata_init(){

    // Set the ATA interrupt handler
    //
    irq_set_handler(14, &ata_irq_handler);
    irq_set_handler(15, &ata_irq_handler);

    //ata_devs    = list_new();

    ata_find_devs();
}


/*  ----------------------------------------------------
 *  Function:       ata_find_devs
 *
 *  Finds ATA devices.
 *  --------------------------------------------------*/
void ata_find_devs(){


    // We look for ATA controllers, and for each found
    // we look for devices on that controller.
    //
    // We will check for controllers whose ports are at
    // the default addresses. If the status port
    // for a controller is set to 0xFF, then the controller
    // does not exist, otherwise it does.
    //
    
    uint16_t base_ports[4]  = {ATA_BASE_1, ATA_BASE_2, ATA_BASE_3, ATA_BASE_4};
    uint8_t  status;
    int      i;

    for(i=0; i<4; i++){

        status  = inb(base_ports[i] + ATA_ALT_STATUS);

        if(status == 0xFF){
            ata_ctlrs[i]    = NULL;
            continue;
        }

        ata_ctlrs[i]    = (ata_ctlr_t *) kmalloc(sizeof(ata_ctlr_t));
        MALLOC_PANIC(ata_ctlrs[i]);
        ata_ctlrs[i]->base          = base_ports[i];

        ata_ctlr_find_devs(ata_ctlrs[i]);

        // If this ctlr has no devices attached then we don't need to
        // do any more work.
        //
        if(ata_ctlrs[i]->devs[0] == NULL && ata_ctlrs[i]->devs[1] == NULL){
            ata_ctlrs[i]->sem           = sem_new(1);
            ata_ctlrs[i]->busmaster     = 0;
            ata_ctlrs[i]->dma_simplex   = 0;
            ata_ctlrs[i]->prdt          = 0;
            continue;
        }

        // Set the control register to zeros -
        // this makes sure interrupts are enabled for the controller
        //
        outb(ata_ctlrs[i]->base + ATA_CTL, 0x00);

        // Create a PRDT for this controller.
        //
        ata_ctlrs[i]->prdt  = ata_prdt_new();
        if(ata_ctlrs[i]->prdt == NULL){
            PANIC("Not enough memory to create PRDT\n");
        }

        // Each PCI IDE controller probably has two ATA controllers.
        // Assume that the first IDE controller found on the PCI bus
        // is for the primary and secondary ATA controllers.
        //
        uint32_t bar4;
        if(ide_ctlrs.size < 1 + (i/2) ||
            (   bar4 = ((pci_dev_t *)list_get(&ide_ctlrs, i/2))->bar4   ) == 0
        ){
            // Didn't find a PCI IDE controller for this ATA controller
            // or the address of the busmaster control block was given as 0 (wrong),
            // so we can't use DMA.
            //
            if(ata_ctlrs[i]->devs[0] != NULL){
                ata_ctlrs[i]->devs[0]->use_dma  = 0;
            }
            if(ata_ctlrs[i]->devs[1] != NULL){
                ata_ctlrs[i]->devs[1]->use_dma  = 0;
            }
        }
        else{
            // The base address of the busmaster is the high 28 bits of BAR4
            //
            ata_ctlrs[i]->busmaster = (bar4 & 0xFFFFFFF0) + 0x08*(i % 2);

            // Tell the busmaster about the physical addresss of the PRDT for this controller.
            //
            outl(ata_ctlrs[i]->busmaster + ATA_BM_PRDT_ADDR, addr_v2p(ata_ctlrs[i]->prdt));

            // TODO: Make sure this was written correctly to verify that this is actually the
            // busmaster register like we think it is.
            //

            char bm_status  = inb(ata_ctlrs[i]->busmaster + ATA_BM_STATUS);

            // Bit 7 indicates whether both busmaster channels (primary and secondary) can
            // be operated at the same time.
            //
            ata_ctlrs[i]->dma_simplex   = bm_status & BIT(7) ? 1 : 0;

            // Bits 5 and 6 indicate whether drive 0 and drive 1 are DMA capable.
            //
            if(ata_ctlrs[i]->devs[0] != NULL){
                ata_ctlrs[i]->devs[0]->use_dma  = bm_status & BIT(5) ? 1 : 0;
            }
            if(ata_ctlrs[i]->devs[1] != NULL){
                ata_ctlrs[i]->devs[1]->use_dma  = bm_status & BIT(6) ? 1 : 0;
            }
        }
    }
}


/*  ----------------------------------------------------
 *  Function:       ata_prdt_new
 *
 *  Create a physical region descriptor table.
 *  --------------------------------------------------*/
ata_prd_t * ata_prdt_new(){

    // The PRDT must be dword aligned and not on a 64K boundary.
    // It must also be contigous in physical memory. To arrange
    // this, we dword align and make sure it does not cross a page
    // boundary.
    //
    // To dword align we ask for 3 bytes more than needed so we can
    // manually align. We will never need to free this memory, so
    // it is not necessary to keep track of the original kmalloc
    // pointer.
    //
    // To make sure the prdt does not cross a page boundary we use trial
    // and error.
    //
    // Each PRDT will be three entries long.
    //

    ata_prd_t * prdt;

    list_head_t prdt_tries  = list_new(); // List of pointers to rejected PRDTs
    int timeout = 50;

    while(timeout > 0){

        void *tmp   = kmalloc(3*sizeof(ata_prd_t) + 3);
        if(tmp == NULL) return NULL;

        // Make sure the PRDT is on a dword boundary.
        //
        prdt  = (ata_prd_t *) (tmp + ((uint32_t) tmp % 4));

        // Check that the PRDT doesn't cross a page (4K) boundary.
        //
        if((uint32_t) prdt % (4096) <= (4096 - 2*sizeof(ata_prd_t))){

            // This PRDT is fine, so initialise to zeroes.
            //
            prdt->buffer    = 0;
            prdt->byte_cnt  = 0;
            prdt->reserved  = 0;
            prdt->eot       = 0;

            break;    
        }

        // Add to the list of failed PRDTs.
        //
        if(!list_add(&prdt_tries, tmp)) return NULL;

        timeout--;
    }

    if(timeout == 0) return NULL;

    // Free the list of rejected PRDT pointers,
    // including the PRDTs that they point to.
    //
    list_free(&prdt_tries, 1, 0);

    return prdt;
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_find_devs
 *
 *  Finds devices attached to an ATA controller
 *  --------------------------------------------------*/
void ata_ctlr_find_devs(ata_ctlr_t *ctlr){

    int i;
    for(i=0; i<2; i++){

        ata_ctlr_select_dev(ctlr, i); // Select drive - master or slave

        /*
         * Check that the ports for this drive work as
         * expected. We write to a couple of R/W ports and
         * then check to see if they still have the values
         * written.
         */
        outb(ctlr->base + ATA_SEC_NUM, 187 + (i % 2)*50);
        outb(ctlr->base + ATA_SEC_CNT, 59  + (i % 2)*50);


        uint8_t sec_num = inb(ctlr->base + ATA_SEC_NUM);
        uint8_t sec_cnt = inb(ctlr->base + ATA_SEC_CNT);

        if( sec_num != 187 + (i % 2)*50 || sec_cnt != 59 + (i % 2)*50){
            ctlr->devs[i]   = NULL;
            continue;
        }

        /* We have found a drive!
         */
        ctlr->devs[i]   = (ata_dev_t *) kmalloc(sizeof(ata_dev_t));
        MALLOC_PANIC(ctlr->devs[i]);

        ata_dev_t *dev  = ctlr->devs[i];

        dev->sem    = sem_new(1); // Create the device's semaphore lock.

        dev->parts  = list_new(); // Create the devices partitions list.

        // Software reset the drive.
        //
        if(!ata_ctlr_reset(ctlr)){
            ATA_ADD_ERR("Failed to software reset ATA controller.");
        }
        
        /* Get info about drive using IDENTIFY command
         */
        //asdf
        if(!ata_dev_identify(ctlr, i)){
            ATA_ADD_ERR("Failed to run IDENTIFY command on ATA device.");
            // Carry on silently
        }
        else{
            // Find partitions on drive.
            //
            ata_dev_find_parts(ctlr, i);
        }

    }
}


/*  ----------------------------------------------------
 *  Function:       ata_dev_find_parts
 *
 *  Finds partitions on ata device.
 *
 *  NO LOCKING
 *  --------------------------------------------------*/
bool ata_dev_find_parts(ata_ctlr_t *ctlr, bool devno){

    void *buffer    = kmalloc(512);

    // Read first sector of the disk into the buffer
    //
    if(!ata_pio_lba28_read(ctlr, devno, 0, 1, buffer)){
        ATA_ADD_ERR("Failed to read first sector of disk.");
        return 0;
    }

    // The partition table is at offset 0x01BE
    //
    if(!ptbl_parse(buffer + 0x01BE, &(ctlr->devs[(int) devno]->parts))){
        ATA_ADD_ERR(ptbl_err);
        ATA_ADD_ERR("Failed to parse partition table for ATA device.");
        return 0;
    }

    return 1;
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_reset
 *
 *  Software reset drives on a ctlr.
 *
 *  NO LOCKING
 *  --------------------------------------------------*/
bool ata_ctlr_reset(ata_ctlr_t *ctlr){

    // Reseting the drive will automatically select device 0 afterwards.
    // We save the currently selected dev number to change it back later.
    //
    uint8_t sel_dev = ctlr->sel_dev;

    outb(ctlr->base + ATA_CTL, ATA_CTL_RESET);

    if(!ata_ctlr_wait_bsy(ctlr)){
        ATA_ADD_ERR("Ctlr failed to set BSY after software reset.");
        return 0;
    }

    outb(ctlr->base + ATA_CTL, 0);

    if(!ata_ctlr_wait_not_bsy(ctlr)){

        ATA_ADD_ERR("Ctlr failed to clear BSY after software reset.");
        return 0;
    }

    if(!ata_ctlr_wait_rdy(ctlr)){

        ATA_ADD_ERR("Ctlr failed to set RDY after software reset.");
        return 0;
    }

    // reselect the originally selected device.
    //
    ctlr->sel_dev   = 0;
    ata_ctlr_select_dev(ctlr, sel_dev);

    return 1;
}

/*  ----------------------------------------------------
 *  Function:       ata_dev_identify
 *
 *  Uses IDENTIFY_DRIVE command to get info about an
 * ATA device (Not for ATAPI devs).
 *
 * NO LOCKING
 *  --------------------------------------------------*/
bool ata_dev_identify(ata_ctlr_t *ctlr, uint8_t devno){

    ata_ctlr_select_dev(ctlr, devno);

    ata_dev_t *dev  = ctlr->devs[devno];

    outb(ctlr->base + ATA_CMD, ATA_CMD_IDENTIFY_DRIVE);


    // If the device is ATAPI, SATAPI, or SATA then
    // then ATA_STATUS_ERR will be set.
    //
    // In this case we can detect the type of device
    // by looking at the ATA_LBA_2 and ATA_LBA_3 registers.
    //
    if(!ata_ctlr_wait_drq(ctlr)){
        if(inb(ctlr->base + ATA_STATUS) & ATA_STATUS_ERR){
           uint8_t lba2 = inb(ctlr->base + ATA_LBA_2); 
           uint8_t lba3 = inb(ctlr->base + ATA_LBA_3); 
           if(lba2 == 0x14 && lba3 == 0xEB){
               dev->atapi   = 1;
               return 0;
           }
           else return 0; // Found unknown ATA device (SATA or SATAPI prob.)
        }
        else return 0; // Leave error string as set by ata_ctlr_wait_drq().
    }


    // Read info
    //
    uint16_t id[256];
    int j;
    for(j=0; j<256; j++){
        id[j] = inw(ctlr->base + ATA_DATA);
    }

    // Extract the serial number.
    //
    for(j=0; j<10; j++){
        dev->serial[2*j]     = id[10+j] >> 8; 
        dev->serial[2*j+1]   = id[10+j] & 0xFF;
    }
    dev->serial[20]    = 0;

    // Extract the model name.
    //
    for(j=0; j<20; j++){
        dev->model[2*j]     = id[27+j] >> 8; 
        dev->model[2*j+1]   = id[27+j] & 0xFF;
    }
    dev->model[40]    = 0;

    // Extract DMA modes selected/supported.
    //
    // These are bit fields where bit n indicates whether DMA mode n 
    // is supported/selected.
    //
    dev->dma_sup  = id[63] & 0xFF;
    dev->dma_sel  = id[63] >> 8;
        
    // Get the number of 28 bit addressable sectors on the drive.
    //
    dev->sec_cnt_28 = *((uint32_t *) (id + 60));

    // Get whether drive supports LBA48 mode.
    //
    dev->lba48  = id[83] & 0x0400 ? 1 : 0;

    // Get the number of 48 bit addressable sectors on the drive.
    //
    if(dev->lba48){
        dev->sec_cnt_48 = *((uint64_t *) (id + 100));
    }
    else{
        dev->sec_cnt_48 = dev->sec_cnt_28;
    }

    // Extract Ultra DMA modes selected/supported.
    //
    // These are bit fields where bit n indicates whether Ultra DMA mode n 
    // is supported/selected.
    //
    dev->udma_sup  = id[88] & 0xFF;
    dev->udma_sel  = id[88] >> 8;

    return 1;

}

/*  ----------------------------------------------------
 *  Function:       ata_ctlr_get_error
 *
 *  Reads and interprets error register on ATA controller.
 *  Sets error message to ata_err.
 *
 *  No locking. Caller must aqcuire lock before using.
 *  --------------------------------------------------*/
void ata_ctlr_get_error(ata_ctlr_t *ctlr){

    uint8_t err    = inb(ctlr->base + ATA_ERR);

    if(err & 1){
        ATA_ADD_ERR("Data address mark not found after correct ID field found.");
    }
    else if(err & 2){
        ATA_ADD_ERR("Track 0 not found during execution of Recalibrate command.");
    }
    else if(err & 4){
		ATA_ADD_ERR("Command aborted due to drive status error or invalid command.");
    }
    else if(err & 8){
		ATA_ADD_ERR("Unkown error.");
    }
    else if(err & 16){
		ATA_ADD_ERR("Requested sector's ID field not found.");
    }
    else if(err & 32){
		ATA_ADD_ERR("Unkown error.");
    }
    else if(err & 64){
		ATA_ADD_ERR("Uncorrectable data error encountered.");
    }
    else if(err & 128){
		ATA_ADD_ERR("Bad block mark detected in the requested sector's ID field.");
    }
}

/*  ----------------------------------------------------
 *  Function:       ata_ctlr_select_dev
 *
 *  Changes selected dev (master or slave) on ATA
 *  controller.
 *
 *  If selected device was actually changed then this
 *  function waits a little for the controller to 
 *  keep up.
 *
 *  No locking. Caller must acquire lock before using.
 *  --------------------------------------------------*/
void ata_ctlr_select_dev(ata_ctlr_t *ctlr, bool dev){

    if(ctlr->sel_dev == dev){
        // Device is already selected.
        //
        return;
    }

    int i;

    if(dev == 0){
        // Master
        //
        outb(ctlr->base + ATA_DRIVE_SELECT, 0xA0);
        ctlr->sel_dev   = 0;

        for(i=0; i<5; i++){
            inb(ctlr->base + ATA_STATUS);
        }
        return;
    }
    if(dev == 1){
        // Slave
        //
        outb(ctlr->base + ATA_DRIVE_SELECT, 0xB0);
        ctlr->sel_dev   = 1;

        for(i=0; i<5; i++){
            inb(ctlr->base + ATA_STATUS);
        }
    }
}


/*  ----------------------------------------------------
 *  Function:       ata_pio_lba28_read
 *
 *  Reads from a drive using PIO and 28 bit addresssing.
 *
 *  NOTE: 0 is a special value for the sec_cnt parameter
 *  meaning 0x100.
 *
 *  Obtains necessary locks.
 *  --------------------------------------------------*/
bool ata_pio_lba28_read(ata_ctlr_t *ctlr, bool devno, uint32_t lba, uint8_t sec_cnt, void *buffer){

    uint16_t *buffer_w  = (uint16_t *) buffer;

    ata_dev_t *dev  = ctlr->devs[(int) devno];

    // Check that the required sectors are addressable.
    //
    if(lba + sec_cnt > dev->sec_cnt_28){
        ATA_ADD_ERR("Sectors to be read are not LBA28 addressable.");
        return 0;
    }

    // Obtain locks on controller and device.
    //
    //sem_wait(&(ctlr->devs[(int) devno]->sem)); // Must obtain device lock first!
    //sem_wait(&(ctlr->sem));

    ata_ctlr_select_dev(ctlr, devno);

    // Set the device/head register:
    //
    // bit 7 reserved
    // bit 6 LBA mode
    // bit 5 reserved
    // bit 4 device select
    // bits 3-0 LBA bits 24-27

    outb(ctlr->base + ATA_DRIVE_SELECT, BIT(6) | (devno << 4) | ((lba << 24) & 0x0F));

    outb(ctlr->base + ATA_SEC_CNT, sec_cnt);            // set the sector count
    outb(ctlr->base + ATA_LBA_1, lba & 0xFF);           // set LBA bits 1-7
    outb(ctlr->base + ATA_LBA_2, (lba >> 8) & 0xFF);    // set LBA bits 8-15
    outb(ctlr->base + ATA_LBA_3, (lba >> 16) & 0xFF);   // set LBA bits 16-23
    outb(ctlr->base + ATA_CMD, ATA_CMD_READ_SECTORS);   // set the read sectors command

    int i, j;
    for(i=0; i<sec_cnt; i++){
        if(!ata_ctlr_wait_drq(ctlr)){
            // Leave error string as set by ata_ctlr_wait_drq().
            return 0;
        }
        for(j=0; j<256; j++){
            *(buffer_w++) = inw(ctlr->base + ATA_DATA);
        }
    }

    return 1;

}



/*  ----------------------------------------------------
 *  Function:       ata_dma_lba28_read
 *
 *  Reads from a drive using DMA and 28 bit addresssing.
 *
 *  NOTE: 0 is a special value for the sec_cnt parameter
 *  meaning 0x100.
 *
 *  Obtains necessary locks.
 *  --------------------------------------------------*/
bool ata_dma_lba28_read(ata_ctlr_t *ctlr, bool devno, uint32_t lba, uint8_t sec_cnt, void *buffer){

    // Check drive supports DMA
    //
//    if(!ctlr->devs[(int) devno]->use_dma){
//        ATA_ADD_ERR("Drive does not support DMA.");
//        return 0;
//    }

    // Check that the required sectors are addressable.
    //
    if(lba + sec_cnt > ctlr->devs[(int) devno]->sec_cnt_28){
        ATA_ADD_ERR("Sectors to be read are not LBA28 addressable.");
        return 0;
    }

    // Obtain locks on controller and device.
    //
    sem_wait(&(ctlr->devs[(int) devno]->sem)); // Must obtain device lock first!
    sem_wait(&(ctlr->sem));

    ata_ctlr_select_dev(ctlr, devno);

    // Setup the PRDT for this read.
    //
    // We will use two entries in the PRDT because each PRDT
    // entry has a byte count upto 0x10000, which is less
    // than 0x100 (max sec_cnt) sectors.
    //
    // Each entry takes half the byte count,
    // but half the byte count is 0x100 times the sector count.
    // so we just put the sector count into the upper byte of
    // the byte_cnt field of the PRDT entry.
    // This still works if sector count is the special value
    // 0 (meaning 0x100) because 0 is a special value in the
    // byte count field (meaning 0x10000).
    // 
    ctlr->prdt[0].buffer    = addr_v2p(buffer);
    ctlr->prdt[0].byte_cnt  = ((uint16_t) sec_cnt) << 8;
    ctlr->prdt[0].eot       = 0;
    ctlr->prdt[1].buffer    = ctlr->prdt[0].buffer + (ctlr->prdt[0].byte_cnt || 0x10000);
    ctlr->prdt[1].byte_cnt  = ((uint16_t) sec_cnt) << 8;
    ctlr->prdt[1].eot       = 0x80;

    // Clear the interrupt and error bits of the busmaster status register.
    // This is done by sending 1s to these bits (bits 1 and 2).
    //
    outb(ATA_BM_STATUS, 0x06);

    // Send a 0 to the read/write bit of the command register of the busmaster.
    // The other bits should also be zero at this point.
    //
    outb(ATA_BM_CMD, 0x00);

    outb(ATA_SEC_CNT, sec_cnt);         // Send the sector count
    outb(ATA_LBA_1, lba & 0xFF);        // Send low byte of LBA
    outb(ATA_LBA_2, (lba >> 8) & 0xFF); // Send mid byte of LBA
    outb(ATA_LBA_3, lba >> 16);         // Send high byte of LBA
    outb(ATA_CMD, 0xC8);                // Issue the LBA28 DMA read command

    // Start the busmaster by writing to the start bit in the command register.
    //
    outb(ATA_BM_CMD, 0x01);

    // Wait for an interrupt!
    //
    //sem_wait(ctlr->devs[devno]->

    return 1;
}

/*  ----------------------------------------------------
 *  Function:       ata_irq_handler
 *  --------------------------------------------------*/
void ata_irq_handler(void *r){
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_wait_rdy
 *  --------------------------------------------------*/
bool ata_ctlr_wait_rdy(ata_ctlr_t *ctlr){

    uint8_t status;

    int i;

    for(i=0; i<ATA_WAIT_TIMEOUT; i++){

        status  = inb(ctlr->base + ATA_STATUS);

        if(status & ATA_STATUS_BSY) continue;

        if(status & ATA_STATUS_ERR || status & ATA_STATUS_DF){
            ATA_ADD_ERR("Error bit set in ATA controller status register.");
            return 0;
        }

        if(status & ATA_STATUS_RDY) return 1;
    }

    ATA_ADD_ERR("Timeout while waiting for RDY bit in ATA Controller status register.");
    return 0;
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_wait_not_bsy
 *  --------------------------------------------------*/
bool ata_ctlr_wait_not_bsy(ata_ctlr_t *ctlr){

    uint8_t status;

    int i;

    for(i=0; i<ATA_WAIT_TIMEOUT; i++){

        status  = inb(ctlr->base + ATA_STATUS);

        if(status & ATA_STATUS_BSY) continue;

        if(status & ATA_STATUS_ERR || status & ATA_STATUS_DF){
            ATA_ADD_ERR("Error bit set in ATA controller status register.");
            return 0;
        }

        return 1;
    }

    ATA_ADD_ERR("Timeout while waiting for BSY bit to clear in ATA Controller status register.");
    return 0;
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_wait_bsy
 * 
 *  WARNING: This function waits for the BSY status bit
 *  to be set, NOT for it to clear.
 *  --------------------------------------------------*/
bool ata_ctlr_wait_bsy(ata_ctlr_t *ctlr){

    int i;

    for(i=0; i<ATA_WAIT_TIMEOUT; i++){
        if(inb(ctlr->base + ATA_STATUS) & ATA_STATUS_BSY) return 1;
    }

    ATA_ADD_ERR("Timeout while waiting for BSY bit in ATA Controller status register.");
    return 0;
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_wait_drq
 *  --------------------------------------------------*/
bool ata_ctlr_wait_drq(ata_ctlr_t *ctlr){

    uint8_t status;
    int i;

    for(i=0; i<ATA_WAIT_TIMEOUT; i++){

        status  = inb(ctlr->base + ATA_STATUS);

        if(status & ATA_STATUS_BSY) continue;

        if(status & ATA_STATUS_ERR || status & ATA_STATUS_DF){
            ATA_ADD_ERR("Error bit set in ATA controller status register.");
            return 0;
        }

        if(status & ATA_STATUS_DRQ) return 1;
    }

    ATA_ADD_ERR("Timeout while waiting for DRQ bit in ATA controller status register.");
    return 0;
}


/*  ----------------------------------------------------
 *  Function:       ata_ctlr_print_status
 *
 *  Print a drives status info to screen.
 *  --------------------------------------------------*/
void ata_ctlr_print_status(ata_ctlr_t *ctlr){

    uint8_t status  = inb(ctlr->base + ATA_ALT_STATUS);

    printf("Device status: %X -", status);

    if(status & ATA_STATUS_BSY){
        printf(" Busy\n");
        return;
    }

    if(!(status & ATA_STATUS_RDY)){
        printf(" Not ready. Device needs initialisation.\n");
        return;
    }

    if(status & ATA_STATUS_DF) printf(" Device fault.");
    if(status & ATA_STATUS_DRQ) printf(" Requesting data transfer.");
    if(status & ATA_STATUS_ERR) printf(" Error.");

    printf(" Ready for commands.\n");

}



/*  ----------------------------------------------------
 *  Function:       ata_dev_print
 *
 *  Print contents of an ata_dev_t structure to the
 *  screen.
 *  --------------------------------------------------*/
void ata_dev_print(ata_dev_t * dev){
    printf("ATA DEVICE:\n");
    printf("atapi:\t%d\n", dev->atapi);
    printf("use_dma:\t%d\n", dev->use_dma);
    printf("serial:\t%s\n", dev->serial);
    printf("model:\t%s\n", dev->model);
    printf("dma_sup\t%X\n", dev->dma_sup);
    printf("dma_sel\t%X\n", dev->dma_sel);
    printf("udma_sup\t%X\n", dev->udma_sup);
    printf("udma_sel\t%X\n", dev->udma_sel);
    printf("sec_cnt_28\t%d\n", dev->sec_cnt_28);
    printf("sec_cnt_48\t%d\n", dev->sec_cnt_48);
    printf("lba48\t%d\n", dev->lba48);
    partition_t *part;
    FOREACH(part, &(dev->parts)){
        ptbl_partition_print(part);
    }
}


