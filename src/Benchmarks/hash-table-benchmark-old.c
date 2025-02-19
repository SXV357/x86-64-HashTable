/* Shreyas Viswanathan, hash-table-benchmark-old.c 
 * Last updated Feb 18, 2025
 */

#include "./benchmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

/* Constant definitions */
#define MAX_WORD_SIZE (30)

/* Global variable declarations */

struct timespec start, end; // For measuring time
FILE *wordFp; // File pointer associated with the 1000w.txt file

/* Function Prototypes for ASM functions that will be benchmarked */

extern Table * ASM_init(long);
extern bool ASM_insert(Table *, char *, long);
extern bool ASM_lookup(Table *, char *);
extern bool ASM_delete(Table *, char *);

/* Driver function that runs all the benchmarks and prints the time taken for different operation types. */
int main(int argc, char **argv) {
    srand(time(NULL)); // seed random number generator

    printf("Initializing hash table...\n");

    Table * table = ASM_init(MAX_WORDS_OLD);
    assert(table->maxWords == MAX_WORDS_OLD);
    assert(table->nBuckets == N_BUCKETS_OLD);
    for (int i = 0; i < table->nBuckets; i++) {
      assert(table->array[i] == NULL);
    }

    printf("Hash Table initialized...\n\n");

    long long insert_total_ns = benchmark_insert(table);

    char ** non_existent_words = load_non_existent_words();
    char ** random_existent_words = get_random_existent_words(wordFp);

    long long existent_lookup_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_lookup, true);

    long long non_existent_lookup_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_lookup, false);

    long long existent_delete_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_delete, true);

    long long non_existent_delete_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_delete, false);

    double insertAvgTime = ((double) insert_total_ns / N_TRIALS) / BILLION;
    double existentLookupAvgTime = ((double) existent_lookup_total_ns / N_NON_EXISTENT) / BILLION;
    double nonExistentLookupAvgTime = ((double) non_existent_lookup_total_ns / N_NON_EXISTENT) / BILLION;
    double existentDeleteAvgTime = ((double) existent_delete_total_ns / N_NON_EXISTENT) / BILLION;
    double nonExistentDeleteAvgTime = ((double) non_existent_delete_total_ns / N_NON_EXISTENT) / BILLION;

    printf("Average insertion time: %.9f\n", insertAvgTime);
    printf("Average existent key lookup time: %.9fs\n", existentLookupAvgTime);
    printf("Average non-existent key lookup time: %.9fs\n", nonExistentLookupAvgTime);
    printf("Average existent key deletion time: %.9fs\n", existentDeleteAvgTime);
    printf("Average non-existent key deletion time: %.9fs\n", nonExistentDeleteAvgTime);

    return 0;
} /* main() */

/* This function opens the non-existent.txt file, reads all 500
 * words into the non_existent array and then returns it.
 */
char ** load_non_existent_words() {
  char ** non_existent = malloc(sizeof(char *) * N_NON_EXISTENT);
  assert(non_existent);

  FILE *neFp = fopen("src/Words/non-existent.txt", "r");
  assert(neFp);

  int i = 0;
  char buf[MAX_WORD_SIZE];

  while (fscanf(neFp, "%s\n", buf) == 1) {
    non_existent[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
    assert(non_existent[i]);

    strncpy(non_existent[i++], buf, MAX_WORD_SIZE);
  }

  return non_existent;
} /* load_non_existent_words() */

/* This function returns a list of 500 unique random words from the 1000w.txt file.
 * It first reads all 1000 words from the 1000w.txt file into the 
 * all_existent_words array. Then, it selects 500 random words from this list,
 * reads them into the random_existent_words array and returns it.
 */
char ** get_random_existent_words(FILE * fp) {
  // pointer reset to the start of file since this function is executed post the benchmark_insert operation
  fseek(fp, 0, SEEK_SET);

  // reading all 1000 words into the array
  char ** all_existent_words = malloc(sizeof(char *) * N_TRIALS);
  assert(all_existent_words);

  char buf[MAX_WORD_SIZE];
  int i = 0;

  while (fscanf(fp, "%s\n", buf) == 1) {
    all_existent_words[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
    assert(all_existent_words[i]);

    strncpy(all_existent_words[i++], buf, MAX_WORD_SIZE);
  }

  // counter array to keep track of selected words and prevent duplicate ones
  int countFreq[N_TRIALS] = {0};
  i = 0;

  char ** random_existent_words = malloc(sizeof(char *) * N_NON_EXISTENT);
  assert(random_existent_words);

  // selecting 500 random words and placing them in the random_existent_words array
  while (i < N_NON_EXISTENT) {
    int randomIdx = rand() % N_TRIALS;
    if (countFreq[randomIdx] == 0) {
      random_existent_words[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
      assert(random_existent_words[i]);

      strncpy(random_existent_words[i++], all_existent_words[randomIdx], MAX_WORD_SIZE);
      countFreq[randomIdx]++;
    }
  }

  return random_existent_words;
} /* get_random_existent_words() */

/* This function runs the insert benchmark operation. It opens the 1000w.txt file and for
 * each word read, it inserts it along with a random value into the hash table. The time
 * is measured for each individual insertion operation in this case.
 */
long long benchmark_insert(Table * table) {
    wordFp = fopen("src/Words/1000w.txt", "r");
    assert(wordFp);

    long long insert_total_ns = 0;
    char buf[MAX_WORD_SIZE];

    while (fscanf(wordFp, "%s\n", buf) == 1) {
      clock_gettime(CLOCK_MONOTONIC, &start);
      bool insertCurr = ASM_insert(table, buf, rand());
      clock_gettime(CLOCK_MONOTONIC, &end);

      insert_total_ns += (end.tv_sec - start.tv_sec) * BILLION_LL +
                            (end.tv_nsec - start.tv_nsec);

      assert(insertCurr);
    }

    assert(table->nWords == N_TRIALS);
    return insert_total_ns;
} /* benchmark_insert() */

/* This function either runs the lookup benchmark or delete benchmark operation depending on
 * what the pointer pointed to by the asm_func parameter points to. It operates similarly to
 * the insert benchmark function where the time is measured for each individual lookup or 
 * deletion operation.
 */
long long benchmark_lookup_and_delete(Table * table, char ** words, 
                                      bool (*asm_func)(Table *, char *), bool existent) {
  long long total_ns = 0;

  for (int i = 0; i < N_NON_EXISTENT; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool curr = (*asm_func)(table, words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    total_ns += (end.tv_sec - start.tv_sec) * BILLION_LL +
             (end.tv_nsec - start.tv_nsec);

    assert(existent ? curr : !curr);
  }

  return total_ns;
} /* benchmark_lookup_and_delete() */