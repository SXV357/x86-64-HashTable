#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "hash-table-c-chain.h"

#define MAX_WORDS (12308)
#define N_TRIALS (1000)
#define N_NON_EXISTENT (500)
#define MAX_WORD_SIZE (30)

struct timespec start, end; // for measuring time
FILE *wordFp; // FILE pointer for the 1000w.txt file

// ASM functions that will be benchmarked
extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_lookup(Table * table, char * word);
extern bool ASM_delete(Table * table, char * word);
extern void ASM_print(Table * table);

// forward function declarations
char ** load_non_existent_words();
char ** get_random_existent_words(FILE *);
void print_bucket(Table *, long);
void benchmark_insert(Table *);
void benchmark_lookup(Table *, char **);
void benchmark_delete(Table *, char **);

int main(int argc, char **argv) {
    srand(time(NULL)); // seed random number generator

    printf("Initializing hash table...\n");

    Table * table = ASM_init(MAX_WORDS);
    assert(table->maxWords == MAX_WORDS);
    assert(table->nBuckets == 16411);

    printf("Hash Table initialized...\n\n");

    // wordFp file pointer is opened as part of this benchmark function
    benchmark_insert(table);

    char ** non_existent_words = load_non_existent_words();
    benchmark_lookup(table, non_existent_words);
    benchmark_delete(table, non_existent_words);
}

// debugging utility
void print_bucket(Table * table, long idx) {
  assert(idx >= 0 && idx <= table->nBuckets - 1);

  Node * head = table->array[idx];
  if (!head) {
    printf("NULL\n");
  }
  else {
  while (head) {
    if (head->next != NULL) {
        printf("(Key = %s, Value = %ld)->", head->word, head->value);
    } else {
        printf("(Key = %s, Value = %ld)\n", head->word, head->value);
    }
    head = head->next;
  }
  }
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
    // 0.000000307s before optimization
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
    printf("Average time for insertion into the hash table: %.9fs\n", insertAvgTime);
}

void benchmark_lookup(Table * table, char ** non_existent_words) {
  char ** random_existent_words = get_random_existent_words(wordFp);
  int i; // loop var

  // 500 existent words
  long long existent_lookup_total_ns = 0;

  for (i = 0; i < N_NON_EXISTENT; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool existentLookupCurr = ASM_lookup(table, random_existent_words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    existent_lookup_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);

    assert(existentLookupCurr);
  }

  // 500 non-existent words
  long long non_existent_lookup_total_ns = 0;

  for (i = 0; i < N_NON_EXISTENT; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool nonExistentLookupCurr = ASM_lookup(table, non_existent_words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    non_existent_lookup_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);

    assert(!nonExistentLookupCurr);
  }

  double existentLookupAvgTime = ((double) existent_lookup_total_ns / N_NON_EXISTENT) / 1000000000.0;
  printf("Average time for existent key lookup: %.9fs\n", existentLookupAvgTime);

  double nonExistentLookupAvgTime = ((double) non_existent_lookup_total_ns / N_NON_EXISTENT) / 1000000000.0;
  printf("Average time for non-existent key lookup: %.9fs\n", nonExistentLookupAvgTime);
}

void benchmark_delete(Table * table, char ** non_existent_words) { 
  char ** random_existent_words = get_random_existent_words(wordFp);
  int i; // loop var

  // 500 existent words
  long long existent_delete_total_ns = 0;

  for (i = 0; i < N_NON_EXISTENT; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);

    // no matter what word it is, it was inserted in the table as part of the insert benchmark function
      // so existentDeleteCurr is expected to always return true
    bool existentDeleteCurr = ASM_delete(table, random_existent_words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    existent_delete_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);

    assert(existentDeleteCurr);
  }

  // 500 non-existent words
  long long non_existent_delete_total_ns = 0;

  for (i = 0; i < N_NON_EXISTENT; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool nonExistentDeleteCurr = ASM_delete(table, non_existent_words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    non_existent_delete_total_ns += (end.tv_sec - start.tv_sec) * 1000000000LL +
                            (end.tv_nsec - start.tv_nsec);

    assert(!nonExistentDeleteCurr);
  }

  double existentDeleteAvgTime = ((double) existent_delete_total_ns / N_NON_EXISTENT) / 1000000000.0;
  printf("Average time for existent key deletion: %.9fs\n", existentDeleteAvgTime);

  double nonExistentDeleteAvgTime = ((double) non_existent_delete_total_ns / N_NON_EXISTENT) / 1000000000.0;
  printf("Average time for non-existent key delete: %.9fs\n", nonExistentDeleteAvgTime);
}