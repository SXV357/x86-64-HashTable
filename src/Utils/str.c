/* Shreyas Viswanathan, str.c 
 * Last updated Feb 18, 2025
 */

#include "./str.h"

#include <stdbool.h>
#include <stdlib.h>

/* In-line implementation of strncpy that uses 32 for the len parameter. 
 * It ensures that no matter how long the string defined by the src parameter
 * is, the resulting string will always have 32 bytes, is null-terminated, and 
 * padded with extra 0's if necessary - when the string defined by the src
 * parameter is less than 32 bytes long.
 */
void my_str_cpy(char * dest, char * src) {
    int i = 0;
    while ((src[i] != '\0') && (i < MAX_KEY_SIZE)) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';

    while (i < MAX_KEY_SIZE) {
        dest[i++] = '\0';
    }
} /* my_str_cpy() */

/* In-line implementation of strlen. */
int my_str_len(char *s) {
    char *temp = s;
    int len = 0;
    
    while (*temp++ != '\0') len++;

    return len;
} /* my_str_len() */

/* In-line implementation of strcmp that uses long comparison 
 * instead of byte-by-byte comparisons. This function assumes that
 * both the strings to be compared are exactly 32 bytes long. 
 */
int my_str_cmp_opt(char * s1, char * s2) {
    char *s1_tmp = s1;
    char *s2_tmp = s2;

    int i = 1;

    while (true) {
        unsigned long c1 = *(unsigned long *)s1_tmp;
        unsigned long c2 = *(unsigned long *)s2_tmp;

        if (c1 == c2) {
            if (i++ == 4) {
                break;
            }

            s1_tmp += 8;
            s2_tmp += 8;
            continue;
        }

        return (c1 < c2) ? -1 : 1;
    }

    return 0;
} /* my_str_cmp_opt */

/* In-line implementation of strdup. */
char * my_str_dup(char * s) {
    char * res = calloc(MAX_KEY_SIZE, sizeof(char));
    if (!res) {
        return NULL;
    }

    my_str_cpy(res, s);
    return res;
} /* my_str_dup() */