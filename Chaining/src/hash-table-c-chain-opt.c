#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "hash-table-c-chain.h"

#define MAX_KEY_SIZE (32) // each key(char *) will be 32 bytes long

// forward declarations for the custom string funcs
void my_str_cpy(char *, char *);
int my_str_len(char *);
int my_str_cmp_opt(char *, char *);
int my_str_cmp_opt(char *, char *); // optimal version that uses long comparisons instead of byte operations
char * my_str_dup(char *);

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

int my_str_cmp_old(char * s1, char * s2) {
    char *s1_tmp = s1;
    char *s2_tmp = s2;
    
    while ((*s1_tmp != '\0') && (*s2_tmp != '\0')) {
        if (*s1_tmp < *s2_tmp) {
            return -1;
        } else if (*s1_tmp > *s2_tmp) {
            return 1;
        }

        s1_tmp++;
        s2_tmp++;
    }
    
    if ((*s1_tmp == '\0') && (*s2_tmp == '\0')) {
        return 0;
    }

    return *s1 == '\0' ? -1 : 1;
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

Table * init(long maxWords) {
    long bucketSizes[] = { 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 };
    long nBucketSizes = sizeof(bucketSizes)/sizeof(long);

    if ((maxWords <= 0) || (maxWords > bucketSizes[nBucketSizes - 1])) {
        return NULL;
    }

    Table * table = (Table *) malloc(sizeof(Table));
    if (table == NULL) {
        perror("(init) error allocating memory for the table");
        return NULL;
    }

    table->nBuckets = bucketSizes[nBucketSizes - 1];

    for (int i = 0; i < nBucketSizes; i++) {
       if (maxWords < bucketSizes[i]) {
            table->nBuckets =  bucketSizes[i];
            break;
       }
    }

    table->maxWords = maxWords;
    table->nWords = 0;

    table->array = (Node **)
        malloc(table->nBuckets * sizeof(Node *));

    if (table->array == NULL) {
       perror("(init) error allocating memory for table->array");
       return NULL;
    }

    for (int i = 0; i < table->nBuckets; i++) {
       table->array[i] = NULL;
    }

    return table;
}

long hash(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return -1;
    }
    
    long hashNum = 1;
    long len = my_str_len(word);

    for (long i = 0; i < len; i++) {
        hashNum = ((hashNum << 5) - hashNum) + word[i];
    }

    return hashNum & (table->nBuckets - 1);
}

/**
 * Looks up the key specified by the word parameter in the hash table and return true if it exists
 */
bool lookup(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return false;
    }

    long hashNum = hash(table, word);
    Node * elem = table->array[hashNum];

    while ((elem != NULL) && (!my_str_cmp_opt(elem->word,word))) {
      elem = elem->next;
    }

    return elem == NULL ? false : true;
}

long get(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return -1;
    }

    long targetIdx = hash(table, word);
    Node * head = table->array[targetIdx];

    while (head) {
        if (!my_str_cmp_opt(head->word, word)) {
            return head->value;
        }
        head = head->next;
    }

    return -1; // non-existent key
}

bool insert(Table * table, char * word, long value) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0) || (value < 0) || (table->nWords > table->maxWords)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * new = (Node *) malloc(sizeof(Node));
    if (new == NULL) {
        perror("(insert) error allocating memory for a new node");
        return false;
    }

    new->word = calloc(32, sizeof(char)); // all new words being inserted have space for 32 bytes
    if (new->word == NULL) {
        perror("(insert) error allocating memory for the word field");
        return false;
    }

    my_str_cpy(new->word, word);
    new->value = value;
    new->next = NULL;

    Node * head = table->array[targetIdx];

    if (head == NULL) {
        table->array[targetIdx] = new;
    } else {
        while (head) {
            if (!my_str_cmp_opt(head->word, word)) {
                if (head->value == value) {
                    return false;
                } else {
                    head->value = value;
                    return true;
                }
            } else if (head->next == NULL) {
                break;
            }
            head = head->next;
        }

        head->next = new;
    }

    table->nWords++;
    return true;
}

bool delete(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * head = table->array[targetIdx];
    Node * prev = NULL;
    bool found = false;

    while (head) {
        if (!my_str_cmp_opt(head->word, word)) {
            found = true;
            Node * temp = head;

            if (prev == NULL) {
                table->array[targetIdx] = head->next;
            } else {
                prev->next = head->next;
            }
            
            free(temp);
            temp = NULL;
            break;
        }

        prev = head;
        head = head->next;
    }

    if (!found) {
        return false;
    }

    table->nWords--;
    return true;
}

bool update(Table * table, char * word, long value) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0) || (value < 0)) {
        return false;
    }

    long hashNum = hash(table, word);
    Node * elem = table->array[hashNum];

    while (elem) {
        if (!my_str_cmp_opt(elem->word, word)) {
            elem->value = value;
            return true;
        } else if (elem->next == NULL) {
            break;
        }
        elem = elem->next;
    }

    if (table->nWords > table->maxWords) {
        return false;
    }

    Node * e = (Node *) malloc(sizeof(Node));

    if (e == NULL) {
        return false;
    }

    e->word = my_str_dup(word);
    e->value = value;
    e->next = NULL;

    if (elem == NULL) {
        table->array[hashNum] = e;
    } else {
        elem->next = e;
    }

    table->nWords++;
    return false;
}

void print(Table * table) {
    if (table != NULL) {
        for (int i = 0; i < table->nBuckets; i++) {
            Node * head = table->array[i];

            if (head != NULL) {
                printf("Bucket %d\n", (i + 1));

                while (head != NULL) {
                    if (head->next != NULL) {
                        printf("Node(Key=%s, Value=%ld)->", head->word, head->value);
                    } else {
                        printf("Node(Key=%s, Value=%ld)\n", head->word, head->value);
                    }
                    head = head->next;
                }
            }
        }
    }
}

bool clear(Table * table) {
    if (table == NULL) {
        return false;
    }

    for (int i = 0; i < table->nBuckets; i++) {
        Node * head = table->array[i];
        Node * temp;

        while (head) {
            temp = head;
            head = head->next;
            free(temp);
        }

        table->array[i] = NULL;
    }

    table->nWords = 0;
    return true;
}