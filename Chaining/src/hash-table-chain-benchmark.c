#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "hash-table-c-chain.h"

#define LOAD_FACTOR (0.75)
#define MAX_WORDS (12308)
#define N_TRIALS (100)
struct timespec start, end; // for measuring time

// ASM functions that will be benchmarked
extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_lookup(Table * table, char * word);
extern bool ASM_delete(Table * table, char * word);
extern void ASM_print(Table * table);

// forward function declarations
void benchmark_insert(Table * table);
void benchmark_lookup(Table * table);
void benchmark_delete(Table * table);
void benchmark_load_fact_impact(Table * table);

int main(int argc, char **argv) {
    printf("Initializing hash table...\n");

    Table * table = ASM_init(MAX_WORDS);
    assert(table->maxWords == MAX_WORDS);
    assert(table->nBuckets == 16411);

    printf("Hash Table initialized...\n\n");

    benchmark_insert(table);

    /**benchmark_lookup(table);
    benchmark_delete(table);
    benchmark_load_fact_impact(table); **/
}

void benchmark_insert(Table * table) {
  // empty bucket insert
  char *words[] = {
    "apple", "banana", "carrot", "dolphin", "elephant", "falcon", "giraffe", 
    "hippo", "iguana", "jackal", "koala", "lemur", "monkey", "newt", 
    "octopus", "penguin", "quail", "rabbit", "snake", "tiger", "umbrella", 
    "vulture", "walrus", "xenon", "yak", "zebra", "acorn", "berry", "cactus", 
    "daisy", "eagle", "fern", "grape", "holly", "iris", "jasmine", "kale", 
    "lilac", "maple", "nectar", "olive", "peony", "quince", "rose", "sage", 
    "thyme", "unicorn", "violet", "willow", "xerox", "yacht", "zephyr", 
    "axe", "bell", "coin", "dice", "edge", "flute", "globe", "harp", "ink", 
    "jelly", "kite", "lamp", "mask", "nail", "opal", "pearl", "queen", 
    "rope", "salt", "tool", "vase", "watch", "xray", "zone", "arc", 
    "belt", "clay", "drum", "echo", "fire", "gold", "horn", "iron", 
    "key", "leaf", "moon", "nest", "oak", "pine", "quilt", "ring", 
    "star", "tree", "veil", "wave", "basket", "candle", "dragon", 
    "emerald", "feather", "garden", "hammer", "island", "jungle"
  };

  // nanoseconds can get quite large and cause potential overflow for long so use long long as safety measure
  long long total_ns = 0;

  for (int i = 0; i < N_TRIALS; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool insertCurr = ASM_insert(table, words[i], rand());
    clock_gettime(CLOCK_MONOTONIC, &end);

    // get difference in seconds and multiply by 1000000000 to get nanoseconds and add nanosecond diff
    total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

    assert(insertCurr);
  }

  assert(table->nWords == N_TRIALS);

  double emptyBucketInsertionAvgTime = ((double) total_ns / N_TRIALS) / 1000000000.0;
  printf("Average time for empty bucket insertion: %.9fs\n", emptyBucketInsertionAvgTime);

  // insert w small chain(1-2 elements)
  // insert with medium chain(5-10 elements)
  // insert with long chain(15+ elements)
}

void benchmark_lookup(Table * table) {
  // empty bucket lookup
  // first element in chain
  // middle of chain(5-7 elements deep)
  // end of long chain(worst case)
  // element not in existing chain
}

void benchmark_delete(Table * table) {
  // delete from chain start
  // delete from middle of chain
  // delete from end of chain
  // delete non-existent element
}

void benchmark_load_fact_impact(Table * table) {
  // 25% capacity: 0.25(12308) = 3077
  // 50% capacity: 0.5(12308) = 6154
  // 75% capacity: 0.75(12308) = 9231
  // > 75% capacity: 9231 < nWords <= 12308
}