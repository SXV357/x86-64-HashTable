#include "../src/hash-table.h"

/* Global array definitions */

static const char *keys[] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
static const long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};

/* Constant Definitions */

#define MAX_WORDS_OLD (256)
#define MAX_WORDS_NEW (255)
#define N (sizeof(vals) / sizeof(long))

#define TABLE_INIT_TEST_START "Running table initialization tests...\n"
#define TABLE_INIT_TEST_END "Table initialization tests passed...\n\n"

#define TABLE_INIT_ACC_START "Running actual table initialization test...\n"
#define TABLE_INIT_ACC_END "Actual table initialization test passed...\n\n"

#define TABLE_HASH_START "Running table hash tests...\n"
#define TABLE_HASH_END "Table hash tests passed...\n\n"

#define TABLE_INSERT_START "Running table insertion tests...\n"
#define TABLE_INSERT_END "Table insertion tests passed...\n\n"

#define TABLE_LOOKUP_START "Running table lookup tests...\n"
#define TABLE_LOOKUP_END "Table lookup tests passed...\n\n"

#define TABLE_DELETE_START "Running table delete tests...\n"
#define TABLE_DELETE_END "Table delete tests passed...\n\n"

#define TABLE_UPDATE_START "Running table update tests...\n"
#define TABLE_UPDATE_END "Table update tests passed...\n\n"

#define TABLE_GET_START "Running table get tests...\n"
#define TABLE_GET_END "Table get tests passed...\n\n"

#define TABLE_CLEAR_START "Running table clear tests...\n"
#define TABLE_CLEAR_END "Table clear tests passed...\n"

/* Function Prototypes */

void test_init();
void test_hash(Table *);

void test_lookup(Table *);
void test_insert(Table *);
void test_update(Table *);
void test_delete(Table *);
void test_get(Table *);

void test_clear(Table *);