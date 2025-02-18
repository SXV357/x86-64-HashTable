#include <stdbool.h>

/* Constant Definitions */

#define TABLE_ALLOCATION_ERR_MSG "(init) error allocating memory for the table"
#define TABLE_ARRAY_ALLOCATION_ERR_MSG "(init) error allocating memory for table->array"
#define NODE_ALLOCATION_ERR_MSG "(insert) error allocating memory for a new node"
#define NODE_WORD_ALLOCATION_ERR_MSG "(insert) error allocating memory for the word field"
#define BUCKET_NUMBER_OLD "Bucket %d\n"
#define BUCKET_NUMBER_NEW "Bucket index %d\n"
#define NODE_NEXT_NOT_NULL "Node(Key=%s, Value=%ld)->"
#define NODE_NEXT_NULL "Node(Key=%s, Value=%ld)\n"

/* Structure Definitions */
typedef struct HashTableElement {
    char * word;
    long value;
    struct HashTableElement * next;
} Node;

typedef struct HashTable {
    long maxWords;
    long nWords;
    long nBuckets;
    struct HashTableElement ** array;
} Table;

/* Function Prototypes */

Table * init(long);
long hash(Table *, char *);

bool lookup(Table *, char *);
bool insert(Table *, char *, long);
bool update(Table *, char *, long);
bool delete(Table *, char *);
long get(Table *, char *);

void print(Table *);
bool clear(Table *);