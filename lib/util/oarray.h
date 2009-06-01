/* ---------------------------------------------------
* Jonix
*
* oarray.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _OARRAY_H
#define _OARRAY_H

#include "system.h"
#include "util/sort.h"


/*  ----------------------------------------------------
 *  Typedef struct:     oarray_t
 *  --------------------------------------------------*/
typedef struct {
    list_item_t *array;
    cmp_func_t  cmp;
    uint32_t    max_size;
    uint32_t    size;
} oarray_t;


/*
 * Exported Functions.
 */
extern oarray_t     oarray_place(void *array, cmp_func_t cmp, uint32_t max_size);
extern bool         oarray_add(oarray_t *oarray, list_item_t item);
extern list_item_t  oarray_get(oarray_t *arr, uint32_t index);
extern void         oarray_del(oarray_t *arr, uint32_t index);
extern bool         oarray_del_value(oarray_t *oarray, list_item_t val);

#endif
