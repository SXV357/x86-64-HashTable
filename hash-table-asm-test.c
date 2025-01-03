#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

extern long ASM_hash(void * table, char * word);

extern bool ASM_lookup(void * table, char * word, long * value);
extern long ASM_get(void * table, char * word);
extern bool ASM_insert(void * table, char * word, long value);
extern bool ASM_delete(void * table, char * word);
extern bool ASM_update(void * table, char * word, long value);

extern void ASM_print(void * table);
extern void ASM_clear(void * table);

int main(int argc, char **argv) {
   return 0; 
}