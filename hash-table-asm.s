.data
primeNumbers: 
    .quad 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073
nPrimeNumbers: 
    .quad 12

.text
initErr:
    .string "error initializing the array field"

insertErrNode:
    .string "error allocating memory for a new node"

insertErrWord:
    .string "error allocating memory for the word field"

bucketNumber:
    .string "Bucket %ld\n"

nodeFormatOne:
    .string "Node(Key=%s, Value=%ld)->"

nodeFormatTwo:
    .string "Node(Key=%s, Value=%ld)\n"

.globl ASM_init
ASM_init:        # Table * ASM_init(long maxWords)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_hash
ASM_hash:        # long ASM_hash(Table * table, char * word);
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx      # push all callee-saved registers in pairs to ensure 16-byte alignment
    pushq %rbx
    pushq %r10
    pushq %r10
    pushq %r13
    pushq %r13

    cmpq $0x0, %rsi   # if (word == NULL)
    je hash_null_word

    movq %rdi, %rbx  # store table in rbx(ensure it doesn't get overwritten)
    movq %rsi, %r13  # store word in r13

    movq $1, %rdx   # long hashNum = 1

    movq %rsi, %rdi
    xorq %rax, %rax

    pushq %rdx      # push rdx 2x to ensure 16-byte alignment
    pushq %rdx

    call strlen

    popq %rdx
    popq %rdx

    movq %rax, %rcx   # long len = strlen(word);
    movq $0, %r8     # long i = 0;

while_hash:
    cmpq %r8, %rcx   # while (i < len)
    jle break_while_hash

    movb (%r13), %r9b   # load the current character into a byte register
    movzbq %r9b, %r9    # extend to full quad word so quad operations can be used

    movq %rdx, %rax
    imulq $31, %rax   # hashNum = 31 * hashNum
    addq %r9, %rax    # hashNum = 31 * hashNum + word[i]
    movq %rax, %rdx   # move the result back into hashNum

    addq $1, %r8     # i++
    addq $1, %r13    # word++
    jmp while_hash

break_while_hash:
    movq %rdx, %rax
    xorq %rdx, %rdx
    movq 16(%rbx), %r10

    idivq %r10

    movq %rdx, %rax    # we are interested in the remainder so move this
    jmp finish_hash

hash_null_word:
    movq $-1, %rax     # return -1
    jmp finish_hash

finish_hash:
    popq %r13
    popq %r13
    popq %r10
    popq %r10
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_lookup
ASM_lookup:       # bool ASM_lookup(Table * table, char * word);
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx

    cmpq $0x0, %rsi   # if (word == NULL)
    je lookup_null_word

    movq %rsi, %rbx    # store word in rbx

    xorq %rax, %rax
    call ASM_hash
    movq %rax, %rdx    # long hashNum = hash(table, word)

    movq 24(%rdi), %rcx  # rcx has address of table->array
    movq %rdx, %r8
    imulq $24, %r8      # offset of 24
    addq %rcx, %r8      # Node * elem = table->array[hashNum]

while_lookup:
    cmpq $0x0, %r8     # if (elem != NULL)
    je break_while_lookup

    movq (%r8), %rdi
    movq %rbx, %rsi
    xorq %rax, %rax

    pushq %r8
    pushq %r8

    call strcmp

    popq %r8
    popq %r8

    movq %rax, %r9     # r9 = strcmp(elem->word, word)

    cmpq $0, %r9       # if strcmp(elem->word, word) != 0
    je break_while_lookup

    movq 16(%r8), %r8   # elem = elem->next;
    jmp while_lookup

break_while_lookup:
    cmpq $0x0, %r8     # if (elem == NULL)
    je lookup_null_word

    movq $1, %rax     # return 1(true)
    jmp finish_lookup

lookup_null_word:
    movq $0, %rax      # return 0(false)
    jmp finish_lookup

finish_lookup:
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_get
ASM_get:         # long ASM_get(Table * table, char * word)
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

.globl ASM_delete
ASM_delete:        # bool ASM_delete(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret

.globl ASM_update
ASM_update:       # bool ASM_update(Table * table, char * word, long value);
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
ASM_clear:        # void ASM_clear(Table * table)
    pushq %rbp
    movq %rsp, %rbp

    leave
    ret
