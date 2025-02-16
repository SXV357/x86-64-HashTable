#include "../src/hash-table.h"

// Constant definitions
#define MAX_WORDS_OLD (256)
#define MAX_WORDS_NEW (255)

// Function headers

void test_init();
void test_hash(Table *);

void test_lookup(Table *);
void test_insert(Table *);
void test_update(Table *);
void test_delete(Table *);
void test_get(Table *);

void test_clear(Table *);