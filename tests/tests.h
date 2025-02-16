#include "../../src/hash-table.h"

// Constant definitions
#define MAX_WORDS_OLD (256)

// Function headers

void test_init();
void test_hash(Table * table);

void test_lookup(Table * table);
void test_insert(Table * table);
void test_update(Table * table);
void test_delete(Table * table);
void test_get(Table * table);

void test_clear(Table * table);