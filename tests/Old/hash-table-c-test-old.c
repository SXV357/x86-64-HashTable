#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../tests.h"

// For testing the old C implementation

int main(int argc, char **argv) {
    printf("Running table initialization tests...\n");
    test_init();
    printf("Table initialization tests passed...\n\n");

    // will initialize maxWords to 256 and number of buckets to 257
    printf("Running actual table initialization test...\n");

    Table * table = init(MAX_WORDS_OLD);
    assert(table);
    assert(table->maxWords == MAX_WORDS_OLD);
    assert(table->nBuckets == MAX_WORDS_OLD + 1);
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
    printf("Table clear tests passed...\n");

    return 0;
}

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
}

void test_init() {
    long maxWordsValidOne = 56;
    Table * tableOne = init(maxWordsValidOne);
    assert(tableOne);
    assert(tableOne->maxWords == maxWordsValidOne);
    assert(tableOne->nBuckets == 67);
    assert(tableOne->array);
    for (int i = 0; i < tableOne->nBuckets; i++) {
        assert(tableOne->array[i] == NULL);
    }

    long maxWordsValidTwo = 113.5;
    Table * tableTwo = init(maxWordsValidTwo);
    assert(tableTwo);
    assert(tableTwo->maxWords == 113);
    assert(tableTwo->nBuckets == 131);
    assert(tableTwo->array);
    for (int j = 0; j < tableTwo->nBuckets; j++) {
        assert(tableTwo->array[j] == NULL);
    }

    long maxWordsInvalid[3] = {-3, 200000, 0};
    for (int k = 0; k < 3; k++) {
        Table * curr = init(maxWordsInvalid[k]);
        assert(curr == NULL);
    }
}

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

    // bunch of random key-value pairs
        // random distribution + collision-creating
    char *keys[] = {"apple", "banana", "carrot", "dog", "elephant", "fish", "grape", "house", "igloo", "jazz", "vine", "xray", "nest", "yarn", "boat", "under"};
    long vals[16] = {42, 17, 98765, 1234, 555, 999999, 12, 8765, 333, 45678, 123, 456, 789, 101112, 131415, 161718};
    int n = sizeof(vals) / sizeof(long);

    for (int i = 0; i < n; i++) {
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
}

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
}

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
}

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
}

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