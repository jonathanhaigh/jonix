/* ---------------------------------------------------
* Jonix
*
* list.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _LIST_H
#define _LIST_H

#include "system.h"

// macro to add foreach construct
//
#define foreach(var_name, list_ptr_name)                                \
int xxx_foreach_counter = 0;                                            \
while(                                                                  \
(var_name = list_get(list_ptr_name, xxx_foreach_counter)) &&            \
    ++xxx_foreach_counter                                               \
)

typedef void * list_item_t; // Type of items stored in a list.

/*  ----------------------------------------------------
 *  Typedef struct:     list_node_t
 *  --------------------------------------------------*/
struct list_node{
    list_item_t value;
    struct list_node *next;
    struct list_node *prev;
};
typedef struct list_node list_node_t;

/*  ----------------------------------------------------
 *  Typedef struct:     list_head_t
 *  --------------------------------------------------*/
typedef struct {
    list_node_t *first;
    list_node_t *last;
    uint32_t size;
} list_head_t;

extern list_head_t list_new();
extern bool list_add(list_head_t *list, list_item_t item);
extern list_item_t list_del(list_head_t *list, uint32_t index);
extern list_item_t list_get(list_head_t *list, uint32_t index);
extern void list_free(list_head_t *list, bool free_values, bool free_head);

#endif
