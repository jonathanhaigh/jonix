/* ---------------------------------------------------
* Jonix
*
* id.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _ID_H
#define _ID_H

#include "system.h"


/*  ----------------------------------------------------
 *  Typedef struct:     idarray_t
 *  --------------------------------------------------*/
typedef struct {
    list_item_t *ids;
    id_t        size;
    id_t        last;
} idarray_t;


/*  ----------------------------------------------------
 *  Typedef:            idlist_t
 *  --------------------------------------------------*/
typedef list_head_t idlist_t;


/*
 *  Functions
 */
extern idarray_t idarray_new(id_t size);
extern bool idarray_failed(idarray_t *arr);
extern int idarray_add(idarray_t *arr, list_item_t item);
extern int idarray_add_lowest(idarray_t *arr, list_item_t item);
extern list_item_t idarray_get(idarray_t *arr, id_t id){
extern bool idarray_del(idarray_t *arr, id_t id);
extern void idarray_free(idarray_t *arr, bool free_values, bool free_head);

extern idlist_t idlist_new();
extern int idlist_add(idlist_t *list, list_item_t item);
extern int idlist_add_lowest(idlist_t *list, list_item_t item);
extern list_item_t idlist_get(idlist_t *list, id_t id);
extern bool idlist_del(idlist_t *list, id_t id);
extern void idlist_free(idlist_t *list, bool free_values, bool free_head);

#endif
