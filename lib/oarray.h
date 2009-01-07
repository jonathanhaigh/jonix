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

// Data type stored in oarrays
//
typedef void * atype_t;

// Functions used to sort oarrays
//
typedef char (*sort_func_t)(atype_t, atype_t);

/*  ----------------------------------------------------
 *  Typedef struct:     oarray_t
 *  --------------------------------------------------*/
typedef struct {
    atype_t     *array;
    sort_func_t sort;
    uint32_t    max_size;
    uint32_t    size;
} oarray_t;


/*
 * Exported Functions.
 */
extern oarray_t oarray_place(void *array, sort_func_t sort, uint32_t max_size);
extern bool     oarray_insert(oarray_t *oarray, atype_t item);
extern atype_t  oarray_get(oarray_t *arr, uint32_t index);
extern void     oarray_delete(oarray_t *arr, uint32_t index);
extern bool     oarray_delete_value(oarray_t *oarray, atype_t val);
extern char     intcmp_32(atype_t a, atype_t b);

#endif
