/* ---------------------------------------------------
* Jonix
*
* ptbl.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
*
* Partition table library header file.
* --------------------------------------------------*/

#ifndef _PTBL_H
#define _PTBL_H

#include "system.h"
#include "util/list.h"
#include "c/string.h"
#include "filesystems/vfs.h"

#define PTBL_ADD_ERR(str) (strcat(ptbl_err, (str)), strcat(ptbl_err, "\n"))
#define PTBL_PRINT_ERR() (printf("PTBL Error String:\n%s\n", ptbl_err), *ptbl_err=NULL)

extern char ptbl_err[ERR_STR_SIZE];


/*  ----------------------------------------------------
 *  Typedef struct:     partition_t
 *  --------------------------------------------------*/
typedef struct {
    bool            active;
    uint32_t        start;
    uint32_t        size;
    uint8_t         fs_id;
} partition_t;

extern bool ptbl_parse(void *ptbl, list_head_t *list);
extern partition_t * ptbl_entry_parse(uint8_t *ptr);
extern void ptbl_partition_print(partition_t *part);

#endif
