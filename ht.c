#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ht.h"

// TO DOS
    // test current implementation
    // update them to make it more compatible with assembly
    // assembly implementation
    // optimizations for assembly implementation

Table * init(long maxWords) {
    // there needs to be at least one bucket in the hash table for it to be valid
    if (maxWords <= 0) {
        return NULL;
    }

    Table * hashTable = (Table *) malloc(sizeof(Table));

    long primeNumbers[] = { 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073 };
    long nPrimeNumbers = sizeof(primeNumbers)/sizeof(long);

    if (maxWords > primeNumbers[nPrimeNumbers - 1]) {
        // cannot have more than 131,073 buckets
        return NULL;
    }

    // default number of buckets = largest prime number in the list(131,073)
    hashTable->nBuckets = primeNumbers[nPrimeNumbers-1];

    // we set nBuckets = 131,073 but we may not even need those many buckets
    // if the maxWords which is passed in isn't that much, then we realistically don't need those many buckets
    // only have as many buckets as needed which helps in saving space
    for (int i = 0; i < nPrimeNumbers; i++) {
       if ( maxWords < primeNumbers[i]) {
            hashTable->nBuckets =  primeNumbers[i];
            break;
       }
    }

    hashTable->maxWords = maxWords;
    hashTable->nWords = 0; // at the start no words exist in the hash table

    // the table has nBuckets slots(each of these slots contains a linked list)
    hashTable->array = (Node **)
        malloc(hashTable->nBuckets * sizeof(Node *));

    if (hashTable->array == NULL) {
       return NULL;
    }

    // the linked list within each bucket is NULL by default
    for (int i = 0; i < hashTable->nBuckets; i++) {
       hashTable->array[i] = NULL;
    }

    return (void*) hashTable;
}

long hash(void * table, char * word) {
    // hashes the word passed in and returns the index of the bucket it's located in
    Table * hashTable = table;
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

    return hashNum % hashTable->nBuckets;
}

/**
 * Looks up the key specified by the word parameter in the hash table and return true if it exists
 */
bool lookup(void * table, char * word) {
    Table * hashTable  = table;

    long hashNum = hash(hashTable, word);

    // the actual head of the linked list
    Node * elem = hashTable->array[hashNum];

    // traverse thru all elements in the linked list and if we reach the end return false otherwise return true
    while (elem != NULL && strcmp(elem->word,word) != 0) {
      elem = elem->next;
    }

    if ( elem == NULL) {
      //Not found
      return false;
    }

    return true;
}

// gets the value associated with a given key in the hash table
long *get(void * table, char * word) {
    Table * hashTable = table;
    long targetIdx = hash(table, word);

    Node * head = hashTable->array[targetIdx];

    while (head) {
        if (strcmp(head->word, word) == 0) {
            return &(head->value);
        }
        head = head->next;
    }

    return NULL;
}

bool insert(void * table, char * word, long value) {
    // would just be simple linked list tail insertion since we're not using probing as the collision handling mechanism
    // cases to handle
        // we try inserting the exact same key-value pair
        // we insert the same key, but with a different value

    Table * hashTable = table;
    if (hashTable->nWords > hashTable->maxWords) {
        return false;
    }

    long targetIdx = hash(table, word);

    Node * new = (Node *) malloc(sizeof(Node));
    if (new == NULL) {
        perror("error allocating memory for a new node");
        return false;
    }

    // the word field is a pointer to a char so memory needs to be allocated for this as well
    new->word = malloc(strlen(word) + 1);
    if (new->word == NULL) {
        perror("error allocating memory for the word field");
        return false;
    }

    strcpy(new->word, word);
    new->word[strlen(word)] = '\0';
    new->value = value;
    new->next = NULL;

    Node * head = hashTable->array[targetIdx];

    if (head == NULL) {
        // list is empty so no issues
            // doing head = new won't work
        hashTable->array[targetIdx] = new;
    } else {
        Node * temp = head;

        while (temp) {
            // found the same key
            if (strcmp(temp->word, word) == 0) {
                if (temp->value == value) {
                    // if the value is also the same then no changes happen and we return false
                    return false;
                } else {
                    // the value is different so just update
                    temp->value = value;
                    return true;
                }
            } else if (temp->next == NULL) {
                break;
            }
            temp = temp->next;
        }

        // reach here when we're inserting a new key-value pair that hasn't been seen previously
        temp->next = new;
    }

    hashTable->nWords++;
    return true;
}

bool delete(void * table, char * word) {
    // would just be a simple linked list deletion since we're not using probing as the collision handling mechanism
    Table * hashTable = table;
    long targetIdx = hash(table, word);

    Node * head = hashTable->array[targetIdx];
    Node * prev = NULL;
    bool found = false;

    while (head) {
        if (strcmp(head->word, word) == 0) {
            found = true;
            Node * temp = head;

            if (prev == NULL) {
                // deleting the head
                head = head->next;
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

    hashTable->nWords--;
    return true;
}

bool update(void * table, char * word, long value) {
    // finds the key associated with "char * word", updates its value and returns true if successful
    Table * hashTable  = table;

    long hashNum = hash(hashTable, word);

    Node * elem = hashTable->array[hashNum];

    while (elem->next != NULL && strcmp(elem->next->word,word) != 0) {
      elem = elem->next;
    }

    if ( elem->next != NULL) {
        // found
        elem->next->value = value;
        return true;
    }

    // Not found case
    if ( hashTable->nWords > hashTable->maxWords) {
        return false;
    }

    Node * e = (Node *) malloc(sizeof(Node));

    if (e == NULL) {
        return false;
    }

    // create a new key value pair and insert it
    elem->next = e;
    e->word = strdup(word);
    e->value = value;
    e->next = NULL;

    hashTable->nWords++;
    return false;
}

// prints the content of all the nodes in the hash table
void print(void * table) {
    Table * hashTable = table;

    for (int i = 0; i < hashTable->nBuckets; i++) {
        Node * head = hashTable->array[i];

        // only print the contents if the bucket isn't NULL
        if (head) {
            printf("Bucket %d\n", (i + 1));

            while (head) {
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

// clears out the contents of all buckets in the hash table
void clear(void * table) {
    Table * hashTable = table;

    for (int i = 0; i < hashTable->nBuckets; i++) {
        Node * temp = hashTable->array[i];

        while (temp) {
            Node * next = temp->next;
            free(temp);
            temp = NULL;

            temp = next;
        }
    }

    // all key-value pairs have been deleted
    hashTable->nWords = 0;
}