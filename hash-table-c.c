#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

// TO DO:
    // read thru code to understand implementation
    // implement an effective collision handling mechanism if not done so alr
    // implement delete and print functions

struct WordTableFuncs {
    struct HashTable* (*table_init) (long maxWords);
    long (*table_lookup) (void * table, char * word, long * value); // searches for a word and returns true/false
    long (*table_insert) (void * table, char *word, long value); // inserts the (word, value) pair into the hash table
    long (*table_update) (void * table, char * word, long value); // finds the word and updates its value
    long (*table_delete) (void * table, char * word); // finds the word and deletes it
    void (*table_print) (void * table);
};

struct HashTableElement {
    char * word; // key
    long value; // value(can be anything)
    struct HashTableElement * next;
};

struct HashTable {
    struct WordTableFuncs funcs;

    long maxWords; // maxWords that can exist in the hash table
    long nWords; // current number of words that exist in the hash table
    long nBuckets; // number of buckets that exist
    
    // collision handling mechanism = chaining
        // each slot in the hash table is a linked list
    struct HashTableElement * array;
};

struct HashTable * HashTable_C_init(long maxWords) {
    struct HashTable * hashTable = (struct HashTable *) malloc(sizeof(struct HashTable));

    // initialize all the functions associated with this hash table
    hashTable->funcs.table_init = HashTable_C_init;
    hashTable->funcs.table_lookup = HashTable_C_lookup;
    hashTable->funcs.table_update = HashTable_C_update;
    hashTable->funcs.table_delete = HashTable_C_delete;
    hashTable->funcs.table_print = HashTable_C_print;

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
    hashTable->array = (struct HashTableElement *)
        malloc(hashTable->nBuckets * sizeof(struct HashTableElement));

    if (hashTable->array == NULL) {
       perror("error when initializing the array element...");
       exit(1);
    }

    // the word and value elements could still hold random values since they weren't explicitly initialized
    // hashTable->array[i] itself isn't NULL since memory was allocated for it via the previous statement
    for (int i = 0; i < hashTable->nBuckets; i++) {
        hashTable->array[i].next = NULL;
    }

    return (void*) hashTable;
}

long HashTable_C_hash(void * table, char * word) {
    // hashes the word passed in and returns the index of the bucket it's located in
    struct HashTable * hashTable = table;
    long hashNum = 1;
    long len = strlen(word);

    // ex: if word = "bat", len = 3
        // i = 0(hashNum = 31 + 98)
        // i = 1(hashNum = 31 * (31 + 98) + 97)
        // i = 2(hashNum = 31 * (31 * (31 + 98) + 97) + 116)

    for (long i = 0; i < len; i++) {
        hashNum = 31 * hashNum + word[i];
    }

    // this is invalid since this would end up returning a negative index which won't work
    // if (hashNum < 0) hashNum = - hashNum;

    return hashNum % hashTable->nBuckets;
}

long HashTable_C_lookup(void * table, char * word, long * value) {
    struct HashTable * hashTable  = table;

    long hashNum = HashTable_C_hash(hashTable, word);

    // starting element in the bucket
    struct HashTableElement * elem = hashTable->array[hashNum].next;

    // traverse thru all elements in the linked list and if we reach the end return false otherwise return true
    while (elem != NULL && strcmp(elem->word,word) != 0) {
      elem = elem->next;
    }

    if ( elem == NULL) {
      //Not found
      return false;
    }

    *value = elem->value;
    return true;
}

long HashTable_C_insert(void * table, char * word, long value) {
    // TO DO
    return false;
}

long HashTable_C_delete(void * table, char * word) {
    // TO DO
    return false;
}

long HashTable_C_update(void * table, char * word, long value) {
    struct HashTable * hashTable  = table;

    long hashNum = HashTable_C_hash(hashTable, word);
    struct HashTableElement * elem = &hashTable->array[hashNum];

    while (elem->next != NULL && strcmp(elem->next->word,word) != 0) {
      elem = elem->next;
    }

    if ( elem->next != NULL) {
        // found
        elem->next->value = value;
        return true;
    }

    // Not found
    struct HashTableElement * e = (struct HashTableElement *)
                        malloc(sizeof(struct HashTableElement));
    if (e == NULL) {
        perror("malloc");
        exit(1);
    }

    elem->next = e;
    e->word = strdup(word);
    e->value = value;
    e->next = NULL;

    return false;
}

void HashTable_C_print(void * table) {
    // TO DO
}