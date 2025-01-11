.data

.text

.globl ASM_init
ASM_init:        # Table * ASM_init()
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_hash
ASM_hash:        # long ASM_hash(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_insert
ASM_insert:        # bool ASM_insert(Table * table, char * word, long value)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_lookup
ASM_lookup:        # bool ASM_lookup(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_get
ASM_get:        # long ASM_get(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_update
ASM_update:        # bool ASM_update(Table * table, char * word, long value)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_remove
ASM_remove:        # bool ASM_remove(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_size
ASM_size:        # long ASM_size(Table * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_print
ASM_print:        # void ASM_print(Table * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_clear
ASM_clear:        # bool ASM_clear(Table * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret