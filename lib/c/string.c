/* ---------------------------------------------------
* Jonix - C Standard Library
*
* string.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "c/string.h"
#include "c/stdint.h"

/*  ----------------------------------------------------
 *  Function:       memcpy
 *  --------------------------------------------------*/
void * memcpy(void *dest, const void *src, size_t count){

    char * dest_c   = (char *) dest;
    char * src_c    = (char *) src;

    size_t i;

    for( i=0 ; i<count ; i++){
        *(dest_c + i)   = *(src_c + i);
    }

    return dest;
}


/*  ----------------------------------------------------
 *  Function:       memmove
 *  --------------------------------------------------*/
void * memmove(void *dest, const void *src, size_t count){

    char * dest_c   = (char *) dest;
    char * src_c    = (char *) src;

    size_t i;

    /* We have to make sure that memory is not corrupted if objects overlap
     */
    if(src > dest){
        for( i=0 ; i<count ; i++){
            *(dest_c + i)   = *(src_c + i);
        }
    }
    else{
        for( i=count-1 ; i>=0 ; i--){
            *(dest_c + i)   = *(src_c + i);
        }
    }

    return dest;
}


/*  ----------------------------------------------------
 *  Function:       memset
 *  --------------------------------------------------*/
void * memset(void *dest, int val, size_t count){

    char *dest_c    = (char *) dest; 
    char val_c      = (char) val; 
    size_t i;

    for( i=0 ; i<count ; i++ ){
        *(dest_c + i) = val_c;
    }

    return dest;
}


/*  ----------------------------------------------------
 *  Function:       memset_w
 *  --------------------------------------------------*/
void * memset_w(void *dest, int val, size_t count){

    uint16_t *dest_w    = (uint16_t *) dest; 
    uint16_t val_w      = (uint16_t) val; 
    size_t i;

    for( i=0 ; i<count ; i++ ){
        *(dest_w + i) = val_w;
    }

    return dest;
}


/*  ----------------------------------------------------
 *  Function:       strlen
 *  --------------------------------------------------*/
size_t strlen(const char *str){

    size_t len  = 0;

    while(*str != '\0'){
        len++;
        str++;
    }

    return len;
}


/*  ----------------------------------------------------
 *  Function:       strcpy
 *  --------------------------------------------------*/
char *strcpy(char *dest, const char *src){

    int i;

    for(i=0; *(src+i) != '\0'; i++){
        *(dest+i) = *(src+i);
    }

    *(dest+i)   = '\0';

    return dest;
}


/*  ----------------------------------------------------
 *  Function:       strcpy_n
 *  --------------------------------------------------*/
char *strcpy_n(char *dest, const char *src, size_t max_len){

    int i;

    for(i=0; i < max_len-1 && *(src+i) != '\0'; i++){
        *(dest+i) = *(src+i);
    }

    *(dest+i)   = '\0';

    return dest;
}


/*  ----------------------------------------------------
 *  Function:       strcat
 *  --------------------------------------------------*/
char *strcat(char *dest, const char *src){
    return strcpy(dest + strlen(dest), src);
}


/*  ----------------------------------------------------
 *  Function:       strcmp
 *  --------------------------------------------------*/
int strcmp(const char *s1, const char *s2){

    int i;
    for(i=0; 1; i++){

        if(s1[i] == '\0' && s2[i] == '\0') return 0;

        if((unsigned char) s1[i] != (unsigned char) s2[i]){
            return (unsigned char) s1[i] - (unsigned char) s2[i];
        }
    }
}
