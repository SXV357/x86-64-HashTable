.text

.globl HashTable_ASM_hash
HashTable_ASM_hash:        # long HashTable_ASM_hash(void * table, char * word);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_lookup
HashTable_ASM_lookup:       # long HashTable_ASM_lookup(void * table, char * word, long * value);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_get
HashTable_ASM_get:         # long HashTable_C_get(void * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_insert
HashTable_ASM_insert:        # void HashTable_C_insert(void * table, char * word, long value)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_delete
HashTable_ASM_delete:        # void HashTable_C_delete(void * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_update
HashTable_ASM_update:       # long HashTable_ASM_update(void * table, char * word, long value);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_print
HashTable_ASM_print:        # void HashTable_C_print(void * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_clear
HashTable_ASM_clear:        # void HashTable_C_clear(void * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret