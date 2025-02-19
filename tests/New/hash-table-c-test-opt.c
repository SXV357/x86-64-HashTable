/* Shreyas Viswanathan, hash-table-c-test-opt.c 
 * Last updated Feb 18, 2025
 */

#include "../../src/Utils/str.h"
#include "../tests.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Driver function that runs all tests associated with the optimized C hash table implementation. */
int main(int argc, char **argv) {
    printf(TABLE_INIT_TEST_START);
    test_init();
    printf(TABLE_INIT_TEST_END);

    printf(TABLE_INIT_ACC_START);

    Table * table = init(MAX_WORDS_NEW);
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

    return 0;
} /* main() */

/* Function to test the hash function associated with the optimized C implementation. */
void test_hash(Table * table) {
    // invalid values
    assert(hash(table, NULL) == -1);
    assert(hash(table, "") == -1);

    char *curr = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(curr);
    my_str_cpy(curr, "something");

    assert(hash(NULL, curr) == -1);
    assert(hash(NULL, "") == -1);
    assert(hash(NULL, NULL) == -1);

    // general cases
    char *w1 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w1);
    my_str_cpy(w1, "hello");

    char *w2 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w2);
    my_str_cpy(w2, "bye");

    char *w3 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w3);
    my_str_cpy(w3, "see");

    char *w4 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w4);
    my_str_cpy(w4, "flag");
    
    char *words[] = {w1, w2, w3, w4};
    long indices[4] = {113, 77, 178, 237};

    for (int i = 0; i < 4; i++) {
        assert(hash(table, words[i]) == indices[i]);
    }
} /* test_hash() */

/* Function to test the init function associated with the optimized C implementation. */
void test_init() {
    // valid cases
    long maxWordsValidOne = 56;
    Table * tableOne = init(maxWordsValidOne);
    assert(tableOne);
    assert(tableOne->maxWords == maxWordsValidOne);
    assert(tableOne->nBuckets == 64);
    assert(tableOne->array);
    for (int i = 0; i < tableOne->nBuckets; i++) {
        assert(tableOne->array[i] == NULL);
    }

    long maxWordsValidTwo = 113.5;
    Table * tableTwo = init(maxWordsValidTwo);
    assert(tableTwo);
    assert(tableTwo->maxWords == 113);
    assert(tableTwo->nBuckets == 128);
    assert(tableTwo->array);
    for (int j = 0; j < tableTwo->nBuckets; j++) {
        assert(tableTwo->array[j] == NULL);
    }

    // invalid cases
    long maxWordsInvalid[3] = {-3, 200000, 0};
    for (int i = 0; i < 3; i++) {
        Table * curr = init(maxWordsInvalid[i]);
        assert(curr == NULL);
    }
} /* test_init() */

/* Function to test the insert function associated with the optimized C implementation. */
void test_insert(Table * table) {
    // brand new key-value pair
    char *keyOne = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(keyOne);
    my_str_cpy(keyOne, "Hello");

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

    for (int i = 0; i < N; i++) {
        char *curr = calloc(MAX_KEY_SIZE, sizeof(char));
        assert(curr);
        my_str_cpy(curr, keys[i]);

        bool insertCurr = insert(table, curr, vals[i]);
        assert(insertCurr);
    }

    assert(table->nWords == 17);

    // key is NULL
    bool insertInvalidOne = insert(table, NULL, 15);
    assert(!insertInvalidOne);
    assert(table->nWords == 17);

    // value is less than 0
    char *w1 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w1);
    my_str_cpy(w1, "hello");

    bool insertInvalidTwo = insert(table, w1, -57);
    assert(!insertInvalidTwo);
    assert(table->nWords == 17);

    // mix of invalid insert arguments
    char *w2, *w3;
    w2 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w2);
    my_str_cpy(w2, "");

    w3 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w3);
    my_str_cpy(w3, "something");

    bool insertInvalidThree = insert(table, w2, 15);
    bool insertInvalidFour = insert(table, w2, -10);
    bool insertInvalidFive = insert(table, NULL, -110);
    bool insertInvalidSix = insert(NULL, w3, 42);
    bool insertInvalidSeven = insert(NULL, w2, 32);
    bool insertInvalidEight = insert(NULL, w2, -23);
    bool insertInvalidNine = insert(NULL, NULL, 94);
    bool insertInvalidTen = insert(NULL, NULL, -5);

    assert((!insertInvalidThree) && (!insertInvalidFour) && (!insertInvalidFive) && (!insertInvalidSix) && (!insertInvalidSeven) && (!insertInvalidEight) && (!insertInvalidNine) && (!insertInvalidTen));
} /* test_insert() */

/* Function to test the lookup function associated with the optimized C implementation. */
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

    // existent key(only node in the bucket)
    bool lookupOne = lookup(table, w1);
    assert(lookupOne);

    // non-existent key
    bool lookupTwo = lookup(table, w2);
    assert(!lookupTwo);

    // invalid values
    bool lookupThree = lookup(table, NULL);
    bool lookupFour = lookup(table, w3);
    bool lookupFive = lookup(NULL, w1);
    bool lookupSix = lookup(NULL, w2);
    bool lookupSeven = lookup(NULL, NULL);

    // test to check if after a successful lookup the element is moved to the front of chain
    long idx = hash(table, w4);
    bool lookupEight = lookup(table, w4);
    assert(lookupEight);
    
    assert(!my_str_cmp_opt(table->array[idx]->word, w4));
    assert(!my_str_cmp_opt(table->array[idx]->next->word, w5));
    assert(table->array[idx]->next->next == NULL);

    assert((!lookupThree) && (!lookupFour) && (!lookupFive) && (!lookupSix) && (!lookupSeven));
} /* test_lookup() */

/* Function to test the update function associated with the optimized C implementation. */
void test_update(Table * table) {
    char *w1, *w2, *w3, *w4, *w5, *w6;

    w1 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w1);
    my_str_cpy(w1, "under");

    w2 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w2);
    my_str_cpy(w2, "formula");

    w3 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w3);
    my_str_cpy(w3, "elephant");

    w4 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w4);
    my_str_cpy(w4, "");

    w5 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w5);
    my_str_cpy(w5, "blabla");

    w6 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w6);
    my_str_cpy(w6, "ajdhjkfe");

    // update value associated with an existing key
    long newValOne = 192021;
    bool updateOne = update(table, w1, newValOne);
    assert(updateOne);
    assert(table->nWords == 17);

    // update the value of a non-existent key
    long newValTwo = 489321;
    bool updateTwo = update(table, w2, newValTwo);
    assert(!updateTwo);
    assert(table->nWords == 18);

    // invalid values
    bool updateThree = update(table, NULL, 45);
    assert(!updateThree);

    bool updateFour = update(table, w3, -100);
    assert(!updateFour);

    // mix of other invalid update args
    bool updateFive = update(table, w4, 68);
    bool updateSix = update(table, w4, -76);
    bool updateSeven = update(table, NULL, -43);
    bool updateEight = update(NULL, w4, 90);
    bool updateNine = update(NULL, w4, -36);
    bool updateTen = update(NULL, w5, 11);
    bool updateEleven = update(NULL, w6, 34);
    bool updateTwelve = update(NULL, NULL, 1111);
    bool updateThirteen = update(NULL, NULL, -765);

    assert((!updateFive) && (!updateSix) && (!updateSeven) && (!updateEight) && (!updateNine) && (!updateTen) && (!updateEleven) && (!updateTwelve) && (!updateThirteen));
} /* test_update() */

/* Function to test the get function associated with the optimized C implementation. */
void test_get(Table * table) {
    char *w1, *w2, *w3;

    w1 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w1);
    my_str_cpy(w1, "carrot");

    w2 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w2);
    my_str_cpy(w2, "blablah");

    w3 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w3);
    my_str_cpy(w3, "");

    // existent key
    long getOne = get(table, w1);
    assert(getOne == 98765);

    // non-existent valid key
    long getTwo = get(table, w2);
    assert(getTwo == -1);

    // invalid values
    long getThree = get(table, NULL);
    long getFour = get(table, w3);
    long getFive = get(NULL, w1);
    long getSix = get(NULL, w2);
    long getSeven = get(NULL, NULL);

    assert(getThree + getFour + getFive + getSix + getSeven == -5);
} /* test_get() */

/* Function to test the delete function associated with the optimized C implementation. */
void test_delete(Table * table) {    
    char *w1, *w2, *w3, *w4, *w5, *w6, *w7;

    w1 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w1);
    my_str_cpy(w1, "fish");

    w2 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w2);
    my_str_cpy(w2, "nest");

    w3 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w3);
    my_str_cpy(w3, "banana");

    w4 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w4);
    my_str_cpy(w4, "blablah");

    w5 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w5);
    my_str_cpy(w5, "");

    w6 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w6);
    my_str_cpy(w6, "igloo");

    w7 = calloc(MAX_KEY_SIZE, sizeof(char));
    assert(w7);
    my_str_cpy(w7, "AK47");

    // delete existing key in a bucket with > 1 node
    long idxOne = hash(table, w1);
    bool deleteOne = delete(table, w1);
    assert(deleteOne);
    assert(table->nWords == 17);
    assert(!my_str_cmp_opt(table->array[idxOne]->word, w2));

    // delete only node in a bucket
    long idxThree = hash(table, w3);
    bool deleteThree = delete(table, w3);
    assert(deleteThree);
    assert(table->nWords == 16);
    assert(table->array[idxThree] == NULL);

    // delete a non-existent key
    bool deleteFour = delete(table, w4);
    assert(!deleteFour);
    assert(table->nWords == 16);

    // delete a NULL key
    bool deleteFive = delete(table, NULL);
    assert(!deleteFive);
    assert(table->nWords == 16);

    // mix of other invalid delete args
    bool deleteSix = delete(table, w7);
    bool deleteSeven = delete(NULL, w5);
    bool deleteEight = delete(NULL, w7);
    bool deleteNine = delete(NULL, w6);
    
    assert((!deleteSix) && (!deleteSeven) && (!deleteEight) && (!deleteNine));
} /* test_delete() */

/* Function to test the clear function associated with the optimized C implementation. */
void test_clear(Table * table) {
    // NULL table
    bool clearOne = clear(NULL);
    assert(!clearOne);

    // non-NULL table
    bool clearTwo = clear(table);
    assert(clearTwo);
    for (int i = 0; i < table->nBuckets; i++) {
        assert(table->array[i] == NULL);
    }

    assert(table->nWords == 0);
} /* test_clear() */