#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "hash-table-c-chain.h"

#define LOAD_FACTOR (0.75)
#define MAX_WORDS (12308)
#define N_TRIALS (1000)
#define MAX_WORD_SIZE (30)

struct timespec start, end; // for measuring time
FILE *wordFp;

// ASM functions that will be benchmarked
extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_lookup(Table * table, char * word);
extern bool ASM_delete(Table * table, char * word);
extern void ASM_print(Table * table);

// forward function declarations
char ** get_random_existent_words(FILE *);
void benchmark_insert(Table *);
void benchmark_lookup(Table *, char **);
void benchmark_delete(Table *);
void benchmark_load_fact_impact(Table *);

int main(int argc, char **argv) {
    printf("Initializing hash table...\n");

    Table * table = init(MAX_WORDS);
    assert(table->maxWords == MAX_WORDS);
    assert(table->nBuckets == 16411);

    printf("Hash Table initialized...\n\n");

    benchmark_insert(table);

    char ** non_existent_words = load_non_existent_words();
    benchmark_lookup(table, non_existent_words);

    /**benchmark_delete(table);
    benchmark_load_fact_impact(table); **/
}

char ** load_non_existent_words() {
  char ** non_existent = malloc(sizeof(char *) * (N_TRIALS / 2));
  assert(non_existent);

  FILE *neFp = fopen("non-existent.txt", "r");
  assert(neFp);

  int i = 0;
  char buf[MAX_WORD_SIZE];
  while (fscanf(neFp, "%s\n", buf) == 1) {
    non_existent[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
    assert(non_existent[i]);

    strncpy(non_existent[i], buf, MAX_WORD_SIZE);
    non_existent[i][strlen(buf)] = '\0';
    i++;
  }

  return non_existent;
}

char ** get_random_existent_words(FILE * fp) {
  fseek(fp, 0, SEEK_SET);
  char ** all_existent_words = malloc(sizeof(char *) * N_TRIALS);
  assert(all_existent_words);

  // read all the 1000 words into an array
  char buf[MAX_WORD_SIZE];
  int i = 0;
  while (fscanf(fp, "%s\n", buf) == 1) {
    all_existent_words[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
    assert(all_existent_words[i]);

    strncpy(all_existent_words[i], buf, MAX_WORD_SIZE);
    all_existent_words[i][strlen(buf)] = '\0';

    i++;
  }

  // extract out 500 random words from the list
  int nTarget = N_TRIALS / 2;
  char ** random_existent_words = malloc(sizeof(char *) * nTarget);
  assert(random_existent_words);

  i = 0;
  while (i < nTarget) {
    random_existent_words[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
    assert(random_existent_words[i]);

    strncpy(random_existent_words[i++], all_existent_words[rand() % N_TRIALS], MAX_WORD_SIZE);
  }

  return random_existent_words;
}

void benchmark_insert(Table * table) {
    // 0.000000307s before optimization
    wordFp = fopen("1000w.txt", "r");
    assert(wordFp != NULL);

    // nanoseconds can get quite large and cause potential overflow for long so use long long as safety measure
    long long insert_total_ns = 0;

    char buf[30] = "";
    while (fscanf(wordFp, "%s\n", buf) == 1) {
      // measure the insertion time for each respective insertion
      clock_gettime(CLOCK_MONOTONIC, &start);
      bool insertCurr = ASM_insert(table, buf, rand());
      clock_gettime(CLOCK_MONOTONIC, &end);

      // get difference in seconds and multiply by 1000000000 to get nanoseconds and add nanosecond diff
      insert_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);
      
      assert(insertCurr);   
    }

    assert(table->nWords == N_TRIALS);
    double insertAvgTime = ((double) insert_total_ns / N_TRIALS) / 1000000000.0;
    printf("Average time for empty bucket insertion: %.9fs\n", insertAvgTime);
}

void benchmark_lookup(Table * table, char ** non_existent_words) {
  char ** random = get_random_existent_words(wordFp);
  int i;

  // 500 existent words
  long long existent_lookup_total_ns = 0;

  for (i = 0; i < N_TRIALS / 2; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool existentLookupCurr = ASM_lookup(table, random[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    existent_lookup_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);
    
    assert(existentLookupCurr);
  }

  // 500 non-existent words
  long long non_existent_lookup_total_ns = 0;
  for (i = 0; i < N_TRIALS / 2; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool nonExistentLookupCurr = ASM_lookup(table, non_existent_words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    existent_lookup_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);
    
    assert(nonExistentLookupCurr);
  }

  double existentLookupAvgTime = ((double) existent_lookup_total_ns / (N_TRIALS / 2)) / 1000000000.0;
  printf("Average time for empty bucket insertion: %.9fs\n", existentLookupAvgTime);

  double nonExistentLookupAvgTime = ((double) non_existent_lookup_total_ns / (N_TRIALS / 2)) / 1000000000.0;
  printf("Average time for empty bucket insertion: %.9fs\n", nonExistentLookupAvgTime);
}

void benchmark_delete(Table * table) {
  // 500 random existent and 500 non-existent elements
}

void benchmark_load_fact_impact(Table * table) {
  // 25% capacity: 0.25(12308) = 3077
  // 50% capacity: 0.5(12308) = 6154
  // 75% capacity: 0.75(12308) = 9231
  // > 75% capacity: 9231 < nWords <= 12308
}