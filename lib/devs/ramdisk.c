/* ---------------------------------------------------
* Jonix
*
* ramdisk.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "devs/ramdisk.h"
#include "devs/hal.h"
#include "mem.h"

char ramdisk_err[ERR_STR_SIZE];


/*  ----------------------------------------------------
 *  Function:       ramdisk_new
 *  --------------------------------------------------*/
block_dev_t *ramdisk_new(uint32_t size, const char *name){

    void *space = kmalloc(size);    
    if(space == NULL){
        ADD_ERR(ramdisk, "Not enough memory to create new ramdisk.\n");
        return NULL;
    }

    block_dev_t *dev    = kmalloc(sizeof(block_dev_t));
    if(dev == NULL){
        ADD_ERR(ramdisk, "Not enough memory to create new ramdisk.\n");
        return NULL;
    }

    ramdisk_dev_t *ramdisk  = kmalloc(sizeof(ramdisk_dev_t));
    if(ramdisk == NULL){
        ADD_ERR(ramdisk, "Not enough memory to create new ramdisk.\n");
        return NULL;
    }

    ramdisk->start  = space;
    ramdisk->size   = size;
    dev->extra      = ramdisk;
    dev->open       = ramdisk_open;
    dev->close      = ramdisk_close;
    dev->read       = ramdisk_read;
    dev->write      = ramdisk_write;
    dev->id         = idlist_add(&hal_ids, dev);
    strcpy(dev->name, name, HAL_DEV_NAME_LEN - 1);

    return dev;
}


/* ----------------------------------------------------
 *  Function:       ramdisk_open
 * --------------------------------------------------*/
int ramdisk_open(block_dev_t *dev){ 
    return dev->id;
}


/*  ----------------------------------------------------
 *  Function:       ramdisk_close
 *  --------------------------------------------------*/
bool ramdisk_close(id_t id){
    return 1;
}


/*  ----------------------------------------------------
 *  Function:       ramdisk_read
 *  --------------------------------------------------*/
bool ramdisk_read(id_t id, void *buffer, size_t bytes, off_t offset){

    ramdisk_dev_t *dev  = ((block_dev_t *)idlist_get(&hal_ids, id))->extra;

    if(offset + bytes > dev->size) return 0;

    uint8_t *start   = dev->start + offset;
    int  i;

    for(i=0; i<bytes; i++) ((uint8_t *)buffer)[i]   = start[i];

    return 1;
}


/*  ----------------------------------------------------
 *  Function:       ramdisk_write
 *  --------------------------------------------------*/
bool ramdisk_write(id_t id, void *buffer, size_t bytes, off_t offset){

    ramdisk_dev_t *dev  = ((block_dev_t *)idlist_get(&hal_ids, id))->extra;

    if(offset + bytes > dev->size) return 0;

    char *start   = dev->start + offset;
    int  i;

    for(i=0; i<bytes; i++) start[i] = ((uint8_t *)buffer)[i];

    return 1;
}
