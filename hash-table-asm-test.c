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
void test_init();

void test_lookup(Table * table);
void test_insert(Table * table);
void test_update(Table * table);
void test_delete(Table * table);
void test_get(Table * table);

void test_clear(Table * table);

int main(int argc, char **argv) {
   printf("Running table init tests...\n");
   test_init();
   printf("Table init tests passed...\n\n");

   printf("Running actual table initialization test...\n");
   long maxWords = 256;
   Table * table = ASM_init(maxWords);
   assert(table);
   assert(table->maxWords == maxWords);
   assert(table->nBuckets == maxWords + 1);
   assert(table->array);
   for (int i = 0; i < table->nBuckets; i++) {
      assert(table->array[i] == NULL);
   }
   printf("Actual table initialization test passed...\n\n");

   printf("Running table hash tests...\n");
   test_hash(table);
   printf("Table hash tests passed...\n\n");

   printf("Running table insert tests...\n");
   test_insert(table);
   printf("Table insert tests passed...\n\n");

   printf("Running table lookup tests...\n");
   test_lookup(table);
   printf("Table lookup tests passed...\n\n");

   printf("Running table get tests...\n");
   test_get(table);
   printf("Table get tests passed...\n\n");

   return 0;
}

void test_init() {
   long maxWordsValidOne = 56;
   Table * tableOne = ASM_init(maxWordsValidOne);
   assert(tableOne);
   assert(tableOne->maxWords == maxWordsValidOne);
   assert(tableOne->nBuckets == 67);
   assert(tableOne->array);
   for (int i = 0; i < tableOne->nBuckets; i++) {
     assert(tableOne->array[i] == NULL);
   }

   long maxWordsValidTwo = 113.5;
   Table * tableTwo = ASM_init(maxWordsValidTwo);
   assert(tableTwo);
   assert(tableTwo->maxWords == 113);
   assert(tableTwo->nBuckets == 131);
   assert(tableTwo->array);
   for (int j = 0; j < tableTwo->nBuckets; j++) {
      assert(tableTwo->array[j] == NULL);
   }

   long maxWordsInvalid[3] = {-3, 200000, 0};
   for (int k = 0; k < 3; k++) {
      Table * curr = ASM_init(maxWordsInvalid[k]);
      assert(curr == NULL);
   }
}

void test_hash(Table * table) {
  // test for a valid character
  char *word = "hello";
  long cHashVal = hash(table, word);
  long asmHashVal = ASM_hash(table, word);

  assert(cHashVal == asmHashVal);

  // test for invalid word argument
  assert(hash(table, NULL) == ASM_hash(table, NULL));
  assert(hash(table, "") == ASM_hash(table, ""));
}

void test_insert(Table * table) {
  // brand new key-value pair
  char *keyOne = "Hello";
  long valOne = 15;
  bool insertOne = ASM_insert(table, keyOne, valOne);
  assert(insertOne);
  assert(table->nWords == 1);

  // same key-value pair
  bool insertTwo = ASM_insert(table, keyOne, valOne);
  assert(!insertTwo);
  assert(table->nWords == 1);

  // same key but different value
  bool insertThree = ASM_insert(table, keyOne, 500);
  assert(insertThree);
  assert(table->nWords == 1);

  char *keys[] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
  long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};
  int n = sizeof(vals) / sizeof(long);

  for (int i = 0; i < n; i++) {
    bool insertCurr = ASM_insert(table, keys[i], vals[i]);
    assert(insertCurr);
  }

  assert(table->nWords == 17);

  // insert invalid values

  // null key
  bool insertInvalidOne = ASM_insert(table, NULL, 15);
  assert(!insertInvalidOne);
  assert(table->nWords == 17);

  // negative value
  bool insertInvalidTwo = ASM_insert(table, "hello", -30);
  assert(!insertInvalidTwo);
  assert(table->nWords == 17);
}

void test_lookup(Table * table) {
   // existent key
   bool lookupOne = ASM_lookup(table, "banana");
   assert(lookupOne);

   // non-existent key
   bool lookupTwo = ASM_lookup(table, "diablo");
   assert(!lookupTwo);

   // NULL key
   bool lookupThree = lookup(table, NULL);
   assert(!lookupThree);
}

void test_get(Table * table) {
  // valid key
  long getOne = ASM_get(table, "carrot");
  assert(getOne == 98765);

  // non-existent valid key
  long getTwo = ASM_get(table, "blablah");
  assert(getTwo == -1);

  // invalid key
  long getThree = ASM_get(table, NULL);
  assert(getThree == -1);

}

void test_update(Table * table) {

}

void test_delete(Table * table) {

}

void test_clear(Table * table) {
   
}