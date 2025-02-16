#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../../src/hash-table.h"
#include "../../src/Utils/str.h"
#include "../tests.h"

// For testing the new C implementation

int main(int argc, char **argv) {
    printf("Running table initialization tests...\n");
    test_init();
    printf("Table initialization tests passed...\n\n");

    // initialize number of buckets to 256 with 255 maxWords
    printf("Running actual table initialization test...\n");

    Table * table = init(MAX_WORDS_NEW);
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
    
    printf("Running table insertion tests...\n");
    test_insert(table);
    printf("Table insertion tests passed...\n\n");

    printf("Running table lookup tests...\n");
    test_lookup(table);
    printf("Table lookup tests passed...\n\n");
    
    printf("Running table update tests...\n");
    test_update(table);
    printf("Table update tests passed...\n\n");
    
    printf("Running table get tests...\n");
    test_get(table);
    printf("Table get tests passed...\n\n");

    printf("Running table delete tests...\n");
    test_delete(table);
    printf("Table delete tests passed...\n\n");

    printf("Running table clear tests...\n");
    test_clear(table);
    printf("Table clear tests passed...\n\n");

    return 0;
}

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
    long indices[4] = {113, 77, 178, 237}; // UPDATE 2

    for (int i = 0; i < 4; i++) {
        assert(hash(table, words[i]) == indices[i]);
    }
}

void test_init() {
    long maxWordsValidOne = 56;
    Table * tableOne = init(maxWordsValidOne);
    assert(tableOne);
    assert(tableOne->maxWords == maxWordsValidOne);
    assert(tableOne->nBuckets == 64); // UPDATE 3
    assert(tableOne->array);
    for (int i = 0; i < tableOne->nBuckets; i++) {
        assert(tableOne->array[i] == NULL);
    }

    long maxWordsValidTwo = 113.5;
    Table * tableTwo = init(maxWordsValidTwo);
    assert(tableTwo);
    assert(tableTwo->maxWords == 113);
    assert(tableTwo->nBuckets == 128); // UPDATE 4
    assert(tableTwo->array);
    for (int j = 0; j < tableTwo->nBuckets; j++) {
        assert(tableTwo->array[j] == NULL);
    }

    long maxWordsInvalid[3] = {-3, 200000, 0};
    for (int i = 0; i < 3; i++) {
        Table * curr = init(maxWordsInvalid[i]);
        assert(curr == NULL);
    }
}

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

    // bunch of random key-value pairs
        // random distribution + collision-creating
    char *keys[] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
    long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};
    int n = sizeof(vals) / sizeof(long);

    for (int i = 0; i < n; i++) {
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
}

void test_update(Table * table) {
    // update the value of an existing key
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
}

void test_get(Table * table) {
    // valid key
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
}

void test_delete(Table * table) {    
    // delete head in a bucket with > 1 node
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
}

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
}