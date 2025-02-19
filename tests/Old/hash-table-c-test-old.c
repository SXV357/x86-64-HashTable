/* Shreyas Viswanathan, hash-table-c-test-old.c 
 * Last updated Feb 19, 2025
 */

#include "../tests.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Driver function that runs all tests associated with the old C hash table implementation. */
int main(int argc, char **argv) {
    printf(TABLE_INIT_TEST_START);
    test_init();
    printf(TABLE_INIT_TEST_END);

    printf(TABLE_INIT_ACC_START);

    Table * table = init(MAX_WORDS_OLD);
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
    
    printf(TABLE_UPDATE_START);
    test_update(table);
    printf(TABLE_UPDATE_END);
    
    printf(TABLE_GET_START);
    test_get(table);
    printf(TABLE_GET_END);

    printf(TABLE_DELETE_START);
    test_delete(table);
    printf(TABLE_DELETE_END);

    printf(TABLE_CLEAR_START);
    test_clear(table);
    printf(TABLE_CLEAR_END);

    free(table->array);
    table->array = NULL;

    free(table);
    table = NULL;

    return 0;
} /* main() */

/* Function to test the hash function associated with the old C implementation. */
void test_hash(Table * table) {
    // invalid values
    assert(hash(table, NULL) == -1);
    assert(hash(table, "") == -1);
    assert(hash(NULL, "something") == -1);
    assert(hash(NULL, "") == -1);
    assert(hash(NULL, NULL) == -1);

    // general cases
    char *words[] = {"hello", "bye", "see", "flag"};
    long indices[4] = {22, 92, 132, 20};

    for (int i = 0; i < 4; i++) {
        assert(hash(table, words[i]) == indices[i]);
    }
} /* test_hash() */

/* Function to test the init function associated with the old C implementation. */
void test_init() {
    // valid cases
    long maxWordsValidOne = 56;
    Table * tableOne = init(maxWordsValidOne);
    assert(tableOne);
    assert(tableOne->maxWords == maxWordsValidOne);
    assert(tableOne->nBuckets == 67);
    assert(tableOne->array);
    for (int i = 0; i < tableOne->nBuckets; i++) {
        assert(tableOne->array[i] == NULL);
    }

    free(tableOne->array);
    tableOne->array = NULL;

    free(tableOne);
    tableOne = NULL;

    long maxWordsValidTwo = 113.5;
    Table * tableTwo = init(maxWordsValidTwo);
    assert(tableTwo);
    assert(tableTwo->maxWords == 113);
    assert(tableTwo->nBuckets == 131);
    assert(tableTwo->array);
    for (int j = 0; j < tableTwo->nBuckets; j++) {
        assert(tableTwo->array[j] == NULL);
    }

    free(tableTwo->array);
    tableTwo->array = NULL;

    free(tableTwo);
    tableTwo = NULL;

    // invalid cases
    long maxWordsInvalid[3] = {-3, 200000, 0};
    for (int k = 0; k < 3; k++) {
        Table * curr = init(maxWordsInvalid[k]);
        assert(curr == NULL);
    }
} /* test_init() */

/* Function to test the insert function associated with the old C implementation. */
void test_insert(Table * table) {
    // brand new key-value pair
    char *keyOne = "Hello";
    long valOne = 15;
    bool insertOne = insert(table, keyOne, valOne);
    assert(insertOne);
    assert(table->nWords == 1);

    // same key-value pair
    bool insertTwo = insert(table, keyOne, valOne);
    assert(!insertTwo);
    assert(table->nWords == 1);

    // same key but different value
    long valTwo = 500;
    bool insertThree = insert(table, keyOne, valTwo);
    assert(insertThree);
    assert(table->nWords == 1);

    char *keys[16] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
    long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};

    for (int i = 0; i < N_KEYS; i++) {
        bool insertCurr = insert(table, keys[i], vals[i]);
        assert(insertCurr);
    }

    assert(table->nWords == 17);

    // key is NULL
    bool insertInvalidOne = insert(table, NULL, 15);
    assert(!insertInvalidOne);
    assert(table->nWords == 17);

    // value is less than 0
    bool insertInvalidTwo = insert(table, "hello", -57);
    assert(!insertInvalidTwo);
    assert(table->nWords == 17);

    // mix of invalid insert arguments
    bool insertInvalidThree = insert(table, "", 15);
    bool insertInvalidFour = insert(table, "", -10);
    bool insertInvalidFive = insert(table, NULL, -110);
    bool insertInvalidSix = insert(NULL, "something", 42);
    bool insertInvalidSeven = insert(NULL, "", 32);
    bool insertInvalidEight = insert(NULL, "", -23);
    bool insertInvalidNine = insert(NULL, NULL, 94);
    bool insertInvalidTen = insert(NULL, NULL, -5);

    assert((!insertInvalidThree) && (!insertInvalidFour) && (!insertInvalidFive) && (!insertInvalidSix) && (!insertInvalidSeven) && (!insertInvalidEight) && (!insertInvalidNine) && (!insertInvalidTen));
} /* test_insert() */

/* Function to test the lookup function associated with the old C implementation. */
void test_lookup(Table * table) {
    // existent key
    char *existent = "banana";
    bool lookupOne = lookup(table, existent);
    assert(lookupOne);

    // non-existent key
    char *nonExistent = "diablo";
    bool lookupTwo = lookup(table, nonExistent);
    assert(!lookupTwo);

    // invalid values
    bool lookupThree = lookup(table, NULL);
    bool lookupFour = lookup(table, "");
    bool lookupFive = lookup(NULL, "banana");
    bool lookupSix = lookup(NULL, "diablo");
    bool lookupSeven = lookup(NULL, NULL);

    assert((!lookupThree) && (!lookupFour) && (!lookupFive) && (!lookupSix) && (!lookupSeven));
} /* test_lookup() */

/* Function to test the update function associated with the old C implementation. */
void test_update(Table * table) {
    // update the value of an existing key
    char *existingKey = "under";
    long newValOne = 192021;
    bool updateOne = update(table, existingKey, newValOne);
    assert(updateOne);
    assert(table->nWords == 17);

    // update the value of a non-existent key
    char *nonExistentKey = "formula";
    long newValTwo = 489321;
    bool updateTwo = update(table, nonExistentKey, newValTwo);
    assert(!updateTwo);
    assert(table->nWords == 18);

    // invalid values
    bool updateThree = update(table, NULL, 45);
    assert(!updateThree);

    bool updateFour = update(table, "elephant", -100);
    assert(!updateFour);

    // mix of other invalid update args
    bool updateFive = update(table, "", 68);
    bool updateSix = update(table, "", -76);
    bool updateSeven = update(table, NULL, -43);
    bool updateEight = update(NULL, "", 90);
    bool updateNine = update(NULL, "", -36);
    bool updateTen = update(NULL, "blabla", 11);
    bool updateEleven = update(NULL, "ajdhjkfe", 34);
    bool updateTwelve = update(NULL, NULL, 1111);
    bool updateThirteen = update(NULL, NULL, -765);

    assert((!updateFive) && (!updateSix) && (!updateSeven) && (!updateEight) && (!updateNine) && (!updateTen) && (!updateEleven) && (!updateTwelve) && (!updateThirteen));
} /* test_update() */

/* Function to test the get function associated with the old C implementation. */
void test_get(Table * table) {
    // valid key
    long getOne = get(table, "carrot");
    assert(getOne == 98765);

    // non-existent valid key
    long getTwo = get(table, "blablah");
    assert(getTwo == -1);

    // invalid values
    long getThree = get(table, NULL);
    long getFour = get(table, "");
    long getFive = get(NULL, "carrot");
    long getSix = get(NULL, "blablah");
    long getSeven = get(NULL, NULL);

    assert(getThree + getFour + getFive + getSix + getSeven == -5);
} /* test_get() */

/* Function to test the delete function associated with the old C implementation. */
void test_delete(Table * table) {    
    // delete head in a bucket with > 1 node
    long idxOne = hash(table, "nest");
    bool deleteOne = delete(table, "nest");
    assert(deleteOne);
    assert(table->nWords == 17);
    assert(!strcmp(table->array[idxOne]->word, "yarn"));

    // delete non-head in a bucket with > 1 node
    long idxTwo = hash(table, "under");
    bool deleteTwo = delete(table, "under");
    assert(deleteTwo);
    assert(table->nWords == 16);
    assert(!strcmp(table->array[idxTwo]->word, "boat"));

    // delete only node in a bucket
    long idxThree = hash(table, "banana");
    bool deleteThree = delete(table, "banana");
    assert(deleteThree);
    assert(table->nWords == 15);
    assert(table->array[idxThree] == NULL);

    // delete a non-existent key
    bool deleteFour = delete(table, "blablah");
    assert(!deleteFour);
    assert(table->nWords == 15);

    // delete a NULL key
    bool deleteFive = delete(table, NULL);
    assert(!deleteFive);
    assert(table->nWords == 15);

    // mix of other invalid delete args
    bool deleteSix = delete(table, "");
    bool deleteSeven = delete(NULL, "igloo");
    bool deleteEight = delete(NULL, "");
    bool deleteNine = delete(NULL, "AK47");
    
    assert((!deleteSix) && (!deleteSeven) && (!deleteEight) && (!deleteNine));
} /* test_delete() */

/* Function to test the clear function associated with the old C implementation. */
void test_clear(Table * table) {
    // NULL table
    bool clearOne = clear(NULL);
    assert(!clearOne);

    bool clearTwo = clear(table);
    assert(clearTwo);
    for (int i = 0; i < table->nBuckets; i++) {
        assert(table->array[i] == NULL);
    }

    assert(table->nWords == 0);
} /* test_clear() */