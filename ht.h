typedef struct HashTableElement {
    char * word; // key
    long value; // value(can be anything)
    struct HashTableElement * next;
} Node;

typedef struct HashTable {
    long maxWords; // maxWords that can exist in the hash table
    long nWords; // current number of words that exist in the hash table
    long nBuckets; // number of buckets that exist
    
    // collision handling mechanism = chaining
        // each slot in the hash table is a linked list
    // array of pointers to HashTableElement structs
    struct HashTableElement ** array;
} Table;

// function headers
Table* init(long maxWords);

bool lookup(void * table, char * word); // searches for a word and returns true/false
bool insert(void * table, char *word, long value); // inserts the (word, value) pair into the hash table
bool update(void * table, char * word, long value); // finds the word and updates its value
bool delete(void * table, char * word); // finds the word and deletes it
long *get(void * table, char * word); // gets the value associated with a word

void print(void * table); // prints all the key value pairs in the table
void clear(void * table); // clears the key value pairs in all buckets in the hash table