/* ---------------------------------------------------
* Jonix
*
* oarray.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "oarray.h"

/*  ----------------------------------------------------
 *  Function:       oarray_place
 *
 *                  Creates a new ordered array, with
 *                  array at a given address.
 *  --------------------------------------------------*/
oarray_t oarray_place(void *array, sort_func_t sort, uint32_t max_size){

    oarray_t oarray;
    oarray.array    = (atype_t) array;
    oarray.sort     = sort;
    oarray.max_size = max_size;
    oarray.size     = 0;

    return oarray;
}

/*  ----------------------------------------------------
 *  Function:       oarray_insert
 *  --------------------------------------------------*/
bool oarray_insert(oarray_t *oarray, atype_t item){

    if(oarray->size >= oarray->max_size){
        return 0;
    }

    /*
     * TODO: Improve the following algorithm. Maybe use binary search or something similar.
     */
    uint32_t i  = 0;
    while( i<oarray->size && oarray->sort(oarray->array[i], item) <= 0 ){
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
atype_t oarray_get(oarray_t *arr, uint32_t index){
    return arr->array[index];
}

/*  ----------------------------------------------------
 *  Function:       oarray_find_value
 *  --------------------------------------------------*/
int oarray_find_value(oarray_t *oarray, atype_t val){

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
 *  Function:       oarray_delete
 *  --------------------------------------------------*/
void oarray_delete(oarray_t *oarray, uint32_t index){

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
 *  Function:       oarray_delete_value
 *  --------------------------------------------------*/
bool oarray_delete_value(oarray_t *oarray, atype_t val){
    int index = oarray_find_value(oarray, val);

    if(index >= 0){
        oarray_delete(oarray, index);
        return 1;
    }

    return 0;
}

/*  ----------------------------------------------------
 *  Function:       intcmp_32
 *  --------------------------------------------------*/
char intcmp_32(atype_t a, atype_t b){
     if((uint32_t) a == (uint32_t) b){
         return 0;
     }
     else if((uint32_t) a > (uint32_t) b){
         return 1;
     }
     return -1;
}
