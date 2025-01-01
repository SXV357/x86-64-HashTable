#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

struct WordTableFuncs {
    Table* (*table_init) (long maxWords);

    long (*table_lookup) (void * table, char * word, long * value); // searches for a word and returns true/false
    void (*table_insert) (void * table, char *word, long value); // inserts the (word, value) pair into the hash table
    long (*table_update) (void * table, char * word, long value); // finds the word and updates its value
    void (*table_delete) (void * table, char * word); // finds the word and deletes it
    long (*table_get) (void * table, char * word); // gets the value associated with a word

    void (*table_print) (void * table); // prints all the key value pairs in the table
    void (*table_clear) (void * table); // clears the key value pairs in all buckets in the hash table
};

typedef struct HashTableElement {
    char * word; // key
    long value; // value(can be anything)
    struct HashTableElement * next;
} Node;

typedef struct HashTable {
    struct WordTableFuncs funcs;

    long maxWords; // maxWords that can exist in the hash table
    long nWords; // current number of words that exist in the hash table
    long nBuckets; // number of buckets that exist
    
    // collision handling mechanism = chaining
        // each slot in the hash table is a linked list
    // array of pointers to HashTableElement structs
    struct HashTableElement ** array;
} Table;

struct HashTable * HashTable_C_init(long maxWords) {
    Table * hashTable = (Table *) malloc(sizeof(Table));

    // initialize function
    hashTable->funcs.table_init = HashTable_C_init;

    // core operations
    hashTable->funcs.table_lookup = HashTable_C_lookup;
    hashTable->funcs.table_update = HashTable_C_update;
    hashTable->funcs.table_delete = HashTable_C_delete;
    hashTable->funcs.table_insert = HashTable_C_insert;
    hashTable->funcs.table_get = HashTable_C_get;

    // utilities
    hashTable->funcs.table_print = HashTable_C_print;
    hashTable->funcs.table_clear = HashTable_C_clear;

    long primeNumbers[] = { 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073 };
    long nPrimeNumbers = sizeof(primeNumbers)/sizeof(long);

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
    hashTable->array = (Node *)
        malloc(hashTable->nBuckets * sizeof(Node));

    if (hashTable->array == NULL) {
       return NULL;
    }

    // the linked list within each bucket is NULL by default
    for (int i = 0; i < hashTable->nBuckets; i++) {
       hashTable->array[i] = NULL;
    }

    return (void*) hashTable;
}

long HashTable_C_hash(void * table, char * word) {
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

    // this is invalid since this would end up returning a negative index which won't work
    // if (hashNum < 0) hashNum = - hashNum;

    return hashNum % hashTable->nBuckets;
}

/**
 * Looks up the key specified by the word parameter in the hash table and return true if it exists
 */
long HashTable_C_lookup(void * table, char * word) {
    Table * hashTable  = table;

    long hashNum = HashTable_C_hash(hashTable, word);

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
long HashTable_C_get(void * table, char * word) {
    Table * hashTable = table;
    long targetIdx = HashTable_C_hash(table, word);

    Node * head = hashTable->array[targetIdx];
    while (head) {
        if (strcmp(head->word, word) == 0) {
            return head->value;
        }
        head = head->next;
    }

    return NULL;
}

void HashTable_C_insert(void * table, char * word, long value) {
    // would just be simple linked list tail insertion since we're not using probing as the collision handling mechanism
    Table * hashTable = table;
    assert(hashTable->nWords <= hashTable->maxWords);

    long targetIdx = HashTable_C_hash(table, word);

    Node * new = (Node *) malloc(sizeof(Node));
    assert(new != NULL);

    strcpy(new->word, word);
    new->value = value;
    new->next = NULL;

    Node * head = hashTable->array[targetIdx];
    if (head == NULL) {
        head = new;
    } else {
        Node * temp = head;
        while (temp) {
            if (temp->next == NULL) {
                break;
            }
            temp = temp->next;
        }

        temp->next = new;
    }

    hashTable->nWords++;
}

void HashTable_C_delete(void * table, char * word) {
    // would just be a simple linked list deletion since we're not using probing as the collision handling mechanism
    Table * hashTable = table;
    long targetIdx = HashTable_C_hash(table, word);

    Node * head = hashTable->array[targetIdx];
    if (strcmp(head->word, word) == 0) {
        // delete the head
        Node * temp = head;
        head = head->next;
        free(temp);
        temp = NULL;
    } else {
        Node * prev = NULL;
        while (head) {
            if (strcmp(head->word, word) == 0) {
                Node * curr = head;
                prev->next = head->next;

                free(curr);
                curr = NULL;
                break;
            }
            prev = head;
            head = head->next;
        }
    }

    hashTable->nWords--;
}

long HashTable_C_update(void * table, char * word, long value) {
    // finds the key associated with "char * word", updates its value and returns true if successful
    Table * hashTable  = table;

    long hashNum = HashTable_C_hash(hashTable, word);

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
        // need to assert since we're inserting a new key-value pair in this case
    assert(hashTable->nWords <= hashTable->maxWords);
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

// prints the content of all tge nodes in the hash table
void HashTable_C_print(void * table) {
    Table * hashTable = table;

    for (int i = 0; i < hashTable->nBuckets; i++) {
        Node * head = hashTable->array[i];
        printf("Bucket %d\n", (i + 1));

        while (head) {
            printf("Node(Key=%c, Value=%d)\n", head->word, head->value);
            head = head->next;
        }
    }
}

// clears out the contents of all buckets in the hash table
void HashTable_C_clear(void * table) {
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