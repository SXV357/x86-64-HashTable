/* Shreyas Viswanathan, hash-table-asm-test-opt.c 
 * Last updated Feb 18, 2025
 */

#include "../../src/Utils/str.h"
#include "../tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

/* Function prototypes for the x86-64 functions being tested */

extern Table * ASM_init(long maxWords);
extern long ASM_hash(Table * table, char * word);
extern bool ASM_lookup(Table * table, char * word);
extern long ASM_get(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_delete(Table * table, char * word);
extern bool ASM_update(Table * table, char * word, long value);
extern bool ASM_clear(Table * table);

/* Driver function that runs all tests associated with the optimized x86-64 hash table implementation. */
int main(int argc, char **argv) {
   printf(TABLE_INIT_TEST_START);
   test_init();
   printf(TABLE_INIT_TEST_END);

   printf(TABLE_INIT_ACC_START);

   Table * table = ASM_init(MAX_WORDS_NEW);
   assert(table);
   assert(table->maxWords == MAX_WORDS_NEW);
   assert(table->nBuckets == MAX_WORDS_NEW + 1);
   assert(table->array);
   for (int i = 0; i < table->nBuckets; i++) {
      assert(table->array[i] == NULL);
   }

   printf(TABLE_INIT_ACC_END);

   printf(TABLE_HASH_START);
   test_hash(table);
   printf(TABLE_HASH_END);

   printf(TABLE_INSERT_START);
   test_insert(table);
   printf(TABLE_INSERT_END);

   printf(TABLE_LOOKUP_START);
   test_lookup(table);
   printf(TABLE_LOOKUP_END);

   printf(TABLE_GET_START);
   test_get(table);
   printf(TABLE_GET_END);

   printf(TABLE_DELETE_START);
   test_delete(table);
   printf(TABLE_DELETE_END);

   printf(TABLE_UPDATE_START);
   test_update(table);
   printf(TABLE_UPDATE_END);

   printf(TABLE_CLEAR_START);
   test_clear(table);
   printf(TABLE_CLEAR_END);

   return 0;
} /* main() */

/* Function to test the init function associated with the optimized x86-64 implementation. */
void test_init() {
   // valid cases
   long maxWordsValidOne = 56;
   Table * tableOne = ASM_init(maxWordsValidOne);
   assert(tableOne);
   assert(tableOne->maxWords == maxWordsValidOne);
   assert(tableOne->nBuckets == 64);
   assert(tableOne->array);
   for (int i = 0; i < tableOne->nBuckets; i++) {
     assert(tableOne->array[i] == NULL);
   }

   long maxWordsValidTwo = 113.5;
   Table * tableTwo = ASM_init(maxWordsValidTwo);
   assert(tableTwo);
   assert(tableTwo->maxWords == 113);
   assert(tableTwo->nBuckets == 128);
   assert(tableTwo->array);
   for (int j = 0; j < tableTwo->nBuckets; j++) {
      assert(tableTwo->array[j] == NULL);
   }

   // invalid cases
   long maxWordsInvalid[3] = {-3, 200000, 0};
   for (int k = 0; k < 3; k++) {
      Table * curr = ASM_init(maxWordsInvalid[k]);
      assert(curr == NULL);
   }
} /* test_init() */

/* Function to test the hash function associated with the optimized x86-64 implementation. */
void test_hash(Table * table) {
  // valid keys
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
} /* test_hash() */

/* Function to test the insert function associated with the optimized x86-64 implementation. */
void test_insert(Table * table) {
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

  // brand new key-value pair
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

  for (int i = 0; i < N; i++) {
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
} /* test_insert() */

/* Function to test the lookup function associated with the optimized x86-64 implementation. */
void test_lookup(Table * table) {
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

   // existent key
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
} /* test_lookup() */

/* Function to test the get function associated with the optimized x86-64 implementation. */
void test_get(Table * table) {
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

  // existent valid key
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
} /* test_get() */

/* Function to test the update function associated with the optimized x86-64 implementation. */
void test_update(Table * table) {
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

   // update value of an existing key
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
} /* test_update() */

/* Function to test the delete function associated with the optimized x86-64 implementation. */
void test_delete(Table * table) {
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

   // delete head in bucket with > 1 node
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
} /* test_delete() */

/* Function to test the clear function associated with the optimized x86-64 implementation. */
void test_clear(Table * table) {
   // NULL table
   bool clearOne = ASM_clear(NULL);
   assert(!clearOne);

   // non-NULL table
   bool clearTwo = ASM_clear(table);
   assert(clearTwo);
   for (int i = 0; i < table->nBuckets; i++) {
    assert(table->array[i] == NULL);
   }

   assert(table->nWords == 0);
} /* test_clear() */