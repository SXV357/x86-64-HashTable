/* Shreyas Viswanathan, hash-table-asm-test-old.c 
 * Last updated Feb 18, 2025
 */

#include "../tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

/* Function prototypes for the x86-64 functions being tested */
extern Table * ASM_init(long);
extern long ASM_hash(Table *, char *);
extern bool ASM_lookup(Table *, char *);
extern long ASM_get(Table *, char *);
extern bool ASM_insert(Table *, char *, long);
extern bool ASM_delete(Table *, char *);
extern bool ASM_update(Table *, char *, long);
extern bool ASM_clear(Table *);

/* Driver function that runs all tests associated with the old x86-64 hash table implementation. */
int main(int argc, char **argv) {
   printf(TABLE_INIT_TEST_START);
   test_init();
   printf(TABLE_INIT_TEST_END);

   printf(TABLE_INIT_ACC_START);

   Table * table = ASM_init(MAX_WORDS_OLD);
   assert(table);
   assert(table->maxWords == MAX_WORDS_OLD);
   assert(table->nBuckets == MAX_WORDS_OLD + 1);
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

/* Function to test the init function associated with the old x86-64 implementation. */
void test_init() {
   // valid cases
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

   // invalid cases
   long maxWordsInvalid[3] = {-3, 200000, 0};
   for (int k = 0; k < 3; k++) {
      Table * curr = ASM_init(maxWordsInvalid[k]);
      assert(curr == NULL);
   }
} /* test_init() */

/* Function to test the hash function associated with the old x86-64 implementation. */
void test_hash(Table * table) {
   // test for valid characters
   char *words[] = {"hello", "bye", "see", "flag"};

   for (int i = 0; i < 4; i++) {
      assert(hash(table, words[i]) == ASM_hash(table, words[i]));
   }

   // test for invalid word argument
   assert(ASM_hash(table, NULL) == -1);
   assert(ASM_hash(table, "") == -1);

   // test for NULL table
   assert(ASM_hash(NULL, "something") == -1);
   assert(ASM_hash(NULL, "") == -1);
   assert(ASM_hash(NULL, NULL) == -1);
} /* test_hash() */

/* Function to test the insert function associated with the old x86-64 implementation. */
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

  char *keys[16] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
  long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};

  for (int i = 0; i < N; i++) {
    bool insertCurr = ASM_insert(table, keys[i], vals[i]);
    assert(insertCurr);
  }

  assert(table->nWords == 17);

  // null key
  bool insertInvalidOne = ASM_insert(table, NULL, 15);
  assert(!insertInvalidOne);
  assert(table->nWords == 17);

  // negative value
  bool insertInvalidTwo = ASM_insert(table, "hello", -30);
  assert(!insertInvalidTwo);
  assert(table->nWords == 17); 

  // NULL table and empty word tests
  bool insertInvalidThree = ASM_insert(table, "", 15);
  bool insertInvalidFour = ASM_insert(table, "", -10);
  bool insertInvalidFive = ASM_insert(table, NULL, -110);
  bool insertInvalidSix = ASM_insert(NULL, "something", 42);
  bool insertInvalidSeven = ASM_insert(NULL, "", 32);
  bool insertInvalidEight = ASM_insert(NULL, "", -23);
  bool insertInvalidNine = ASM_insert(NULL, NULL, 94);
  bool insertInvalidTen = ASM_insert(NULL, NULL, -5);

  assert((!insertInvalidThree) && (!insertInvalidFour) && (!insertInvalidFive) && (!insertInvalidSix) && 
 (!insertInvalidSeven) && (!insertInvalidEight) && (!insertInvalidNine) && (!insertInvalidTen));
} /* test_insert() */

/* Function to test the lookup function associated with the old x86-64 implementation. */
void test_lookup(Table * table) {
   // existent key
   bool lookupOne = ASM_lookup(table, "banana");
   assert(lookupOne);

   // non-existent key
   bool lookupTwo = ASM_lookup(table, "diablo");
   assert(!lookupTwo);

   // NULL key
   bool lookupThree = ASM_lookup(table, NULL);
   assert(!lookupThree);

   // NULL table and empty word tests
   bool lookupFour = ASM_lookup(NULL, "banana");
   bool lookupFive = ASM_lookup(NULL, NULL);
   bool lookupSix = ASM_lookup(NULL, "");
   bool lookupSeven = ASM_lookup(table, "");

   assert((!lookupFour) && (!lookupFive) && (!lookupSix) && (!lookupSeven));
} /* test_lookup() */

/* Function to test the get function associated with the old x86-64 implementation. */
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

  // NULL table and empty key tests
  long getFour = ASM_get(table, "");
  long getFive = ASM_get(NULL, "carrot");
  long getSix = ASM_get(NULL, "something");
  long getSeven = ASM_get(NULL, NULL);
  long getEight = ASM_get(NULL, "");

  assert(getFour + getFive + getSix + getSeven + getEight == -5);
} /* test_get() */

/* Function to test the update function associated with the old x86-64 implementation. */
void test_update(Table * table) {
  // update value of an existing key
  bool updateOne = ASM_update(table, "apple", 192021);
  assert(updateOne);
  assert(table->nWords == 14);

  // update value of a non-existent key
  bool updateTwo = ASM_update(table, "Kohli", 81);
  assert(!updateTwo);
  assert(table->nWords == 15);

  // invalid values
  bool updateThree = ASM_update(table, NULL, 67);
  assert(!updateThree);

  bool updateFour = ASM_update(table, "elephant", -25);
  assert(!updateFour);

  bool updateCurr = ASM_update(table, "SMG", -112);
  assert(!updateCurr);

  // test NULL table and empty string
  bool updateFive = ASM_update(table, "", 68);
  bool updateSix = ASM_update(table, "", -76);
  bool updateSeven = ASM_update(table, NULL, -43);
  bool updateEight = ASM_update(NULL, "", 90);
  bool updateNine = ASM_update(NULL, "", -36);
  bool updateTen = ASM_update(NULL, "blabla", 11);
  bool updateEleven = ASM_update(NULL, "ajdhjkfe", 34);
  bool updateTwelve = ASM_update(NULL, NULL, 1111);
  bool updateThirteen = ASM_update(NULL, NULL, -765);

  assert((!updateFive) && (!updateSix) && (!updateSeven) && (!updateEight) && (!updateNine) && 
  (!updateTen) && (!updateEleven) && (!updateTwelve) && (!updateThirteen));
} /* test_update() */

/* Function to test the delete function associated with the old x86-64 implementation. */
void test_delete(Table * table) {
  // delete head in bucket with > 1 node
  char * keyOne = "nest";
  long idxOne = ASM_hash(table, keyOne);
  bool deleteOne = ASM_delete(table, keyOne);
  assert(deleteOne);
  assert(table->nWords == 16);
  assert(!strcmp(table->array[idxOne]->word, "yarn"));

  // delete non-head in bucket with > 1 node
  char *keyTwo = "under";
  long idxTwo = ASM_hash(table, keyTwo);
  bool deleteTwo = ASM_delete(table, keyTwo);
  assert(deleteTwo);
  assert(table->nWords == 15);
  assert(!strcmp(table->array[idxTwo]->word, "boat"));

  // delete only node in a bucket
  char *keyThree = "banana";
  long idxThree = ASM_hash(table, keyThree);
  bool deleteThree = ASM_delete(table, keyThree);
  assert(deleteThree);
  assert(table->nWords == 14);
  assert(table->array[idxThree] == NULL);

  // delete non-existent key
  bool deleteFour = ASM_delete(table, "RPG");
  assert(!deleteFour);
  assert(table->nWords == 14);

  // delete NULL key
  bool deleteFive = ASM_delete(table, NULL);
  assert(!deleteFive);
  assert(table->nWords == 14);

  // NULL table and empty word tests
  bool deleteSix = ASM_delete(table, "");
  bool deleteSeven = ASM_delete(NULL, "banana");
  bool deleteEight = ASM_delete(NULL, "");
  bool deleteNine = ASM_delete(NULL, "Rampage");

  assert((!deleteSix) && (!deleteSeven) && (!deleteEight) && (!deleteNine));
} /* test_delete() */

/* Function to test the clear function associated with the old x86-64 implementation. */
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
} /* test_clear() */