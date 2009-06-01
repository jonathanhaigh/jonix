/* ---------------------------------------------------
* Jonix
*
* oarray.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "util/oarray.h"
#include "util/sort.h"

/*  ----------------------------------------------------
 *  Function:       oarray_place
 *
 *                  Creates a new ordered array, with
 *                  array at a given address.
 *  --------------------------------------------------*/
oarray_t oarray_place(void *array, cmp_func_t cmp, uint32_t max_size){

    oarray_t oarray;
    oarray.array    = (list_item_t) array;
    oarray.cmp      = cmp;
    oarray.max_size = max_size;
    oarray.size     = 0;

    return oarray;
}

/*  ----------------------------------------------------
 *  Function:       oarray_add
 *  --------------------------------------------------*/
bool oarray_add(oarray_t *oarray, list_item_t item){

    if(oarray->size >= oarray->max_size){
        return 0;
    }

    /*
     * TODO: Improve the following algorithm. Maybe use binary search or something similar.
     */
    uint32_t i  = 0;
    while( i<oarray->size && oarray->cmp(oarray->array[i], item) <= 0 ){
        i++;
    }

    /* i is now the index of the element that we are going to replace */

    if(i == oarray->size){
        /* We just need to add the item to the end of the array */
        oarray->array[oarray->size++]   = item;
        return 1;
    }

    /* We need to shift everything in the array with index >= i
     * along one place, and then put the new element int the place
     * with index i.
     */
    int j;
    for(j = oarray->size++; j > i; j--){ 
        oarray->array[j]    = oarray->array[j-1];
    }
    oarray->array[i]    =  item;

    return 1;
}


/*  ----------------------------------------------------
 *  Function:       oarray_get
 *  --------------------------------------------------*/
list_item_t oarray_get(oarray_t *arr, uint32_t index){
    return arr->array[index];
}

/*  ----------------------------------------------------
 *  Function:       oarray_find_value
 *  --------------------------------------------------*/
int oarray_find_value(oarray_t *oarray, list_item_t val){

    /* TODO: improve this algorithm! */

    int i;
    for(i=0; i<oarray->size; i++){
        if(oarray->array[i] == val){
            return i;
        }
    }

    return -1;
}


/*  ----------------------------------------------------
 *  Function:       oarray_del
 *  --------------------------------------------------*/
void oarray_del(oarray_t *oarray, uint32_t index){

    if(index == oarray->size - 1){
        oarray->size--;
        return;
    }

    int j;
    for(j = index; j < oarray->size - 1; j++){
        oarray->array[j]    = oarray->array[j+1];
    }

    oarray->size--;
    return;
}

/*  ----------------------------------------------------
 *  Function:       oarray_del_value
 *  --------------------------------------------------*/
bool oarray_del_value(oarray_t *oarray, list_item_t val){
    int index = oarray_find_value(oarray, val);

    if(index >= 0){
        oarray_del(oarray, index);
        return 1;
    }

    return 0;
}

