#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "hash-table-c.h"

// functions defined in the assembly implementation
extern Table * ASM_init(long maxWords);

extern bool ASM_lookup(Table * table, char * word);
extern long ASM_get(Table * table, char * word);
extern bool ASM_insert(Table * table, char * word, long value);
extern bool ASM_delete(Table * table, char * word);
extern bool ASM_update(Table * table, char * word, long value);

extern void ASM_print(Table * table);
extern void ASM_clear(Table * table);

// function forward declarations
void test_init();

void test_lookup(Table * table);
void test_insert(Table * table);
void test_update(Table * table);
void test_delete(Table * table);
void test_get(Table * table);

void test_clear(Table * table);

int main(int argc, char **argv) {
   return 0; 
}

void test_init() {

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