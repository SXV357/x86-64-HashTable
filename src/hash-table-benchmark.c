#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "hash-table.h"

#define MAX_WORDS (12308)
#define N_TRIALS (1000)
#define N_NON_EXISTENT (500)
#define MAX_WORD_SIZE (30)

struct timespec start, end; // for measuring time
FILE *wordFp; // FILE pointer for the 1000w.txt file

// ASM functions that will be benchmarked
extern Table * ASM_init(long maxWords);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_lookup(Table * table, char * word);
extern bool ASM_delete(Table * table, char * word);

// forward function declarations
char ** load_non_existent_words();
char ** get_random_existent_words(FILE *);
void benchmark_insert(Table *);
long long benchmark_lookup_and_delete(Table *, char **, bool (*)(Table *, char *), bool);

int main(int argc, char **argv) {
    srand(time(NULL)); // seed random number generator

    printf("Initializing hash table...\n");

    Table * table = ASM_init(MAX_WORDS);
    assert(table->maxWords == MAX_WORDS);
    assert(table->nBuckets == 16411);

    printf("Hash Table initialized...\n\n");

    // wordFp file pointer is opened as part of this ben
    benchmark_insert(table);

    char ** non_existent_words = load_non_existent_words();
    char ** random_existent_words = get_random_existent_words(wordFp);

    long long existent_lookup_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_lookup, true);

    long long non_existent_lookup_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_lookup, false);

    long long existent_delete_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_delete, true);

    long long non_existent_delete_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_delete, false);


    double existentLookupAvgTime = ((double) existent_lookup_total_ns / N_NON_EXISTENT) / 1000000000.0;
    double nonExistentLookupAvgTime = ((double) non_existent_lookup_total_ns / N_NON_EXISTENT) / 1000000000.0;
    double existentDeleteAvgTime = ((double) existent_delete_total_ns / N_NON_EXISTENT) / 1000000000.0;
    double nonExistentDeleteAvgTime = ((double) non_existent_delete_total_ns / N_NON_EXISTENT) / 1000000000.0;
    printf("Average existent key lookup time: %.9fs\n", existentLookupAvgTime);
    printf("Average non-existent key lookup time: %.9fs\n", nonExistentLookupAvgTime);
    printf("Average existent key deletion time: %.9fs\n", existentDeleteAvgTime);
    printf("Average non-existent key deletion time: %.9fs\n", nonExistentDeleteAvgTime);
}


char ** load_non_existent_words() {
  char ** non_existent = malloc(sizeof(char *) * N_NON_EXISTENT);
  assert(non_existent);

  FILE *neFp = fopen("non-existent.txt", "r");
  assert(neFp);

  int i = 0;
  char buf[MAX_WORD_SIZE];

  while (fscanf(neFp, "%s\n", buf) == 1) {
    non_existent[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
    assert(non_existent[i]);

    strncpy(non_existent[i], buf, MAX_WORD_SIZE);
    i++;
  }

  return non_existent;
}

char ** get_random_existent_words(FILE * fp) {
  // pointer was already opened by insert benchmark function and passed in as param so go back to beginning
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
    i++;
  }

  // extract out 500 random words from the list

  // we need to check whether the random word hasn't already been seen before to prevent duplicates
  int countFreq[N_TRIALS] = {0};

  char ** random_existent_words = malloc(sizeof(char *) * N_NON_EXISTENT);
  assert(random_existent_words);

  i = 0;

  // keep looping until 500 unique random words have been selected
  while (i < N_NON_EXISTENT) {
    int randomIdx = rand() % N_TRIALS;
    if (countFreq[randomIdx] == 0) {
      // word is selected for the first time so OK
      random_existent_words[i] = malloc(sizeof(char) * MAX_WORD_SIZE);
      assert(random_existent_words[i]);

      strncpy(random_existent_words[i++], all_existent_words[randomIdx], MAX_WORD_SIZE);
      countFreq[randomIdx]++;
    }
  }

  return random_existent_words;
}

void benchmark_insert(Table * table) {
    wordFp = fopen("1000w.txt", "r");
    assert(wordFp != NULL);

    // nanoseconds can get quite large and cause potential overflow for long so use long long as safety measure
    long long insert_total_ns = 0;

    char buf[MAX_WORD_SIZE];
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
    printf("Average insertion time: %.9fs\n", insertAvgTime);
}

long long benchmark_lookup_and_delete(Table * table, char ** words, 
                                      bool (*asm_func)(Table *, char *), bool existent) {
  long long total_ns = 0;

  for (int i = 0; i < N_NON_EXISTENT; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool curr = (*asm_func)(table, words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
             (end.tv_nsec - start.tv_nsec);

    assert(existent ? curr : !curr);
  }

  return total_ns;
}