#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash-table-c-chain.h"

// forward declarations for the custom string funcs
int my_str_len(char *);
int my_str_cmp(char *, char *);

int my_str_len(char *s) {
    char *temp = s;
    int len = 0;
    
    while (*temp++ != '\0') {
        len++;
    }
    
    return len;
}

int my_str_cmp(char * s1, char * s2) {
    char *s1_tmp = s1;
    char *s2_tmp = s2;
    
    while (*s1_tmp != '\0' && *s2_tmp != '\0') {
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

    while (elem != NULL && my_str_cmp(elem->word,word) != 0) {
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
        if (!my_str_cmp(head->word, word)) {
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

    new->word = calloc(32, sizeof(char));
    if (new->word == NULL) {
        perror("(insert) error allocating memory for the word field");
        return false;
    }

    strcpy(new->word, word);
    new->value = value;
    new->next = NULL;

    Node * head = table->array[targetIdx];

    if (head == NULL) {
        table->array[targetIdx] = new;
    } else {
        while (head) {
            if (my_str_cmp(head->word, word) == 0) {
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
        if (my_str_cmp(head->word, word) == 0) {
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
        if (my_str_cmp(elem->word, word) == 0) {
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

    e->word = strdup(word);
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