/* ---------------------------------------------------
* Jonix - C Standard Library
*
* printf.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "c/stdio.h"
#include "c/stdarg.h"
#include "c/stddef.h"
#include "scrn.h"

char digit_str_l[]    = "0123456789abcdef";
char digit_str_u[]    = "0123456789ABCDEF";

char printf_buffer[PRINTF_BUFFER_SIZE];


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
#define DEFINE_NUM2STR(base, num_type, max_strlen_pc, str_case)             \
void num_type ## 2str_ ## base ## str_case (char *dest, num_type num){   \
                                                                            \
    int i;                                                                  \
    bool print_flag;                                                        \
    int digits[max_strlen_pc * sizeof(num_type)];                           \
                                                                            \
   if(num < 0){                                                          \
        *dest   = '-';                                                      \
        dest++;                                                             \
        num = -num;                                                         \
    }                                                                   \
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
            *dest   = digit_str_## str_case [digits[i]];                    \
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



DEFINE_NUM2STR(10, char, 3, l)
DEFINE_NUM2STR(10, int,  3, l)
DEFINE_NUM2STR(10, long, 20, l)
DEFINE_NUM2STR(16, char, 2, l)
DEFINE_NUM2STR(16, char, 2, u)
DEFINE_NUM2STR(16, int,  2, l)
DEFINE_NUM2STR(16, int,  2, u)
DEFINE_NUM2STR(16, long, 20, l)
DEFINE_NUM2STR(16, long, 20, u)

#define SPRINT_C(c)             \
    tmp_char    = (c);          \
    if(buffer == NULL){         \
        scrn_putc(tmp_char);    \
    }                           \
    else if(retval < size){     \
        *(buffer++) = tmp_char; \
    }                           \
    retval++;

#define SPRINT_S(str)               \
    tmp_ptr_m = (str);              \
    while(*tmp_ptr_m){              \
        SPRINT_C(*(tmp_ptr_m++));   \
    }

#define SPRINT_N(type, base, str_case)                              \
    tmp_ ## type = va_arg(ap, type);                                \
    type ## 2str_ ## base ## str_case (tmp_str, tmp_ ## type);      \
    SPRINT_S(tmp_str);                                              \

static int _printf_helper(char *buffer, size_t size, const char *format, va_list ap){

    int retval          = 0;
    int tmp_int         = 0;
    char tmp_str[25]    = "";
    char *tmp_ptr_m     = NULL;
    char tmp_char       = 0;
    char *tmp_ptr       = 0;
    
    while(*format){

        if(*format != '%'){
            SPRINT_C(*(format++));
            continue;
        }

        if(*(++format) == '%'){
            SPRINT_C('%');
            continue;
        }

        switch(*(format++)){
            case 'i':
            case 'd': SPRINT_N(int, 10, l); break;

            case 'x': SPRINT_N(int, 16, l); break;

            case 'X': SPRINT_N(int, 16, u); break;

            case 's':
                tmp_ptr = va_arg(ap, char *);
                SPRINT_S(tmp_ptr);
                break;

        }

    }

    return retval;
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list ap){
    if(buffer == NULL){
        PANIC("Tried to print to NULL buffer.");
    }
    return _printf_helper(buffer, size, format, ap);
}

int snprintf(char *buffer, size_t size, const char *format, ...){
  va_list ap;
  int retval = 0;
  
  va_start(ap, format);
  retval = vsnprintf(buffer, size, format, ap);
  va_end(ap);

  return (retval);
}

int vprintf(const char *format, va_list ap){
    return _printf_helper(NULL, 0, format, ap);
}

int printf(const char *format, ...){
    va_list ap;
    int retval  = 0;
    va_start(ap, format);
    retval  = vprintf(format, ap);
    va_end(ap);

    return (retval);
}
