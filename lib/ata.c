/* ---------------------------------------------------
* Jonix
*
* ata.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "ata.h"
#include "io.h"
#include "pci.h"
#include "sem.h"
#include "irq.h"
#include "time.h"
#include "string.h"
#include "scrn.h"
#include "mem.h"
#include "util.h"

char ata_err[ERR_STR_SIZE];

//list_head_t ata_devs;       // List of ATA devices.
//
ata_ctlr_t *ata_ctlrs[4];   // Array of pointers to ATA controllers.

/*  ----------------------------------------------------
 *  Function:       ata_init
 *  --------------------------------------------------*/
void ata_init(){

    scrn_puts("\n");

    //ata_devs    = list_new();

    ata_find_devs();

    int i;
    for(i=0; i<4; i++){
        if(ata_ctlrs[i] != NULL){
            scrn_puts("controller ");
            scrn_putn(i);
            scrn_puts("\n");
            scrn_put_var_16("\tBase", ata_ctlrs[i]->base);
            scrn_put_var_16("\tBusmaster", ata_ctlrs[i]->busmaster);
            scrn_put_var("\tDMA simplex", ata_ctlrs[i]->dma_simplex);
            int j;
            for(j=0; j<2; j++){
                if(ata_ctlrs[i]->devs[j] != NULL){
                    scrn_puts("\tposition ");
                    scrn_putn(j);
                    scrn_puts("\n");
                    scrn_put_var("\t\tDMA", ata_ctlrs[i]->devs[j]->use_dma);
                    scrn_put_var("\t\tATAPI", ata_ctlrs[i]->devs[j]->atapi);
                    scrn_put_var_str("\t\tmodel", ata_ctlrs[i]->devs[j]->model);
                    scrn_put_var_str("\t\tserial", ata_ctlrs[i]->devs[j]->serial);
                    scrn_put_var_16("\t\tDMA Sup", ata_ctlrs[i]->devs[j]->dma_sup);
                    scrn_put_var_16("\t\tDMA Sel", ata_ctlrs[i]->devs[j]->dma_sel);
                    scrn_put_var_16("\t\tUDMA Sel", ata_ctlrs[i]->devs[j]->udma_sel);
                    scrn_put_var_16("\t\tUDMA Sup", ata_ctlrs[i]->devs[j]->udma_sup);
                    scrn_put_var("\t\tLBA48", ata_ctlrs[i]->devs[j]->lba48);
                    scrn_put_var("\t\tSectors", ata_ctlrs[i]->devs[j]->sec_cnt_48);
                }
            }
        }
    }

    // Set the ATA interrupt handler
    //
    irq_set_handler(14, &ata_irq_handler);

    // Test ----------------
    //
    void *buffer    = kpalloc();

    if(!ata_dma_lba28_read(ata_ctlrs[0], 0, 0, 8, buffer)){
        scrn_puts("Read Failed:\n");
        scrn_puts(ata_err);
    }

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

        // Make Physical Region Descriptor Table for this ATA controller.
        //
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
        list_head_t prdt_tries  = list_new(); // List of pointers rejected PRDTs
        while(1){

            void *tmp   = kmalloc(3*sizeof(ata_prd_t) + 3);
            MALLOC_PANIC(tmp);

            // Make sure the PRDT is on a dword boundary.
            //
            ata_ctlrs[i]->prdt  = (ata_prd_t *) (tmp + ((uint32_t) tmp % 4));

            // Check that the PRDT doesn't cross a page (4K) boundary.
            //
            if((uint32_t) ata_ctlrs[i]->prdt % (4096) <= (4096 - 2*sizeof(ata_prd_t))){

                // This PRDT is fine, so initialise to zeroes.
                //
                ata_ctlrs[i]->prdt->buffer    = 0;
                ata_ctlrs[i]->prdt->byte_cnt  = 0;
                ata_ctlrs[i]->prdt->reserved  = 0;
                ata_ctlrs[i]->prdt->eot       = 0;

                break;    
            }

            // Add to the list of failed PRDTs.
            //
            list_add(&prdt_tries, tmp);
        }

        // Free the list of rejected PRDT pointers,
        // including the PRDTs that they point to.
        //
        list_free(&prdt_tries, 1, 0);

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
            ata_ctlrs[i]->busmaster = bar4 + 0x08*(i % 2) - 1;

            // Tell the busmaster about the physical addresss of the PRDT for this controller.
            //
            outl(ata_ctlrs[i]->busmaster + ATA_BM_PRDT_ADDR, addr_v2p(ata_ctlrs[i]->prdt));

            // TODO: Make sure this was written correctly to verify that this is actually the
            // busmaster register like we think it is.
            //

            char bm_status  = inb(ata_ctlrs[i]->busmaster + ATA_BM_STATUS);

            scrn_put_var_16("bm_status", bm_status);

            // Bit 7 indicates whether both busmaster channels (primary and secondary) can
            // be operated at the same time.
            //
            ata_ctlrs[i]->dma_simplex   = bm_status & 0x80 ? 1 : 0;

            // Bits 5 and 6 indicate whether drive 0 and drive 1 are DMA capable.
            //
            if(ata_ctlrs[i]->devs[0] != NULL){
                ata_ctlrs[i]->devs[0]->use_dma  = bm_status & 0x20 ? 1 : 0;
            }
            if(ata_ctlrs[i]->devs[1] != NULL){
                ata_ctlrs[i]->devs[1]->use_dma  = bm_status & 0x40 ? 1 : 0;
            }
        }
    }
}

/*  ----------------------------------------------------
 *  Function:       ata_ctlr_find_devs
 *
 *  Finds devices attached to an ATA controller
 *  --------------------------------------------------*/
void ata_ctlr_find_devs(ata_ctlr_t *ctlr){

    int i;
    for(i=0; i<2; i++){

        ata_select_dev(ctlr, i); // Select drive - master or slave

        sleep(1, 'm'); // TODO: Only need to wait for about 400ns.
                       //       Change this when timer is better.

        /*
         * Check that the ports for this drive work as
         * expected. We write to a couple of R/W ports and
         * then check to see if they still have the values
         * written.
         */
        outb(ctlr->base + ATA_SEC_NUM, 187 + (i % 2)*50);
        outb(ctlr->base + ATA_SEC_CNT, 59  + (i % 2)*50);

        sleep(1, 'm');

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
        
        /* We can check if this is an ATAPI device by
        *  looking at the ATA_CYL_LOW and ATA_CYL_HIGH ports.
        */
        if(
            inb(ctlr->base + ATA_CYL_LOW) == 0x14 &&
            inb(ctlr->base + ATA_CYL_HIGH) == 0xEB
        ){
            dev->atapi    = 1;
        }
        else{
            dev->atapi    = 0;

            /* Get info about drive using IDENTIFY command
             */
            outb(ctlr->base + ATA_CMD, 0xEC); // Issue IDENTIFY command

            uint8_t status  = inb(ctlr->base + ATA_STATUS);

            if(status){
                /* Wait for
                 * status bit 7 (BUSY) to clear and 
                 * status bit 3 (DRQ) sets, or
                 * stauts bit 0 (ERR) sets.
                 */
                // TODO: Make this loop timeout.
                while(1){
                    status  = inb(ctlr->base + ATA_STATUS);
                    if(
                        ((status & 128) == 0 && (status & 3) != 0)
                        || (status & 1) != 1
                    ){
                        break;
                    }
                }

                if(!(status & 1)){
                    // Read info from IDENTIFY
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
                    
                    // Get the number of 24 bit addressable sectors on the drive.
                    //
                    dev->sec_cnt_24 = *((uint32_t *) (id + 60));

                    // Get whether drive supports LBA48 mode.
                    //
                    dev->lba48  = id[83] & 0x0400 ? 1 : 0;

                    // Get the number of 48 bit addressable sectors on the drive.
                    //
                    if(dev->lba48){
                        dev->sec_cnt_48 = *((uint64_t *) (id + 100));
                    }
                    else{
                        dev->sec_cnt_48 = dev->sec_cnt_24;
                    }

                    // Extract Ultra DMA modes selected/supported.
                    //
                    // These are bit fields where bit n indicates whether Ultra DMA mode n 
                    // is supported/selected.
                    //
                    dev->udma_sup  = id[88] & 0xFF;
                    dev->udma_sel  = id[88] >> 8;

                }

            }
        }
    }
}


/*  ----------------------------------------------------
 *  Function:       ata_get_error
 *
 *  Reads and interprets error register on ATA controller.
 *  Sets error message to ata_err.
 *
 *  No locking. Caller must aqcuire lock before using.
 *  --------------------------------------------------*/
void ata_get_error(ata_ctlr_t *ctlr){

    uint8_t err    = inb(ctlr->base + ATA_ERR);

    if(err & 1){
        strcpy(ata_err, "Data address mark not found after correct ID field found.");
    }
    else if(err & 2){
        strcpy(ata_err, "Track 0 not found during execution of Recalibrate command.");
    }
    else if(err & 4){
		strcpy(ata_err, "Command aborted due to drive status error or invalid command.");
    }
    else if(err & 8){
		strcpy(ata_err, "Unkown error.");
    }
    else if(err & 16){
		strcpy(ata_err, "Requested sector's ID field not found.");
    }
    else if(err & 32){
		strcpy(ata_err, "Unkown error.");
    }
    else if(err & 64){
		strcpy(ata_err, "Uncorrectable data error encountered.");
    }
    else if(err & 128){
		strcpy(ata_err, "Bad block mark detected in the requested sector's ID field.");
    }
}

/*  ----------------------------------------------------
 *  Function:       ata_select_dev
 *
 *  Changes selected dev (master or slave) on ATA
 *  controller.
 *
 *  No locking. Caller must acquire lock before using.
 *  --------------------------------------------------*/
void ata_select_dev(ata_ctlr_t *ctlr, bool dev){

    if(ctlr->sel_dev == dev){
        // Device is already selected.
        //
        return;
    }

    if(dev == 0){
        // Master
        //
        outb(ctlr->base + ATA_DRIVE_SELECT, 0xA0);
        ctlr->sel_dev   = 0;
        return;
    }
    if(dev == 1){
        // Slave
        //
        outb(ctlr->base + ATA_DRIVE_SELECT, 0xB0);
        ctlr->sel_dev   = 1;
    }
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
bool ata_dma_lba28_read(ata_ctlr_t *ctlr, bool dev, uint32_t lba, uint8_t sec_cnt, void *buffer){

    // Check drive supports DMA
    //
//    if(!ctlr->devs[(int) dev]->use_dma){
//        strcpy(ata_err, "Drive does not support DMA.");
//        return 0;
//    }

    // Check that the required sectors are addressable.
    //
    if(lba + sec_cnt > ctlr->devs[(int) dev]->sec_cnt_24){
        strcpy(ata_err, "Sectors to be read are not LBA24 addressable.");
        return 0;
    }

    // Obtain locks on controller and device.
    //
    sem_wait(&(ctlr->devs[(int) dev]->sem)); // Must obtain device lock first!
    sem_wait(&(ctlr->sem));

    ata_select_dev(ctlr, dev);

    // Setup the PRDT for this read.
    //
    // We wil use two entries in the PRDT because each PRDT
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
    outb(ATA_CMD, 0xC8);                // Issue the LBA24 DMA read command

    // Start the busmaster by writing to the start bit in the command register.
    //
    outb(ATA_BM_CMD, 0x01);

    // Wait for an interrupt!
    //
    //sem_wait(ctlr->devs[dev]->
    sleep(100, 's');

    return 1;
}

/*  ----------------------------------------------------
 *  Function:       ata_irq_handler
 *  --------------------------------------------------*/
void ata_irq_handler(isr_stack_t *r){
    scrn_puts("ATA Interupt!!!\n");
}

/*  ----------------------------------------------------
 *  Function:       ata_dev_t_print
 *
 *  Print contents of an ata_dev_t structure to the
 *  screen.
 *  --------------------------------------------------*/
//void ata_dev_t_print(ata_dev_t * dev){
//    scrn_puts("ATA DEVICE:\n");
//    scrn_put_var("Controller", dev->ctrl);
//    scrn_put_var("ATAPI?\t", dev->atapi);
//    scrn_put_var("Position", dev->pos);
//    scrn_put_var("Identify Info", (long) dev->id);
//    scrn_puts("Model\t\t= \"");
//    scrn_puts(dev->model);
//    scrn_puts("\"");
//    scrn_puts("\n");
//}
//

