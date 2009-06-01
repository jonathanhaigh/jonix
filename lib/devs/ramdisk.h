/* ---------------------------------------------------
* Jonix
*
* ramdisk.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _RAMDISK_H
#define _RAMDISK_H

#include "system.h"
#include "devs/hal.h"

extern char ramdisk_err[ERR_STR_SIZE];

/* ----------------------------------------------------
 *  Typedef struct:     ramdisk_dev_t
 * --------------------------------------------------*/
typedef struct {
    void        *start;
    uint32_t    size;
} ramdisk_dev_t;

/*
 *  Functions
 */
extern block_dev_t *ramdisk_new(uint32_t size);
extern int ramdisk_open(block_dev_t *dev);
extern bool ramdisk_close(id_t id);
extern bool ramdisk_read(id_t id, void *buffer, size_t bytes, off_t offset);
extern bool ramdisk_write(id_t id, void *buffer, size_t bytes, off_t offset);


#endif
