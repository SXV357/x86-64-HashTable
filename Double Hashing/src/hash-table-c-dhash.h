#include <stdbool.h>

typedef struct HashTableElement {
    char * word; // will be initialized to NULL to begin with
    long value; // will be initialized to -999999 to begin with
    bool deleted; // a marker to tell whether an element has been deleted or not(set to 0 by default and 1 when deleted)
} Element;

typedef struct HashTable {
    long capacity; // the maximum total number of elements that can be stored
    long nWords; // the current number of elements being stored
    double loadFactor; // load factor for the hash table(resizing happens based on this)

    Element * array; // pointer to an array of structs
} Table;

// utility operations
Table * init(); // will set the initial capacity to a prime number by default
long hash(Table * table, char * word); // returns the appropriate index for a given key after performing the hashing
long size(Table * table); // returns size of the table
bool clear(Table * table); // clears all entries in the hash table
void print(Table * table); // prints all entries in the hash table

// core operations
bool insert(Table * table, char * word, long value); // inserts a new key-value pair in the hash table
bool remove(Table * table, char * word); // removes a specific key-value pair in the hash table
long get(Table * table, char * word); // retrieves the value associated with a specific key in the hash table
bool lookup(Table * table, char * word); // checks whether a given key exists in the hash table
bool update(Table * table, char * word, long value); // updates the value associated with a given key in the hash table