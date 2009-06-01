/* ---------------------------------------------------
* Jonix
*
* hal.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _HAL_H
#define _HAL_H

#include "system.h"
#include "util/id.h"

#define HAL_DEV_NAME_LEN 20

struct block_dev; // Forward declaration to avoid circular referencing problems.

typedef int  (*block_dev_open_func_t)(struct block_dev *);
typedef bool (*block_dev_read_func_t)(id_t, void *, size_t, off_t);
typedef bool (*block_dev_write_func_t)(id_t, void *, size_t, off_t);
typedef bool (*block_dev_close_func_t)(id_t);


/*  ----------------------------------------------------
 *  Typedef struct:     block_dev_t
 *  --------------------------------------------------*/
struct block_dev{
    block_dev_open_func_t   open;
    block_dev_close_func_t  close;
    block_dev_read_func_t   read;
    block_dev_write_func_t  write;
    id_t                    id;
    char                    name[HAL_DEV_NAME_LEN];
    void                    *extra; // this will prob point to a more device specific structure
                                    // like a ramdisk_dev_t or an ata_dev_t.
};
typedef struct block_dev block_dev_t;


/*
 *  Global Variables
 */
extern list_head_t hal_block_devs;
extern list_head_t hal_char_devs;
extern idlist_t    hal_ids;

#endif
