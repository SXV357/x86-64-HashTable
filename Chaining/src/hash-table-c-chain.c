#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash-table-c-chain.h"

Table * init(long maxWords) {
    long primeNumbers[] = { 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073 };
    long nPrimeNumbers = sizeof(primeNumbers)/sizeof(long);

    // there needs to be at least one bucket in the hash table for it to be valid
        // also cannot be more than 131,073 buckets
    if ((maxWords <= 0) || (maxWords > primeNumbers[nPrimeNumbers - 1])) {
        return NULL;
    }

    Table * table = (Table *) malloc(sizeof(Table));
    if (table == NULL) {
        perror("(init) error allocating memory for the table");
        return NULL;
    }

    // default number of buckets = largest prime number in the list(131,073)
    table->nBuckets = primeNumbers[nPrimeNumbers-1];

    // we set nBuckets = 131,073 but we may not even need those many buckets
    // if the maxWords which is passed in isn't that much, then we realistically don't need those many buckets
    // only have as many buckets as needed which helps in saving space
    for (int i = 0; i < nPrimeNumbers; i++) {
       if (maxWords < primeNumbers[i]) {
            table->nBuckets =  primeNumbers[i];
            break;
       }
    }

    table->maxWords = maxWords;
    table->nWords = 0; // at the start no words exist in the hash table

    // the table has nBuckets slots(each of these slots contains a linked list)
    table->array = (Node **)
        malloc(table->nBuckets * sizeof(Node *));

    if (table->array == NULL) {
       perror("(init) error allocating memory for table->array");
       return NULL;
    }

    // the linked list within each bucket is NULL by default
    for (int i = 0; i < table->nBuckets; i++) {
       table->array[i] = NULL;
    }

    return table;
}

long hash(Table * table, char * word) {
    // check for nullity
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        // negative index is appropriate here since such indices are non-existent for arrays
        return -1;
    }
    
    long hashNum = 1;
    long len = strlen(word);

    // ex: if word = "bat", len = 3
        // i = 0(hashNum = 31 + 98)
        // i = 1(hashNum = 31 * (31 + 98) + 97)
        // i = 2(hashNum = 31 * (31 * (31 + 98) + 97) + 116)

    for (long i = 0; i < len; i++) {
        // 31 is prime so ensures even distribution and lesser likelihood of collisions
        hashNum = 31 * hashNum + word[i];
    }

    return hashNum % table->nBuckets;
}

/**
 * Looks up the key specified by the word parameter in the hash table and return true if it exists
 */
bool lookup(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return false;
    }

    long hashNum = hash(table, word);

    // the actual head of the linked list
    Node * elem = table->array[hashNum];

    // traverse thru all elements in the linked list and if we reach the end return false otherwise return true
    while (elem != NULL && strcmp(elem->word,word) != 0) {
      elem = elem->next;
    }

    return elem == NULL ? false : true;
}

// gets the value associated with a given key in the hash table
long get(Table * table, char * word) {
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return -1;
    }

    long targetIdx = hash(table, word);
    Node * head = table->array[targetIdx];

    while (head) {
        if (!strcmp(head->word, word)) {
            return head->value;
        }
        head = head->next;
    }

    return -1; // non-existent key
}

bool insert(Table * table, char * word, long value) {
    // would just be simple linked list tail insertion since we're not using probing as the collision handling mechanism
    // cases to handle
        // we try inserting the exact same key-value pair
        // we insert the same key, but with a different value
    
    // range of values allowed: [0, inf)

    if ((table == NULL) || (word == NULL) || (strlen(word) == 0) || (value < 0) || (table->nWords > table->maxWords)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * new = (Node *) malloc(sizeof(Node));
    if (new == NULL) {
        perror("(insert) error allocating memory for a new node");
        return false;
    }

    // the word field is a pointer to a char so memory needs to be allocated for this as well
    new->word = malloc(strlen(word) + 1);
    if (new->word == NULL) {
        perror("(insert) error allocating memory for the word field");
        return false;
    }

    strcpy(new->word, word);
    new->word[strlen(word)] = '\0';
    new->value = value;
    new->next = NULL;

    Node * head = table->array[targetIdx];

    if (head == NULL) {
        // list is empty so no issues
            // doing head = new won't work
        table->array[targetIdx] = new;
    } else {
        while (head) {
            // found the same key
            if (strcmp(head->word, word) == 0) {
                if (head->value == value) {
                    // if the value is also the same then no changes happen and we return false
                    return false;
                } else {
                    // the value is different so just update
                    head->value = value;
                    return true;
                }
            } else if (head->next == NULL) {
                break;
            }
            head = head->next;
        }

        // reach here when we're inserting a new key-value pair that hasn't been seen previously
        head->next = new;
    }

    table->nWords++;
    return true;
}

bool delete(Table * table, char * word) {
    // would just be a simple linked list deletion since we're not using probing as the collision handling mechanism
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0)) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * head = table->array[targetIdx];
    Node * prev = NULL;
    bool found = false;

    while (head) {
        if (strcmp(head->word, word) == 0) {
            found = true;
            Node * temp = head;

            if (prev == NULL) {
                // deleting the head
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
        // the key to be deleted wasn't found
        return false;
    }

    table->nWords--;
    return true;
}

bool update(Table * table, char * word, long value) {
    // finds the key associated with "char * word", updates its value and returns true if successful
    if ((table == NULL) || (word == NULL) || (strlen(word) == 0) || (value < 0)) {
        return false;
    }

    long hashNum = hash(table, word);
    Node * elem = table->array[hashNum];

    while (elem) {
        if (strcmp(elem->word, word) == 0) {
            // update its value and return true
            elem->value = value;
            return true;
        } else if (elem->next == NULL) {
            break;
        }
        elem = elem->next;
    }

    // Not found case
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

    // create a new key value pair and insert it
    if (elem == NULL) {
        table->array[hashNum] = e;
    } else {
        elem->next = e;
    }

    table->nWords++;
    return false;
}

// prints the content of all the nodes in the hash table
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

// clears out the contents of all buckets in the hash table
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

    // all key-value pairs have been deleted
    table->nWords = 0;
    return true;
}