/* Shreyas Viswanathan, hash-table-old.c 
 * Last updated Feb 18, 2025
 */

#include "../hash-table.h"
#include "../Utils/str.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* This function initializes a hash table based on the capacity provided and then returns it. */
Table * init(long maxWords) {
    long powersOfTwo[12] = {64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
    
    if ((maxWords <= 0) || (maxWords > powersOfTwo[N - 1])) {
        return NULL;
    }

    Table * table = malloc(sizeof(Table));
    if (!table) {
        perror(TABLE_ALLOCATION_ERR_MSG);
        return NULL;
    }

    // set it to the largest power of 2 by default because we will later factor maxWords into this
    table->nBuckets = powersOfTwo[N - 1];

    // set number of buckets based on maxWords parameter(for saving space)
    for (int i = 0; i < N; i++) {
       if (maxWords < powersOfTwo[i]) {
            table->nBuckets = powersOfTwo[i];
            break;
       }
    }

    table->maxWords = maxWords;
    table->nWords = 0;

    table->array = malloc(table->nBuckets * sizeof(Node *));
    if (!(table->array)) {
       perror(TABLE_ARRAY_ALLOCATION_ERR_MSG);
       return NULL;
    }

    // each bucket contains a chain which is set to NULL initially
    for (int i = 0; i < table->nBuckets; i++) {
       table->array[i] = NULL;
    }

    return table;
} /* init() */

/* This function returns the index that the word parameter hashes to in the range [0, table->nBuckets - 1]. */
long hash(Table * table, char * word) {
    if ((table == NULL) || (word == NULL)) {
        return -1;
    }

    int len = my_str_len(word);
    if (!len) return -1;
    
    long hashNum = 1;

    for (long i = 0; i < len; i++) {
        // same as hashNum * 31 + word[i]
        hashNum = ((hashNum << 5) - hashNum) + word[i];
    }

    // same as hashNum % table->nBuckets
    return hashNum & (table->nBuckets - 1);
} /* hash() */

/* Looks up the word specified by the parameter in the hash table and returns if it exists or not. */
bool lookup(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return false;
    }

    long hashNum = hash(table, word);

    Node * prev = NULL;
    Node * head = table->array[hashNum];

    while (head) {
        if (!(my_str_cmp_opt(head->word, word))) {
            break;
        }
        prev = head;
        head = head->next;
    }

    if (!head) {
        return false;
    }

    if (prev != NULL) {
        prev->next = head->next;

        head->next = table->array[hashNum];
        table->array[hashNum] = head;
    }

    return true;
} /* lookup() */

/* Looks up the word specified by the parameter in the hash table and returns the value associated with it.
 * If the word doesn't exist, -1 is returned since negative values are not allowed by default.
 */
long get(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return -1;
    }

    long targetIdx = hash(table, word);
    Node * head = table->array[targetIdx];

    while (head) {
        if (!(my_str_cmp_opt(head->word, word))) {
            return head->value;
        }
        head = head->next;
    }

    return -1;
} /* get() */

/* Inserts the key-value pair specified by the word and value parameters into the hash table.
 * If the specified key-value pair already exists then false is returned but if the key exists
 * and insertion is performed with a different value, the old value is overwritten.
 */
bool insert(Table * table, char * word, long value) {
    // empty keys and negative values not allowed
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0) || (value < 0) || 
        (table->nWords > table->maxWords)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * new = malloc(sizeof(Node));
    if (!new) {
        perror(NODE_ALLOCATION_ERR_MSG);
        return false;
    }

    new->word = calloc(MAX_KEY_SIZE, sizeof(char));
    if (!(new->word)) {
        perror(NODE_WORD_ALLOCATION_ERR_MSG);
        return false;
    }

    my_str_cpy(new->word, word);
    new->value = value;
    new->next = NULL;

    Node * head = table->array[targetIdx];

    // case 1: empty chain
    if (!head) {
        table->array[targetIdx] = new;
    } else {
        // case 2: non-empty chain
        while (head) {
            if (!(my_str_cmp_opt(head->word, word))) {
                // the specified key-value pair already exists
                if (head->value == value) {
                    return false;
                } else {
                    // the value is different so update it
                    head->value = value;
                    return true;
                }
            } else if (head->next == NULL) {
                break;
            }
            head = head->next;
        }

        // inserting the new node
        head->next = new;
    }

    table->nWords++;
    return true;
} /* insert() */

/* Deletes the node with the key specified by the parameter from the hash table if it exists.
 * If the key doesn't exist, the function returns false otherwise true.
 */
bool delete(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0)) {
        return false;
    }

    long targetIdx = hash(table, word);
    Node * head = table->array[targetIdx];

    // prev node tracking necessary for deleting nodes not at the start of the chain
    Node * prev = NULL;
    bool found = false;

    while (head) {
        if (!(my_str_cmp_opt(head->word, word))) {
            found = true;
            Node * temp = head;

            // the node is at the start of chain
            if (!prev) {
                table->array[targetIdx] = head->next;
            } else {
                // either in the middle or the end
                prev->next = head->next;
            }
            
            free(temp);
            temp = NULL;
            break;
        }

        prev = head;
        head = head->next;
    }

    // the node with the key isn't found
    if (!found) {
        return false;
    }

    table->nWords--;
    return true;
} /* delete() */

/* Updates the value of the node with the key the same as the specified word parameter and returns true
 * if the operation was successful. In the case where the node isn't found, a new node with the specified
 * word and value parameters as key and value is inserted at the end of the specific chain.
 */
bool update(Table * table, char * word, long value) {
    if ((table == NULL) || (word == NULL) || (my_str_len(word) == 0) || (value < 0)) {
        return false;
    }

    long hashNum = hash(table, word);
    Node * head = table->array[hashNum];

    while (head) {
        if (!(my_str_cmp_opt(head->word, word))) {
            head->value = value;
            return true;
        } else if (head->next == NULL) {
            break;
        }
        head = head->next;
    }

    // at this point we know the node doesn't exist so make sure we're not at capacity
    if (table->nWords > table->maxWords) {
        return false;
    }

    // create a new node and insert it at the end of the chain of the specified bucket
    Node * e = malloc(sizeof(Node));
    if (!e) {
        return false;
    }

    e->word = my_str_dup(word);
    e->value = value;
    e->next = NULL;

    if (!head) {
        table->array[hashNum] = e;
    } else {
        head->next = e;
    }

    table->nWords++;

    // return false since a new node was inserted and the value of the node wasn't updated like usual
    return false;
} /* update() */

/* Prints the contents only of the non-NULL buckets of the hash table as long as it isn't NULL. */
void print(Table * table) {
    if (table) {
        for (int i = 0; i < table->nBuckets; i++) {
            Node * head = table->array[i];

            if (head) {
                printf(BUCKET_NUMBER_NEW, i);

                while (head) {
                    char *currWord = head->word;
                    long currVal = head->value;

                    if (head->next) {
                        printf(NODE_NEXT_NOT_NULL, currWord, currVal);
                    } else {
                        printf(NODE_NEXT_NULL, currWord, currVal);
                    }
                    head = head->next;
                }
            }
        }
    }
} /* print() */

/* Clears the contents of all buckets of the hash table and returns true if the operation is
 * successful.
 */
bool clear(Table * table) {
    if (!table) {
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
} /* clear() */