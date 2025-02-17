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