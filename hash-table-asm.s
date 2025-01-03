.text

.globl ASM_init
ASM_init:        # Table * ASM_init(long maxWords)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_hash
ASM_hash:        # long ASM_hash(void * table, char * word);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_lookup
ASM_lookup:       # long ASM_lookup(void * table, char * word, long * value);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_get
ASM_get:         # long ASM_get(void * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_insert
ASM_insert:        # void ASM_insert(void * table, char * word, long value)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_delete
ASM_delete:        # void ASM_delete(void * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_update
ASM_update:       # long ASM_update(void * table, char * word, long value);
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_print
ASM_print:        # void ASM_print(void * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_clear
ASM_clear:        # void ASM_clear(void * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret