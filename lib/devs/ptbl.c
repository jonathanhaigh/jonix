/* ---------------------------------------------------
* Jonix
*
* ptbl.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
*
* Partition table library.
* --------------------------------------------------*/

#include "system.h"
#include "devs/ptbl.h"
#include "util/list.h"
#include "mem.h"

char ptbl_err[ERR_STR_SIZE];


/*  ----------------------------------------------------
 *  Function:       ptbl_parse
 *  --------------------------------------------------*/
bool ptbl_parse(void *ptbl, list_head_t *list){

    partition_t *tmp;

    int i;
    for(i=0; i<4; i++){
        tmp = ptbl_entry_parse((uint8_t *) (ptbl+16*i));
        if(tmp != NULL && !list_add(list, tmp)){
            PTBL_ADD_ERR("Failed to add to list of partitions.");
            return 0;
        }
    }

    return 1;
}


/*  ----------------------------------------------------
 *  Function:       ptbl_entry_parse
 *  --------------------------------------------------*/
partition_t * ptbl_entry_parse(uint8_t *ptr){

    partition_t *pte   = (partition_t *) kmalloc(sizeof(partition_t));
    MALLOC_PANIC(pte);
    
    // first byte is the boot indicator.
    // 0x80=active, 0x00=not active, other values invalid.
    //
    switch(*ptr){
        case 0x80:
            pte->active = 1;
            break;
        case 0x00:
            pte->active = 0;
            break;
        default:
            kfree(pte);
            return NULL;
    }
    
    // byte 4 is the fileystem type id
    //
    pte->fs_id  = *(ptr + 4);

    if(pte->fs_id == 0){
        kfree(pte);
        return NULL;
    }

    // bytes 8-11 are the starting lba of the partition in
    // little endian format.
    //
    pte->start  =      *(ptr + 8)
                    + (*(ptr + 9) << 8)
                    + (*(ptr + 10) << 16)
                    + (*(ptr + 11) << 24);

    // bytes 12-15 are the length in sectors of the partition in
    // little endian format.
    //
    pte->size   =      *(ptr + 12)
                    + (*(ptr + 13) << 8)
                    + (*(ptr + 14) << 16)
                    + (*(ptr + 15) << 24);

    return pte;
}


/*  ----------------------------------------------------
 *  Function:       ptbl_partition_print
 *  --------------------------------------------------*/
void ptbl_partition_print(partition_t *part){
    printf("Disk Partition:\n");
    printf("Active:\t%d\n", part->active);
    printf("Start:\t%d\n", part->start);
    printf("Size:\t%d\n", part->size);
    printf("FS ID:\t%X\n", part->fs_id);
}
