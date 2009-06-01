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
#include "util/sort.h"

/*  ----------------------------------------------------
 *  Macro:              FOREACH
 *
 *  Creates a FOREACH(var_name, list){} construct.
 *
 *  Note that list is cast to a list_head_t pointer
 *  so that this works with both ordered and unordered
 *  lists.
 *
 *  TODO: This should be much more efficient - using
 *  list_get instead of doing the work manually is bad.
 *  --------------------------------------------------*/
#define FOREACH(var_name, list) _FOREACH(var_name, list, UNIQUE_VAR()) 

#define _FOREACH(var_name, list, counter)                   \
int counter = 0;                                            \
while(                                                      \
(var_name = list_get((list_head_t *)(list), counter)) &&    \
    ++counter                                               \
)

/*  ----------------------------------------------------
 *  Typedef struct:     list_node_t
 *  --------------------------------------------------*/
struct list_node{
    list_item_t value;
    struct list_node *next;
    struct list_node *prev;
};
typedef struct list_node list_node_t;


// We give the following two structures the PACKED
// attribute so that an olist_head_t pointer can be
// cast and used as a list_head_t pointer.
//
/*  ----------------------------------------------------
 *  Typedef struct:     list_head_t
 *  --------------------------------------------------*/
typedef struct {
    list_node_t *first;
    list_node_t *last;
    uint32_t size;
} __attribute__((__packed__)) list_head_t;


/*  ----------------------------------------------------
 *  Typedef struct:     olist_head_t
 *  --------------------------------------------------*/
typedef struct {
    list_node_t *first;
    list_node_t *last;
    uint32_t    size;
    cmp_func_t  cmp;
} __attribute__((__packed__)) olist_head_t;



/*
 * Functions
 */
extern list_head_t  list_new();
extern bool         list_add(list_head_t *list, list_item_t item);
extern list_item_t  list_del(list_head_t *list, uint32_t index);
extern list_item_t  list_get(list_head_t *list, uint32_t index);
extern void         list_free(list_head_t *list, bool free_values, bool free_head);

extern olist_head_t olist_new(cmp_func_t cmp);
extern bool         olist_add(olist_head_t *list, list_item_t item);
extern list_item_t  olist_del(olist_head_t *list, uint32_t index);
extern list_item_t  olist_get(olist_head_t *list, uint32_t index);
extern void         olist_free(olist_head_t *list, bool free_values, bool free_head);

#endif
