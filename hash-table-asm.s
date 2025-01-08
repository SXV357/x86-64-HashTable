.data
primeNumbers:
    .quad 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131073
nPrimeNumbers:
    .quad 12

.text
initTableErr:
  .string "(init) error allocating memory for the table"

initArrayErr:
  .string "(init) error allocating memory for table->array"

insertErrNode:
    .string "(insert) error allocating memory for a new node"

insertErrWord:
    .string "(insert) error allocating memory for the word field"

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

    pushq %rbx
    pushq %rbx

    cmpq $0, %rdi    # if (maxWords <= 0)
    jle init_violation

    movq nPrimeNumbers, %rax
    subq $1, %rax             # nPrimeNumbers - 1
    imulq $8, %rax
    addq $primeNumbers, %rax   # rax has address of primeNumbers[nPrimeNumbers - 1]

    movq (%rax), %rsi     # rsi = primeNumbers[nPrimeNumbers - 1]

    cmpq %rsi, %rdi     # if (maxWords > primeNumbers[nPrimeNumbers - 1])
    jg init_violation

    movq %rdi, %rdx     # rdx = maxWords(temporarily store maxWords here)

    movq $32, %rdi     # sizeof(Table) = 32
    xorq %rax, %rax

    pushq %rsi       # preserve maxWords and primeNumbers[nPrimeNumbers - 1]
    pushq %rsi
    pushq %rdx
    pushq %rdx

    call malloc

    popq %rdx
    popq %rdx
    popq %rsi
    popq %rsi

    movq %rax, %rcx    # Table * table = (Table *) malloc(sizeof(Table))

    cmpq $0x0, %rcx     # if (table == NULL)
    je init_table_err

    movq %rsi, 16(%rcx)  # table->nBuckets = primeNumbers[nPrimeNumbers - 1]
    movq $0, %r8         # long i = 0;

init_loop:
   cmpq %r8, nPrimeNumbers   # while (i < nPrimeNumbers)
   jle break_init_loop

   movq %r8, %r9
   imulq $8, %r9
   addq $primeNumbers, %r9   # r9 has addr of primeNumbers[i]

   cmpq %rdx, (%r9)    # if (maxWords < primeNumbers[i])
   jle continue_init_loop

   movq (%r9), %rax
   movq %rax, 16(%rcx)   # table->nBuckets = primeNumbers[i]
   jmp break_init_loop

continue_init_loop:
  addq $1, %r8          # i++;
  jmp init_loop

break_init_loop:
  movq %rdx, (%rcx)      # table->maxWords = maxWords;
  movq $0, 8(%rcx)       # table->nWords = 0;

  movq 16(%rcx), %rax
  imulq $8, %rax      # rax = table->nBuckets * sizeof(Node *)
  movq %rax, %rdi

  pushq %rcx          # preserve table(only need this going forward)
  pushq %rcx

  call malloc

  popq %rcx
  popq %rcx

  movq %rax, %rbx
  movq %rbx, 24(%rcx) # table->array = (Node **) malloc(table->nBuckets * sizeof(Node *))

  cmpq $0x0, 24(%rcx)   # if (table->array == NULL)
  je init_array_err

  movq $0, %r8          # long i = 0;

init_loop_two:
  cmpq %r8, 16(%rcx)   # while (i < table->nBuckets)
  jle ret_table

  movq %r8, %r9
  imulq $8, %r9
  addq 24(%rcx), %r9   # r9 has addr of table->array[i]

  movq $0x0, (%r9)    # table->array[i] = NULL;
  addq $1, %r8
  jmp init_loop_two

init_table_err:
   movq $initTableErr, %rdi
   call perror      # perror("(init) error allocating memory for the table")
   jmp init_violation

init_array_err:
    movq $initArrayErr, %rdi
    call perror     # perror("(init) error allocating memory for table->array")
    jmp init_violation

init_violation:
    movq $0x0, %rax     # return NULL
    jmp finish_init

ret_table:
    movq %rcx, %rax     # return table;
    jmp finish_init

finish_init:
    popq %rbx
    popq %rbx

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
    je hash_violation

    movq %rdi, %rbx  # store table in rbx(ensure it doesn't get overwritten)
    movq %rsi, %r13  # store word in r13

    movq %r13, %rdi
    call strlen
    cmpq $0, %rax    # if (strlen(word) == 0)
    je hash_violation

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

hash_violation:
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

    pushq %rbx
    pushq %rbx
    pushq %r10
    pushq %r13

    movq 8(%rdi), %rax  # move table->nWords to rax
    cmpq %rax, (%rdi)   # if (table->nWords > table->maxWords)
    jl insert_violation

    cmpq $0x0, %rsi    # if (word == NULL)
    je insert_violation

    cmpq $0, %rdx      # if (value < 0)
    jl insert_violation

    pushq %rdi       # need the original values of rdi, rsi and rdx after func call
    pushq %rsi
    pushq %rdx
    pushq %rdx

    call ASM_hash     # ASM_hash(table, word)

    popq %rdx
    popq %rdx
    popq %rsi
    popq %rdi

    movq %rax, %rcx    # long targetIdx = ASM_hash(table, word)

    movq %rdi, %rbx   # rbx = Table * table
    movq %rsi, %r10   # r10 = char * word
    movq %rdx, %r13   # r13 = long value

    movq $24, %rdi    # 24 = sizeof(Node)
    xorq %rax, %rax

    # all params moved to callee-saved registers and only thing that needs to be
    # preserved is targetIdx so push that before making the malloc call

    pushq %rcx
    pushq %rcx

    call malloc     # malloc(sizeof(Node))

    popq %rcx
    popq %rcx

    # it's fine that rdx is being overridden here because what was orig in rdx
    # i.e. long value has been moved to r13

    movq %rax, %rdx   # Node *new = (Node *) malloc(sizeof(Node))
    cmpq $0x0, %rdx   # if (new == NULL)
    je insert_err_node

    movq %r10, %rdi    # r10 has char * word
    xorq %rax, %rax

    # table, word and value all in callee-saved registers
    # targetIdx and node to be inserted in caller-saved so push them

    pushq %rcx
    pushq %rcx
    pushq %rdx
    pushq %rdx

    call strlen      # strlen(word)

    popq %rdx
    popq %rdx
    popq %rcx
    popq %rcx

    movq %rax, %r8   # r8 = strlen(word)

    movq %r8, %rax
    addq $1, %rax
    movq %rax, %rdi  # rdi = strlen(word) + 1

    # need to preserve word length, node to insert and targetIdx

    pushq %r8
    pushq %r8
    pushq %rcx
    pushq %rcx
    pushq %rdx
    pushq %rdx

    call malloc

    popq %rdx
    popq %rdx
    popq %rcx
    popq %rcx
    popq %r8
    popq %r8

    movq %rax, (%rdx)  # new->word = malloc(strlen(word) + 1)
    cmpq $0x0, (%rdx)  # if (new->word == NULL)
    je insert_err_word

    movq (%rdx), %rdi  # (%rdx) = new->word
    movq %r10, %rsi    # r10 = char * word

    # strlen(word) will be used in next statement so needs to be preserved
    # so will targetIdx and node in subsequent statements

    pushq %r8
    pushq %r8
    pushq %rcx
    pushq %rcx
    pushq %rdx
    pushq %rdx

    call strcpy      # strcpy(new->word, word);

    popq %rdx
    popq %rdx
    popq %rcx
    popq %rcx
    popq %r8
    popq %r8

    addq (%rdx), %r8  # r8 now stores addr of new->word[strlen(word)]
    movb (%r8), %al
    movb $0, %al   # new->word[strlen(word)] = '\0';

    movq %r13, 8(%rdx)  # new->value = value;
    movq $0x0, 16(%rdx) # new->next = NULL;

    movq %rcx, %rax    # move targetIdx to rax
    imulq $8, %rax     # targetIdx *= 8(each el in the array is a Node *)
    addq 24(%rbx), %rax 

    # overwriting rcx is fine here since we won't need it anymore
    # 24(%rbx) is essentially ** array so at the end of last statement,
    # rax doesnt have addr of table->array[targetIndex]. if we want the pointer
    # at this index, we need to dereference it

    movq (%rax), %rcx    # Node * head = table->array[targetIdx]

    cmpq $0x0, %rcx    # if (head == NULL)
    je insert_list_empty

insert_while:
   cmpq $0x0, %rcx     # while (head != NULL)
   je break_insert_while

   movq (%rcx), %rdi  # (%rcx) = temp->word
   movq %r10, %rsi   # r10 = char * word

   # the only thing we need for future statements is head so we preserve it

   pushq %rcx
   pushq %rcx
   pushq %rdx
   pushq %rdx

   call strcmp

   popq %rdx
   popq %rdx
   popq %rcx
   popq %rcx

   cmpq $0, %rax     # if (strcmp(head->word, word) == 0)
   je insert_match_found

   cmpq $0x0, 16(%rcx)  # else if (head->next == NULL)
   je break_insert_while

   # if both the if and else if statements fail then continue
   jmp continue_insert_while

insert_match_found:
   cmpq 8(%rcx), %r13  # if (head->value == value)
   je insert_violation

   movq %r13, 8(%rcx)   # head->value = value
   jmp insert_succ_no_new_word

continue_insert_while:
   movq 16(%rcx), %rcx   # head = head->next;
   jmp insert_while

break_insert_while:
   movq %rdx, 16(%rcx)  # head->next = new;
   jmp insert_done

insert_list_empty:
   # doing movq %rdx, %rcx is wrong(saying: head = new) - doesn't update anything
    # but just does so in place and won't update the actual list
   # doing movq %rdx, (%rcx) will throw seg fault since %rcx is alr 0x0

   movq %rdx, (%rax)   # table->array[targetIdx] = new;
   jmp insert_done

insert_err_word:
   movq $insertErrWord, %rdi
   call perror         # perror("(insert) error allocating memory for the word field")
   jmp insert_violation

insert_err_node:
   movq $insertErrNode, %rdi
   call perror         # perror("(insert) error allocating memory for a new node")
   jmp insert_violation

insert_violation:
    movq $0, %rax     # return 0(false)
    jmp finish_insert

insert_done:
   addq $1, 8(%rbx)   # table->nWords++;
   movq $1, %rax     # return 1(true)
   jmp finish_insert

insert_succ_no_new_word:
    movq $1, %rax     # return 1(true)
    jmp finish_insert

finish_insert:
    popq %r13
    popq %r10
    popq %rbx
    popq %rbx

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
