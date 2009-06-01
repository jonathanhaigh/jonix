/* ---------------------------------------------------
* Jonix
*
* list.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "util/list.h"
#include "mem.h"


/*  ----------------------------------------------------
 *  Function:           list_new
 *  --------------------------------------------------*/
list_head_t list_new(){
    list_head_t lh;
    lh.size     = 0;
    lh.first    = NULL;
    lh.last     = NULL;
    return lh;
}


/*  ----------------------------------------------------
 *  Function:           list_add
 *  --------------------------------------------------*/
bool list_add(list_head_t *list, list_item_t item){

    list_node_t *node   = (list_node_t *) kmalloc(sizeof(list_node_t));

    if(node == NULL) return 0;

    if(list->size == 0){
        list->first = node;
        node->prev  = NULL;
    }
    else{
        list->last->next    = node;
        node->prev          = list->last;
    }
    node->next          = NULL;
    node->value         = item;
    list->last          = node;
    list->size++;

    return 1;
}


/*  ----------------------------------------------------
 *  Function:           list_set
 *  --------------------------------------------------*/
bool list_set(list_head_t *list, uint32_t index, list_item_t value){

    if(index >= list->size) return 0;

    int i;
    list_node_t *node   = list->first;
    for(i=0; i<index; i++) node = node->next;

    node->value = value;
    
    return 1;
}

/*  ----------------------------------------------------
 *  Function:           list_del
 *  --------------------------------------------------*/
list_item_t list_del(list_head_t *list, uint32_t index){

    if(index >= list->size){
        return 0;
    }

    if(index == 0){
        list_node_t *new_first  = list->first->next;
        list_item_t retval      = list->first->value;
        kfree(list->first);
        list->first             = new_first;
        list->first->prev       = NULL;
        list->size--;
        return retval;
    }

    list_node_t *node   = list->first;
    int i;
    for(i=0; i<index; i++){
        node    = node->next;
    }

    list_node_t *prev   = node->prev;
    list_node_t *next   = node->next;
    list_item_t retval  = node->value;
    kfree(node);
    prev->next          = next;
    if(next != NULL){
        next->prev  = prev;
    }
    list->size--;

    return retval;
}


/*  ----------------------------------------------------
 *  Function:           list_get
 *  --------------------------------------------------*/
list_item_t list_get(list_head_t *list, uint32_t index){

    uint32_t j;
    list_node_t *node;

    /* Negative index counts backwards from the end.
     */
    if(index < 0){
        index   += list->size;
    }

    /* Check that this index exists
     */
    if(index+1 > list->size || index < 0){
        return NULL;
    }

    if(index >= 0){
        if(index <= (list->size-1) / 2){
            for(
                j = 0, node = list->first;
                j < index;
                j++, node = node->next
            );

            return node->value;
        }
        else{
            for(
                j = list->size-1, node = list->last;
                j > index;
                j--, node = node->prev
            );
            return node->value;
        }
    }
}


/*  ----------------------------------------------------
 *  Function:           list_free
 *  --------------------------------------------------*/
void list_free(list_head_t *list, bool free_values, bool free_head){

    list_node_t *node = list->first;
    list_node_t *next;

    if(free_values){
        while(node){
            next    = node->next;
            kfree(node->value);
            kfree(node);
            node    = next;
        }
    }
    else{
        while(node){
            next    = node->next;
            kfree(node);
            node    = next;
        }
    }

    if(free_head){
        kfree(list);
    }
}


/*  ----------------------------------------------------
 *  Function:           olist_new
 *  --------------------------------------------------*/
olist_head_t olist_new(cmp_func_t cmp){
    olist_head_t lh;
    lh.size     = 0;
    lh.first    = NULL;
    lh.last     = NULL;
    lh.cmp      = cmp;
    return lh;
}


/*  ----------------------------------------------------
 *  Function:           olist_add
 *  --------------------------------------------------*/
bool olist_add(olist_head_t *list, list_item_t item){

    list_node_t *new_node   = kmalloc(sizeof(list_node_t));
    if(new_node == NULL) return 0;

    new_node->value = item;

    if(list->size == 0){
        list->first     = new_node;
        list->last      = new_node;
        new_node->next  = NULL;
        new_node->prev  = NULL;
        list->size      = 1;
        return 1;
    }

    list_node_t *node   = list->first;
    while(node != NULL && list->cmp(node->value, item) <= 0){
        node = node->next;
    }

    if(node == NULL){
        new_node->prev      = list->last;
        new_node->next      = NULL;
        list->last->next    = new_node;
        list->last          = new_node;
    }
    else{
        new_node->prev      = node->prev;
        new_node->next      = node;
        if(node->prev == NULL){
            list->first = new_node;
        }
        else{
            node->prev->next    = new_node;
        }
        node->prev          = new_node;
    }

    list->size++;

    return 1;
}


/*  ----------------------------------------------------
 *  Function:           olist_del
 *  --------------------------------------------------*/
list_item_t olist_del(olist_head_t *list, uint32_t index){
    return list_del((list_head_t *)list, index);
}


/*  ----------------------------------------------------
 *  Function:           olist_get
 *  --------------------------------------------------*/
list_item_t olist_get(olist_head_t *list, uint32_t index){
    return list_get((list_head_t *)list, index);
}


/*  ----------------------------------------------------
 *  Function:           olist_free
 *  --------------------------------------------------*/
void olist_free(olist_head_t *list, bool free_values, bool free_head){
    list_free((list_head_t *)list, free_values, free_head);
}
