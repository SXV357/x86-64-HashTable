#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "../hash-table.h"
#include "../str.h"

#define MAX_WORDS (12288)
#define N_TRIALS (1000)
#define N_NON_EXISTENT (500)
#define BILLION (1.0E9)
#define BILLION_LL (1000000000LL)

struct timespec start, end; // for measuring time
FILE *wordFp; // FILE pointer for the 1000w.txt file

// ASM functions that will be benchmarked
extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table *, char *);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_lookup(Table * table, char * word);
extern bool ASM_delete(Table * table, char * word);
extern void ASM_print(Table *);

// forward function declarations
char ** load_non_existent_words();
char ** get_random_existent_words(FILE *);
long long benchmark_insert(Table *);
long long benchmark_lookup_and_delete(Table *, char **, bool (*)(Table *, char *), bool);
void print_bucket(Table *, long);

void print_bucket(Table * t, long idx) {
  assert((idx >= 0) && (idx <= t->nBuckets - 1));

  Node * head = t->array[idx];
  while (head) {
    if (head->next != NULL) {
      printf("Node(Key=%s, Value=%ld)->", head->word, head->value);
    } else {
      printf("Node(Key=%s, Value=%ld)\n", head->word, head->value);
    }
    head = head->next;
  }
}

int main(int argc, char **argv) {
    srand(time(NULL)); // seed random number generator

    printf("Initializing hash table...\n");

    Table * table = ASM_init(MAX_WORDS);
    assert(table->maxWords == MAX_WORDS);
    assert(table->nBuckets == 16384);
    for (int i = 0; i < table->nBuckets; i++) {
        assert(table->array[i] == NULL);
    }

    printf("Hash Table initialized...\n\n");

    // wordFp file pointer is opened as part of this ben
    long long insert_total_ns = benchmark_insert(table);

    char ** non_existent_words = load_non_existent_words();
    char ** random_existent_words = get_random_existent_words(wordFp);

    long long existent_lookup_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_lookup, true);

    long long non_existent_lookup_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_lookup, false);

    long long existent_delete_total_ns = benchmark_lookup_and_delete(table, random_existent_words, ASM_delete, true);

    long long non_existent_delete_total_ns = benchmark_lookup_and_delete(table, non_existent_words, ASM_delete, false);

    double insertionAvgTime = ((double) insert_total_ns / N_TRIALS) / BILLION;
    double existentLookupAvgTime = ((double) existent_lookup_total_ns / N_NON_EXISTENT) / BILLION;
    double nonExistentLookupAvgTime = ((double) non_existent_lookup_total_ns / N_NON_EXISTENT) / BILLION;
    double existentDeleteAvgTime = ((double) existent_delete_total_ns / N_NON_EXISTENT) / BILLION;
    double nonExistentDeleteAvgTime = ((double) non_existent_delete_total_ns / N_NON_EXISTENT) / BILLION;

    printf("Average insertion time: %.9fs\n", insertionAvgTime);
    printf("Average existent key lookup time: %.9fs\n", existentLookupAvgTime);
    printf("Average non-existent key lookup time: %.9fs\n", nonExistentLookupAvgTime);
    printf("Average existent key deletion time: %.9fs\n", existentDeleteAvgTime);
    printf("Average non-existent key deletion time: %.9fs\n", nonExistentDeleteAvgTime);
}

char ** load_non_existent_words() {
  char ** non_existent = malloc(sizeof(char *) * N_NON_EXISTENT);
  assert(non_existent);

  FILE *neFp = fopen("src/Words/non-existent.txt", "r");
  assert(neFp);

  int i = 0;
  char buf[MAX_KEY_SIZE];

  while (fscanf(neFp, "%s\n", buf) == 1) {
    non_existent[i] = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(non_existent[i]);

    my_str_cpy(non_existent[i++], buf);
  }

  return non_existent;
}

char ** get_random_existent_words(FILE * fp) {
  // pointer was already opened by insert benchmark function and passed in as param so go back to beginning
  fseek(fp, 0, SEEK_SET);

  char ** all_existent_words = malloc(sizeof(char *) * N_TRIALS);
  assert(all_existent_words);

  // read all the 1000 words into an array
  char buf[MAX_KEY_SIZE];
  int i = 0;

  while (fscanf(fp, "%s\n", buf) == 1) {
    all_existent_words[i] = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(all_existent_words[i]);

    my_str_cpy(all_existent_words[i++], buf);
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
      random_existent_words[i] = calloc(MAX_KEY_SIZE, sizeof(char));
      assert(random_existent_words[i]);

      my_str_cpy(random_existent_words[i++], all_existent_words[randomIdx]);
      countFreq[randomIdx]++;
    }
  }

  return random_existent_words;
}

long long benchmark_insert(Table * table) {
    wordFp = fopen("src/Words/1000w.txt", "r");
    assert(wordFp);

    // nanoseconds can get quite large and cause potential overflow for long so use long long as safety measure
    long long insert_total_ns = 0;
    char buf[MAX_KEY_SIZE];

    while (fscanf(wordFp, "%s\n", buf) == 1) {
      // measure the insertion time for each respective insertion
      char *curr = calloc(MAX_KEY_SIZE, sizeof(char));
      assert(curr);
      my_str_cpy(curr, buf);
      long val = rand();
      long idx = ASM_hash(table, curr);

    //   printf("Expected bucket index where %s should be inserted: %ld\n", curr, ASM_hash(table, curr));
      printf("Inserting pair(Key=%s, Value=%ld) at bucket index %ld\n", curr, val, idx);

    //   if (!strcmp(curr, "plenty")) {
    //     printf("plenty is being inserted with value %ld\n", val);
    //     long targetIdx = ASM_hash(table, curr);
    //     printf("Index for plenty: %ld\n", targetIdx);
        
    //     // Print bucket contents
    //     Node* head = table->array[targetIdx];
    //     while (head) {
    //         printf("Node at %p: word=%s, value=%ld\n", 
    //                (void*)head, head->word, head->value);
    //         head = head->next;
    //     }
    // }

      clock_gettime(CLOCK_MONOTONIC, &start);
      bool insertCurr = ASM_insert(table, curr, val);
      clock_gettime(CLOCK_MONOTONIC, &end);

      // get difference in seconds and multiply by 1000000000 to get nanoseconds and add nanosecond diff
      insert_total_ns += (end.tv_sec - start.tv_sec) * BILLION_LL +
                            (end.tv_nsec - start.tv_nsec);

      assert(insertCurr);

      printf("After inserting %s at bucket index %ld, contents of the bucket:\n\n", curr, idx);
      print_bucket(table, idx);
      printf("\n\n");
    }

    assert(table->nWords == N_TRIALS);
    return insert_total_ns;
}

long long benchmark_lookup_and_delete(Table * table, char ** words, 
                                      bool (*asm_func)(Table *, char *), bool existent) {
  long long total_ns = 0;

  for (int i = 0; i < N_NON_EXISTENT; i++) {
    if (asm_func == ASM_delete && existent) {
      long idx = ASM_hash(table, words[i]);
      printf("We expect %s to be present in the hash table at bucket index %ld and we want to delete it\n\n", words[i], idx);
      printf("Contents of bucket %ld before deleting %s:\n\n", idx, words[i]);
      print_bucket(table, idx);
      printf("\n\n");
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool curr = (*asm_func)(table, words[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    total_ns += (end.tv_sec - start.tv_sec) * BILLION_LL +
             (end.tv_nsec - start.tv_nsec);

    assert(existent ? curr : !curr);
  }

  return total_ns;
}