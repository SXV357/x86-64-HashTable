#include <stdbool.h>

/* Global variable definitions */

static const long primeNumbers[] = {67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073};
static const long powersOfTwo[] = {64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};

/* Constant Definitions */

#define N_PRIME_NUMBERS (sizeof(primeNumbers) / sizeof(long))
#define N_POWERS_OF_TWO (sizeof(powersOfTwo) / sizeof(long))

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
    char * word; // key
    long value;
    struct HashTableElement * next; // pointer to next node in chain
} Node;

typedef struct HashTable {
    long maxWords; // table capacity
    long nWords; // current key count
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