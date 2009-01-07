/* ---------------------------------------------------
* Jonix - Utilities
*
* string.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "util.h"

char digit_str[]    = "0123456789abcdef";

/*  ----------------------------------------------------
 *  Macro:       define_num2str
 *
 *  Description:        
 *
 * This macro is to make functions called something like char2str_10, and int2str_16
 * that convert a char or int into a string representation base 10, and base 16 respectively.
 * 
 * A macro is used rather than a function because the length of the digits array depends
 * on the base.
 *
 * The parameter max_strlen_pc is the max number of characters required to represent a
 * char in this base. It does depend entirely on the base, but the C preprocessor can't
 * do the maths.
 *
 *  --------------------------------------------------*/
#define define_num2str(base, num_type, max_strlen_pc)                       \
void num_type ## 2str_ ## base (char *dest, num_type num){                  \
                                                                            \
    int i;                                                                  \
    bool print_flag;                                                        \
    int digits[max_strlen_pc * sizeof(num_type)];                           \
                                                                            \
   /* if(num < 0){                                                          \
        *dest   = '-';                                                      \
        dest++;                                                             \
        num = -num;                                                         \
    } */                                                                    \
                                                                            \
    for( i=0 ; i < sizeof(num_type) * max_strlen_pc ; i++){                 \
        digits[i]   = num % base;                                           \
        num         /= base;                                                \
    }                                                                       \
    for( i--, print_flag=0 ; i>=0 ; i-- ){                                  \
        if(digits[i]){                                                      \
            print_flag  = 1;                                                \
        }                                                                   \
        if(print_flag){                                                     \
            *dest   = digit_str[digits[i]];                                 \
            dest++;                                                         \
        }                                                                   \
    }                                                                       \
                                                                            \
    if(!print_flag){                                                        \
        *dest   = '0';                                                      \
        dest++;                                                             \
    }                                                                       \
                                                                            \
    *dest   = 0;                                                            \
}



define_num2str(10, char, 3)
define_num2str(10, int,  3)
define_num2str(10, long, 20)
define_num2str(16, char, 2)
define_num2str(16, int,  2)
define_num2str(16, long, 20)

/*  ----------------------------------------------------
 *  Function:       long2str_10h
 *  --------------------------------------------------*/
void long2str_10h(char *dest, unsigned long num, unsigned char precision){
    /*
     * TODO: Add a max length argument
     */

    char need_plus  = 0;

    if(precision && num >= 1073741824){ 
        /* 1GB */

        int2str_10(dest, num/1073741824);
        num         %= 1073741824;

        while(*dest != 0){
            dest++;
        }
        *(dest++)   = 'G';

        precision--;
        need_plus   = 1;
    }

    if(precision && num >= 1048576){ 

        if(need_plus){
            *(dest++)   = '+';
        }

        int2str_10(dest, num/1048576);
        num         %= 1048576;

        while(*dest != 0){
            dest++;
        }
        *(dest++)   = 'M';

        precision--;
        need_plus   = 1;
    }
    
    if(precision && num >= 1024){
        /* 1kB */
        if(need_plus){
            *(dest++)   = '+';
        }

        int2str_10(dest, num/1024);
        num         %= 1024;

        while(*dest != 0){
            dest++;
        }
        *dest       = 'k';
        *(++dest)   = 0;

        precision--;
        need_plus   = 1;
    }

    if(precision && num >= 0){
        /* 1kB */
        if(need_plus){
            *(dest++)   = '+';
        }

        int2str_10(dest, num);
        need_plus   = 1;
    }

    if(!need_plus){
        *(dest++)   = '0';
        *dest       = 0;
    }
}

/*  ----------------------------------------------------
 *  Function:       join
 *  --------------------------------------------------*/
void join(char *dest, char **str_arr, unsigned int count, char delim){

    unsigned int i;
    unsigned int j;

    for( i=0; i<count; i++){
        for( j=0; str_arr[i][j] != 0; j++){
            *(dest++)   = str_arr[i][j];
        }
        *(dest++)   = delim;
    }

    *(dest-1)   = 0;
}

/*  ----------------------------------------------------
 *  Function:       uc
 *  --------------------------------------------------*/
char uc(const char c){
    return c + 0x20;
}
