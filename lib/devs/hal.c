/* ---------------------------------------------------
* Jonix
*
* hal.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "hal.h"

list_head_t hal_block_devs;
list_head_t hal_char_devs;
idlist_t    hal_ids;


/* ----------------------------------------------------
 *  Function:       hal_init
 * --------------------------------------------------*/
void hal_init(){
    hal_block_devs  = list_new();
    hal_char_devs   = list_new();
    hal_ids         = idlist_new();
}
