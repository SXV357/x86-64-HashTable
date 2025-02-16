#include <stdio.h>
#include <stdbool.h>
#include "../hash-table.h"

// Constants
#define MAX_WORDS_OLD (12308)
#define MAX_WORDS_NEW (12288)

#define N_BUCKETS_OLD (16411)
#define N_BUCKETS_NEW (16384)

#define N_TRIALS (1000)
#define N_NON_EXISTENT (500)
#define BILLION (1.0E9)
#define BILLION_LL (1000000000LL)

// Function headers
char ** load_non_existent_words();
char ** get_random_existent_words(FILE *);
long long benchmark_insert(Table *);
long long benchmark_lookup_and_delete(Table *, char **, bool (*)(Table *, char *), bool);