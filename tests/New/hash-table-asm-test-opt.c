#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "../../src/hash-table.h"
#include "../../src/str.h"
#include "../tests.h"

// For testing the new x86 implementation

// functions defined in the assembly implementation
extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table * table, char * word);

extern bool ASM_lookup(Table * table, char * word);
extern long ASM_get(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_delete(Table * table, char * word);
extern bool ASM_update(Table * table, char * word, long value);

extern bool ASM_clear(Table * table);

int main(int argc, char **argv) {
   printf("Running table init tests...\n");
   test_init();
   printf("Table init tests passed...\n\n");

   printf("Running actual table initialization test...\n");

   Table * table = ASM_init(MAX_WORDS_NEW);
   assert(table);
   assert(table->maxWords == MAX_WORDS_NEW);
   assert(table->nBuckets == MAX_WORDS_NEW + 1);
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

   printf("Running table delete tests...\n");
   test_delete(table);
   printf("Table delete tests passed...\n\n");

   printf("Running table update tests...\n");
   test_update(table);
   printf("Table update tests passed...\n\n");

   printf("Running table clear test...\n");
   test_clear(table);
   printf("Table clear test passed...\n");

   return 0;
}

void test_init() {
   long maxWordsValidOne = 56;
   Table * tableOne = ASM_init(maxWordsValidOne);
   assert(tableOne);
   assert(tableOne->maxWords == maxWordsValidOne);
   assert(tableOne->nBuckets == 64); // UPDATE 2
   assert(tableOne->array);
   for (int i = 0; i < tableOne->nBuckets; i++) {
     assert(tableOne->array[i] == NULL);
   }

   long maxWordsValidTwo = 113.5;
   Table * tableTwo = ASM_init(maxWordsValidTwo);
   assert(tableTwo);
   assert(tableTwo->maxWords == 113);
   assert(tableTwo->nBuckets == 128); // UPDATE 3
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
  char *word = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(word);
  my_str_cpy(word, "hello");

  assert(hash(table, word) == ASM_hash(table, word));

  char *rem[] = {"bye", "see", "flag"};

  for (int i = 0; i < 3; i++) {
   char *curr = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(curr);
   my_str_cpy(curr, rem[i]);

   assert(hash(table, curr) == ASM_hash(table, curr));
  }

  // test for invalid word argument
  assert(ASM_hash(table, NULL) == -1);
  assert(ASM_hash(table, "") == -1);

  // test for NULL table
  char *w1 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w1);
  my_str_cpy(w1, "something");

  char *w2 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w2);
  my_str_cpy(w2, "");

  assert(ASM_hash(NULL, w1) == -1);
  assert(ASM_hash(NULL, w2) == -1);
  assert(ASM_hash(NULL, NULL) == -1);
}

void test_insert(Table * table) {
  // brand new key-value pair
  char *w1, *w2, *w3, *w4;
  
  w1 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w1);
  my_str_cpy(w1, "Hello");

  w2 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w2);
  my_str_cpy(w2, "hello");

  w3 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w3);
  my_str_cpy(w3, "");

  w4 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w4);
  my_str_cpy(w4, "something");

  long valOne = 15;
  bool insertOne = ASM_insert(table, w1, valOne);
  assert(insertOne);
  assert(table->nWords == 1);

  // same key-value pair
  bool insertTwo = ASM_insert(table, w1, valOne);
  assert(!insertTwo);
  assert(table->nWords == 1);

  // same key but different value
  bool insertThree = ASM_insert(table, w1, 500);
  assert(insertThree);
  assert(table->nWords == 1);

  char *keys[] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
  long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};
  int n = sizeof(vals) / sizeof(long);

  for (int i = 0; i < n; i++) {
    char *curr = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(curr);
    my_str_cpy(curr, keys[i]);

    bool insertCurr = ASM_insert(table, curr, vals[i]);
    assert(insertCurr);
  }

  assert(table->nWords == 17);

  // null key
  bool insertInvalidOne = ASM_insert(table, NULL, 15);
  assert(!insertInvalidOne);
  assert(table->nWords == 17);

  // negative value
  bool insertInvalidTwo = ASM_insert(table, w2, -30);
  assert(!insertInvalidTwo);
  assert(table->nWords == 17); 

  // NULL table and empty word tests
  bool insertInvalidThree = ASM_insert(table, w3, 15);
  bool insertInvalidFour = ASM_insert(table, w3, -10);
  bool insertInvalidFive = ASM_insert(table, NULL, -110);
  bool insertInvalidSix = ASM_insert(NULL, w4, 42);
  bool insertInvalidSeven = ASM_insert(NULL, w3, 32);
  bool insertInvalidEight = ASM_insert(NULL, w3, -23);
  bool insertInvalidNine = ASM_insert(NULL, NULL, 94);
  bool insertInvalidTen = ASM_insert(NULL, NULL, -5);

  assert((!insertInvalidThree) && (!insertInvalidFour) && (!insertInvalidFive) && (!insertInvalidSix) && 
 (!insertInvalidSeven) && (!insertInvalidEight) && (!insertInvalidNine) && (!insertInvalidTen));
}

void test_lookup(Table * table) {
   // existent key
   char *w1, *w2, *w3, *w4, *w5;

   w1 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w1);
   my_str_cpy(w1, "banana");

   w2 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w2);
   my_str_cpy(w2, "diablo");

   w3 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w3);
   my_str_cpy(w3, "");

   w4 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w4);
   my_str_cpy(w4, "nest");

   w5 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w5);
   my_str_cpy(w5, "fish");

   bool lookupOne = ASM_lookup(table, w1);
   assert(lookupOne);

   // non-existent key
   bool lookupTwo = ASM_lookup(table, w2);
   assert(!lookupTwo);

   // NULL key
   bool lookupThree = ASM_lookup(table, NULL);
   assert(!lookupThree);

   // NULL table and empty word tests
   bool lookupFour = ASM_lookup(NULL, w1);
   bool lookupFive = ASM_lookup(NULL, NULL);
   bool lookupSix = ASM_lookup(NULL, w3);
   bool lookupSeven = ASM_lookup(table, w3);

   // test where after a successful lookup, the node is moved to front of chain
   long idx = ASM_hash(table, w4);
   bool lookupEight = lookup(table, w4);
   assert(lookupEight);

   assert(!my_str_cmp_opt(table->array[idx]->word, w4));
   assert(!my_str_cmp_opt(table->array[idx]->next->word, w5));
   assert(table->array[idx]->next->next == NULL);

   assert((!lookupFour) && (!lookupFive) && (!lookupSix) && (!lookupSeven));
}

void test_get(Table * table) {
  // valid key
  char *w1, *w2, *w3, *w4;

  w1 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w1);
  my_str_cpy(w1, "carrot");

  w2 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w2);
  my_str_cpy(w2, "blablah");

  w3 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w3);
  my_str_cpy(w3, "something");

  w4 = calloc(MAX_KEY_SIZE, sizeof(char));
  assert(w4);
  my_str_cpy(w4, "");

  long getOne = ASM_get(table, w1);
  assert(getOne == 98765);

  // non-existent valid key
  long getTwo = ASM_get(table, w2);
  assert(getTwo == -1);

  // invalid key
  long getThree = ASM_get(table, NULL);
  assert(getThree == -1);

  // NULL table and empty key tests
  long getFour = ASM_get(table, w4);
  long getFive = ASM_get(NULL, w1);
  long getSix = ASM_get(NULL, w3);
  long getSeven = ASM_get(NULL, NULL);
  long getEight = ASM_get(NULL, w4);

  assert(getFour + getFive + getSix + getSeven + getEight == -5);
}

void test_update(Table * table) {
  // update value of an existing key
  char *w1, *w2, *w3, *w4, *w5, *w6, *w7;

   w1 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w1);
   my_str_cpy(w1, "apple");

   w2 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w2);
   my_str_cpy(w2, "Kohli");

   w3 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w3);
   my_str_cpy(w3, "elephant");   

   w4 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w4);
   my_str_cpy(w4, "SMG");

   w5 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w5);
   my_str_cpy(w5, "");

   w6 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w6);
   my_str_cpy(w6, "blabla");

   w7 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w7);
   my_str_cpy(w7, "ajdhjkfe");

  bool updateOne = ASM_update(table, w1, 192021);
  assert(updateOne);
  assert(table->nWords == 15);

  // update value of a non-existent key
  bool updateTwo = ASM_update(table, w2, 81);
  assert(!updateTwo);
  assert(table->nWords == 16);

  // invalid values
  bool updateThree = ASM_update(table, NULL, 67);
  assert(!updateThree);

  bool updateFour = ASM_update(table, w3, -25);
  assert(!updateFour);

  bool updateCurr = ASM_update(table, w4, -112);
  assert(!updateCurr);

  // test NULL table and empty string
  bool updateFive = ASM_update(table, w5, 68);
  bool updateSix = ASM_update(table, w5, -76);
  bool updateSeven = ASM_update(table, NULL, -43);
  bool updateEight = ASM_update(NULL, w5, 90);
  bool updateNine = ASM_update(NULL, w5, -36);
  bool updateTen = ASM_update(NULL, w6, 11);
  bool updateEleven = ASM_update(NULL, w7, 34);
  bool updateTwelve = ASM_update(NULL, NULL, 1111);
  bool updateThirteen = ASM_update(NULL, NULL, -765);

  assert((!updateFive) && (!updateSix) && (!updateSeven) && (!updateEight) && (!updateNine) && 
  (!updateTen) && (!updateEleven) && (!updateTwelve) && (!updateThirteen));
}

void test_delete(Table * table) {
  // delete head in bucket with > 1 node
  char *w1, *w2, *w3, *w4, *w5, *w6;

   w1 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w1);
   my_str_cpy(w1, "nest");

   w2 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w2);
   my_str_cpy(w2, "fish");

   w3 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w3);
   my_str_cpy(w3, "banana");

   w4 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w4);
   my_str_cpy(w4, "RPG");

   w5 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w5);
   my_str_cpy(w5, "");

   w6 = calloc(MAX_KEY_SIZE, sizeof(char));
   assert(w6);
   my_str_cpy(w6, "Rampage");

  long idxOne = ASM_hash(table, w1);

  bool deleteOne = ASM_delete(table, w1);
  assert(deleteOne);
  assert(table->nWords == 16);
  assert(!my_str_cmp_opt(table->array[idxOne]->word, w2));

  // delete only node in a bucket
  long idxThree = ASM_hash(table, w3);
  bool deleteThree = ASM_delete(table, w3);
  assert(deleteThree);
  assert(table->nWords == 15);
  assert(table->array[idxThree] == NULL);

  // delete non-existent key
  bool deleteFour = ASM_delete(table, w4);
  assert(!deleteFour);
  assert(table->nWords == 15);

  // delete NULL key
  bool deleteFive = ASM_delete(table, NULL);
  assert(!deleteFive);
  assert(table->nWords == 15);

  // NULL table and empty word tests
  bool deleteSix = ASM_delete(table, w5);
  bool deleteSeven = ASM_delete(NULL, w6);
  bool deleteEight = ASM_delete(NULL, w5);

  assert((!deleteSix) && (!deleteSeven) && (!deleteEight));
}

void test_clear(Table * table) {
   // NULL table
   bool clearOne = ASM_clear(NULL);
   assert(!clearOne);

   bool clearTwo = ASM_clear(table);
   assert(clearTwo);
   for (int i = 0; i < table->nBuckets; i++) {
    assert(table->array[i] == NULL);
   }

   assert(table->nWords == 0);
}