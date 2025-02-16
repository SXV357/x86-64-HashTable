#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "./str.h"

void my_str_cpy(char * dest, char * src) {
    int i = 0;
    while ((src[i] != '\0') && (i < MAX_KEY_SIZE)) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';

    while (i < MAX_KEY_SIZE) {
        dest[i] = '\0';
        i++;
    }
}

int my_str_len(char *s) {
    char *temp = s;
    int len = 0;
    
    while (*temp++ != '\0') len++;

    return len;
}

int my_str_cmp_opt(char * s1, char * s2) {
    // both strings are 32 bytes long(padded with 0's as well)
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

        } else {
            return (c1 < c2) ? -1 : 1;
        }

    }

    return 0;
}

char * my_str_dup(char * s) {
    char * res = calloc(32, sizeof(char));
    if (!res) {
        return NULL;
    }

    my_str_cpy(res, s);
    return res;
}