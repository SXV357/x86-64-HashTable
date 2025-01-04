#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "hash-table-c.h"

// functions defined in the assembly implementation
extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table * table, char * word);

extern bool ASM_lookup(Table * table, char * word);
extern long ASM_get(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_delete(Table * table, char * word);
extern bool ASM_update(Table * table, char * word, long value);

extern void ASM_print(Table * table);
extern void ASM_clear(Table * table);

// function forward declarations
void test_hash(Table * table);
void test_init(long maxWords);

void test_lookup(Table * table);
void test_insert(Table * table);
void test_update(Table * table);
void test_delete(Table * table);
void test_get(Table * table);

void test_clear(Table * table);

int main(int argc, char **argv) {
   Table * table = init(256);
   assert(table);
   assert(table->nBuckets == 257);
   assert(table->maxWords == 256);

   printf("Running hash tests...\n");
   test_hash(table);
   printf("Hash tests passed...\n\n");

   return 0;
}

void test_hash(Table * table) {
  // test for a valid character
  char *word = "hello";
  long cHashVal = hash(table, word);
  long asmHashVal = ASM_hash(table, word);

  assert(cHashVal == asmHashVal);
  assert(hash(table, NULL) == ASM_hash(table, NULL));
}

void test_init(long maxWords) {

}

void test_lookup(Table * table) {
   
}

void test_insert(Table * table) {

}

void test_update(Table * table) {

}

void test_delete(Table * table) {

}

void test_get(Table * table) {

}

void test_clear(Table * table) {
   
}