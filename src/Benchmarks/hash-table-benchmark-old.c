/* Shreyas Viswanathan, hash-table-benchmark-old.c 
 * Last updated Feb 19, 2025
 */

#include "./benchmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

/* Global variable declarations */

struct timespec start, end; // For measuring time
FILE *wordFp; // File pointer associated with the 1000w.txt file
FILE *neFp; // File pointer associated with the non-existent.txt file

/* Function Prototypes for ASM functions that will be benchmarked */

extern Table * ASM_init(long);
extern bool ASM_insert(Table *, char *, long);
extern bool ASM_lookup(Table *, char *);
extern bool ASM_delete(Table *, char *);
extern bool ASM_clear(Table *);

/* Utility function that takes in accumulated time across the 3 major operations and prints the average times */
void print_metrics(long long oldInsertTime, long long oldExistentLookupTime, long long oldNonExistentLookupTime, 
                   long long oldExistentDeletionTime, long long oldNonExistentDeletionTime) {  
        
  double oldInsertAvgTime = ((double) oldInsertTime / N_TRIALS) / BILLION;
  double oldExistentLookupAvgTime = ((double) oldExistentLookupTime / N_NON_EXISTENT) / BILLION;
  double oldNonExistentLookupAvgTime = ((double) oldNonExistentLookupTime / N_NON_EXISTENT) / BILLION;
  double oldExistentDeleteAvgTime = ((double) oldExistentDeletionTime / N_NON_EXISTENT) / BILLION;
  double oldNonExistentDeleteAvgTime = ((double) oldNonExistentDeletionTime / N_NON_EXISTENT) / BILLION;  

  printf(OLD_AVG_INSERT_TIME, oldInsertAvgTime);
  printf(OLD_AVG_EXISTENT_LOOKUP_TIME, oldExistentLookupAvgTime);
  printf(OLD_AVG_NON_EXISTENT_LOOKUP_TIME, oldNonExistentLookupAvgTime);
  printf(OLD_AVG_EXISTENT_DELETE_TIME, oldExistentDeleteAvgTime);
  printf(OLD_AVG_NON_EXISTENT_DELETE_TIME, oldNonExistentDeleteAvgTime);
} /* print_metrics() */

/* Driver function that runs all the benchmarks and prints the time taken for different operation types. */
int main(int argc, char **argv) {
    srand(time(NULL)); // seed random number generator

    printf(HASH_TABLE_INIT_START);

    Table * table = ASM_init(MAX_WORDS_OLD);
    assert(table->maxWords == MAX_WORDS_OLD);
    assert(table->nBuckets == N_BUCKETS_OLD);
    for (int i = 0; i < table->nBuckets; i++) {
      assert(table->array[i] == NULL);
    }

    printf(HASH_TABLE_INIT_END);

    // wordFp pointer is opened as part of this benchmark function
    long long insert_total_ns = benchmark_insert(table);

    neFp = fopen("src/Words/non-existent.txt", "r");
    assert(neFp);
    char ** non_existent_words = load_words(false, neFp);

    fseek(wordFp, 0, SEEK_SET);
    char ** all_existent_words = load_words(true, wordFp);

    char ** random_existent_words = get_random_existent_words(all_existent_words);

    long long existent_lookup_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_lookup, true);
    long long non_existent_lookup_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_lookup, false);
    long long existent_delete_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_delete, true);
    long long non_existent_delete_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_delete, false);

    print_metrics(insert_total_ns, existent_lookup_total_ns, non_existent_lookup_total_ns, existent_delete_total_ns, non_existent_delete_total_ns);

    /* Cleanup */

    // close all files
    fclose(neFp);
    neFp = NULL;

    fclose(wordFp);
    wordFp = NULL;

    // free all arrays
    int i;
    for (i = 0; i < N_NON_EXISTENT; i++) {
      free(non_existent_words[i]);
      non_existent_words[i] = NULL;

      free(random_existent_words[i]);
      random_existent_words[i] = NULL;
    }

    for (i = 0; i < N_TRIALS; i++) {
      free(all_existent_words[i]);
      all_existent_words[i] = NULL;
    }

    free(non_existent_words);
    free(all_existent_words);
    free(random_existent_words);

    non_existent_words = all_existent_words = random_existent_words = NULL;

    // clear out all nodes and free the table
    bool clearRes = ASM_clear(table);
    assert(clearRes);
    
    free(table->array);
    table->array = NULL;

    free(table);
    table = NULL;

    return 0;
} /* main() */

/* This function either loads in all the words from the 1000w.txt or the non-existent.txt
 * file based on the supplied existent parameter. If existent is true, all the words from
 * 1000w.txt are loaded and returned otherwise all words from non-existent.txt are loaded
 * and returned.
 */
char ** load_words(bool existent, FILE * fp) {
  char ** res = malloc(sizeof(char *) * (existent ? N_TRIALS : N_NON_EXISTENT));
  assert(res);

  int i = 0;
  char buf[MAX_WORD_SIZE_OLD];

  while (fscanf(fp, "%s\n", buf) == 1) {
    res[i] = malloc(sizeof(char) * MAX_WORD_SIZE_OLD);
    assert(res[i]);

    strncpy(res[i++], buf, MAX_WORD_SIZE_OLD);
  }

  return res;
} /* load_words() */

/* This function returns a list of 500 random and unique existent words taken from the
 * array supplied as a parameter and returns it.
 */
char ** get_random_existent_words(char ** all_existent_words) {
  // counter array to keep track of selected words and prevent duplicate ones
  int countFreq[N_TRIALS] = {0};
  int i = 0;

  char ** random_existent_words = malloc(sizeof(char *) * N_NON_EXISTENT);
  assert(random_existent_words);

  // selecting 500 random words and placing them in the random_existent_words array
  while (i < N_NON_EXISTENT) {
    int randomIdx = rand() % N_TRIALS;
    if (countFreq[randomIdx] == 0) {
      random_existent_words[i] = malloc(sizeof(char) * MAX_WORD_SIZE_OLD);
      assert(random_existent_words[i]);

      strncpy(random_existent_words[i++], all_existent_words[randomIdx], MAX_WORD_SIZE_OLD);
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
    char buf[MAX_WORD_SIZE_OLD];

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