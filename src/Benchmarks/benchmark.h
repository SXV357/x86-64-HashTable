#include "../hash-table.h"

#include <stdio.h>
#include <stdbool.h>

/* Constant definitions */

#define HASH_TABLE_INIT_START "Initializing hash table...\n"
#define HASH_TABLE_INIT_END "Hash Table initialized...\n\n"

#define MAX_WORD_SIZE_OLD (30)
#define MAX_WORDS_OLD (12308)
#define N_BUCKETS_OLD (16411)

#define MAX_WORDS_NEW (12288)
#define N_BUCKETS_NEW (16384)

#define N_TRIALS (1000)
#define N_NON_EXISTENT (500)
#define BILLION (1.0E9)
#define BILLION_LL (1000000000LL)

// Old formatters
#define OLD_AVG_INSERT_TIME "Old Average insertion time: %.9fs\n"

#define OLD_AVG_EXISTENT_LOOKUP_TIME "Old Average existent key lookup time: %.9fs\n"
#define OLD_AVG_NON_EXISTENT_LOOKUP_TIME "Old Average non-existent key lookup time: %.9fs\n"

#define OLD_AVG_EXISTENT_DELETE_TIME "Old Average existent key deletion time: %.9fs\n"
#define OLD_AVG_NON_EXISTENT_DELETE_TIME "Old Average non-existent key deletion time: %.9fs\n"

// Optimized formatters
#define OPT_AVG_INSERT_TIME "Optimized Average insertion time: %.9fs\n"

#define OPT_AVG_EXISTENT_LOOKUP_TIME "Optimized Average existent key lookup time: %.9fs\n"
#define OPT_AVG_NON_EXISTENT_LOOKUP_TIME "Optimized Average non-existent key lookup time: %.9fs\n"

#define OPT_AVG_EXISTENT_DELETE_TIME "Optimized Average existent key deletion time: %.9fs\n"
#define OPT_AVG_NON_EXISTENT_DELETE_TIME "Optimized Average non-existent key deletion time: %.9fs\n"

/* Function Prototypes */

char ** load_words(bool, FILE *);
char ** get_random_existent_words(char **);

long long benchmark_insert(Table *);
long long benchmark_lookup_and_delete(Table *, char **, bool (*)(Table *, char *), bool);

void print_metrics(long long, long long, long long, long long, long long);