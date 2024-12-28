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

.globl HashTable_ASM_update
HashTable_ASM_update:       # long HashTable_ASM_update(void * table, char * word, long value);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_delete
HashTable_ASM_delete:
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl HashTable_ASM_print
HashTable_ASM_print:
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret