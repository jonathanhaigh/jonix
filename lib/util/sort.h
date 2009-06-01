/* ---------------------------------------------------
* Jonix
*
* sort.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _SORT_H
#define _SORT_H

/*  ----------------------------------------------------
 *  Typedef:            list_item_t
 *  --------------------------------------------------*/
typedef void * list_item_t;


/*  ----------------------------------------------------
 *  Macro:              DEFINE_CMP_BY_MEMBER
 *
 *  Defines a function to compare list_item_t's
 *  by casting the list_item_t's to a container struct
 *  pointer and numerically comparing them by some
 *  given member.
 *
 *  the direction argument should be 1 or -1. When
 *  it is 1 then lower numbers come first in a sort.
 *  When -1, higher numbers come first.
 *  --------------------------------------------------*/
#define DEFINE_CMP_MEMBER(container, member, member_type, direction)    \
    char container ## _cmp_ ## member (list_item_t a, list_item_t b){   \
        member_type ma  = ((container *)a)->member;                     \
        member_type mb  = ((container *)b)->member;                     \
        return ma == mb? 0 : ma > mb? (direction) : -(direction);       \
    }


/*  ----------------------------------------------------
 *  Typedef:            cmp_func_t
 *
 *  Comparison function to use in sort functions.
 *  --------------------------------------------------*/
typedef char (*cmp_func_t)(list_item_t, list_item_t);

#endif
