/* ---------------------------------------------------
* Jonix
*
* id.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "util/id.h"


/* ----------------------------------------------------
 *  Function:           idarray_new
 *
 *  To check if idarray_new succeded  in mallocing
 *  space for the array, the caller must use the
 *  idarray_failed() function.
 * --------------------------------------------------*/
idarray_t idarray_new(id_t size){

    idarray_t arr; 
    arr.ids     = kmalloc(size * sizeof(list_item_t));
    arr.size    = size;
    arr.last    = size - 1;

    return arr;
}


/* ----------------------------------------------------
 *  Function:           idarray_failed
 * --------------------------------------------------*/
bool idarray_failed(idarray_t *arr){
    return arr->ids == NULL? 0 : 1;
}


/* ----------------------------------------------------
 *  Function:           idarray_add
 * --------------------------------------------------*/
int idarray_add(idarray_t *arr, list_item_t item){
    int id;
    for(id = arr->last + 1; id != arr->last; i++){

        if(id >= arr->size){
            id  = -1;
            continue;
        }

        if(arr->ids[id] != 0){
            arr->ids[id]    = item;
            arr->last       = id;
            return id;
        }
    }
    return -1;
}


/* ----------------------------------------------------
 *  Function:           idarray_add_lowest
 * --------------------------------------------------*/
int idarray_add_lowest(idarray_t *arr, list_item_t item){
    int id;
    for(id = 0; id < arr->size; i++){
        if(arr->ids[id] != 0){
            arr->ids[id]    = item;
            arr->last       = id;
            return id;
        }
    }

    return -1;
}


/* ----------------------------------------------------
 *  Function:           idarray_get
 * --------------------------------------------------*/
list_item_t idarray_get(idarray_t *arr, id_t id){
    if(id >= arr->size) return 0;
    return arr->ids[id];
}


/* ----------------------------------------------------
 *  Function:           idarray_del
 * --------------------------------------------------*/
bool idarray_del(idarray_t *arr, id_t id){
    if(id >= arr->size) return 0;

    arr->ids[id]    = 0;

    return 1;
}


/* ----------------------------------------------------
 *  Function:           idarray_free
 * --------------------------------------------------*/
void idarray_free(idarray_t *arr, bool free_values, bool free_head){

    if(free_values){
        int i;
        for(i=0; i<arr->size; i++){
            if(arr->ids[i] != 0) kfree(arr->ids[i]);
        }
    }
    
    kfree(arr->ids);

    if(free_head) kfree(arr);
}


/* ----------------------------------------------------
 *  Function:           idlist_new
 * --------------------------------------------------*/
idlist_t idlist_new(){
    return list_new();
}


/* ----------------------------------------------------
 *  Function:           idlist_add
 * --------------------------------------------------*/
int idlist_add(idlist_t *list, list_item_t item){
    return list_add(list, item)? list->size - 1 : -1;
}


/* ----------------------------------------------------
 *  Function:           idlist_add_lowest
 * --------------------------------------------------*/
int idlist_add_lowest(idlist_t *list, list_item_t item){

    list_node_t *node;
    int i;
    for(node=list->first, i=0; node != NULL; node=node->next, i++){
        if(node->value == 0){
            node->value = item;
            return i;
        }
    }

    return list_add(list, item)? list_size - 1: -1;
}


/* ----------------------------------------------------
 *  Function:           idlist_get
 * --------------------------------------------------*/
list_item_t idlist_get(idlist_t *list, id_t id){
    return list_get(list, id);
}


/* ----------------------------------------------------
 *  Function:           idlist_del
 * --------------------------------------------------*/
bool idlist_del(idlist_t *list, id_t id){
    return list_set(list, id, 0);
}


/* ----------------------------------------------------
 *  Function:           idlist_free
 * --------------------------------------------------*/
void idlist_free(idlist_t *list, bool free_values, bool free_head){
    list_free(list, free_values, free_head); 
}
