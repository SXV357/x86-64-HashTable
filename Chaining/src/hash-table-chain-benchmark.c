#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>
#include <assert.h>
#include "hash-table-c-chain.h"

#define LOAD_FACTOR (0.75)
struct timeval start, end; // for recording times

// ASM functions that will be benchmarked
extern Table * ASM_init(long maxWords);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_lookup(Table * table, char * word);
extern bool ASM_delete(Table * table, char * word);

// forward function declarations
void benchmark_insert(Table * table);
void benchmark_lookup(Table * table);
void benchmark_delete(Table * table);
void benchmark_load_fact_impact(Table * table);

// insert operations
    // Empty bucket insert (best case)
    // Insert with small chain (1-2 elements)
    // Insert with medium chain (5-10 elements)
    // Insert with long chain (15+ elements)

// lookup operations
    // Empty bucket lookup (element not found)
    // First element in chain
    // Middle of chain (5-7 elements deep)
    // End of long chain (worst case)
    // Element not in existing chain

// delete operations
    // Delete from chain start (head)
    // Delete from middle of chain
    // Delete from end of chain
    // Delete non-existent element
    // Delete from empty bucket

// load factor impact
    // Operations at 25% capacity: 0.25(773) = 193
    // Operations at 50% capacity: 0.5(773) = 387
    // Operations at 75% capacity = 0.75(773) = 580
    // operations beyond 75% capacity: > 580 words

int main(int argc, char **argv) {
    long maxWords = 773;
    Table * table = ASM_init(maxWords);
    assert(table->maxWords == maxWords);
    assert(table->nBuckets == 1031);
}

void benchmark_insert(Table * table) {

}

void benchmark_lookup(Table * table) {

}

void benchmark_delete(Table * table) {

}

void benchmark_load_fact_impact(Table * table) {

}