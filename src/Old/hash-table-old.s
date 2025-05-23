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

    cmpq $0x0, %rdi   # if (table == NULL)
    je hash_violation

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
    pushq %r12
    pushq %r12

    cmpq $0x0, %rdi    # if (table == NULL)
    je lookup_violation

    cmpq $0x0, %rsi   # if (word == NULL)
    je lookup_violation

    movq %rsi, %rbx    # rbx = char * word
    movq %rdi, %r12

    movq %rbx, %rdi
    xorq %rax, %rax
    call strlen

    cmpq $0, %rax       # if (strlen(word) == 0)
    je lookup_violation

    xorq %rax, %rax

    movq %r12, %rdi
    movq %rbx, %rsi

    call ASM_hash

    movq %rax, %rdx    # long hashNum = hash(table, word)

    movq %rdx, %r8
    imulq $8, %r8      # offset of 24
    addq 24(%r12), %r8
    movq (%r8), %r8    # Node * elem = table->array[hashNum]

while_lookup:
    cmpq $0x0, %r8     # if (elem != NULL)
    je break_while_lookup

    movq (%r8), %rdi   # (%r8) contains pointer to elem->word
    movq %rbx, %rsi    # rbx = char * word

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
    je lookup_violation

    movq $1, %rax     # return 1(true)
    jmp finish_lookup

lookup_violation:
    movq $0, %rax      # return 0(false)
    jmp finish_lookup

finish_lookup:
    popq %r12
    popq %r12
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_get
ASM_get:         # long ASM_get(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx

    cmpq $0x0, %rdi   # if (table == NULL)
    je get_violation

    cmpq $0x0, %rsi    # if (word == NULL)
    je get_violation

    movq %rdi, %rbx   # store table in rbx
    movq %rsi, %rdi   # move word into rdi for strlen call

    pushq %rsi
    pushq %rsi

    call strlen

    popq %rsi
    popq %rsi

    cmpq $0, %rax     # if (strlen(word) == 0)
    je get_violation

    movq %rbx, %rdi   # move table into rdi for ASM_hash call
    pushq %rsi
    pushq %rsi

    xorq %rax, %rax

    call ASM_hash

    popq %rsi
    popq %rsi

    movq %rax, %rdx   # long targetIdx = ASM_hash(table, word);

    imulq $8, %rdx
    addq 24(%rbx), %rdx
    movq (%rdx), %rdx    # Node * head = table->array[targetIdx]

while_get:
    cmpq $0x0, %rdx    # while (head != NULL)
    je get_violation

    xorq %rax, %rax

    # fine to overwrite rdi since we won't be needing it anymore
    movq (%rdx), %rdi   # (%rdx) has pointer to head->word

    pushq %rsi
    pushq %rsi
    pushq %rdx
    pushq %rdx

    call strcmp

    popq %rdx
    popq %rdx
    popq %rsi
    popq %rsi

    cmpq $0, %rax   # if (strcmp(head->word, word) == 0)
    je found_key

    movq 16(%rdx), %rdx  # head = head->next
    jmp while_get

found_key:
   movq 8(%rdx), %rax   # return head->value
   jmp finish_get

get_violation:
    movq $-1, %rax    # return -1
    jmp finish_get

finish_get:
    leave
    ret

.globl ASM_insert
ASM_insert:        # bool ASM_insert(Table * table, char * word, long value)
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx
    pushq %r10
    pushq %r12
    pushq %r13
    pushq %r13

    cmpq $0x0, %rdi    # if (table == NULL)
    je insert_violation

    cmpq $0x0, %rsi    # if (word == NULL)
    je insert_violation

    movq %rdi, %r12    # temporarily store table in r12
    movq %rsi, %rdi    # move word into rdi for strlen call

    pushq %rsi
    pushq %rdx

    call strlen

    popq %rdx
    popq %rsi

    cmpq $0, %rax    # if (strlen(word)) == 0
    je insert_violation

    cmpq $0, %rdx    # if (value < 0)
    jl insert_violation

    movq %r12, %rdi    # move table back into rdi for use again

    movq 8(%rdi), %rax  # move table->nWords to rax
    cmpq %rax, (%rdi)   # if (table->nWords > table->maxWords)
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

   jmp continue_insert_while # if both the if and else if statements fail then continue

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
    popq %r13
    popq %r12
    popq %r10
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_delete
ASM_delete:        # bool ASM_delete(Table * table, char * word)
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx
    pushq %r12
    pushq %r12
    pushq %r13
    pushq %r14

    cmpq $0x0, %rdi     # if (table == NULL)
    je delete_violation

    cmpq $0x0, %rsi    # if (word == NULL)
    je delete_violation

    movq %rdi, %r9    # temporarily store table in r9
    movq %rsi, %rdi   # move word into rdi for strlen call

    pushq %r9
    pushq %rsi

    call strlen

    popq %rsi
    popq %r9

    cmpq $0, %rax    # if strlen(word) == 0
    je delete_violation

    movq %r9, %rdi   # move table back into rdi

    pushq %rdi
    pushq %rdi
    pushq %rsi
    pushq %rsi

    xorq %rax, %rax
    call ASM_hash

    popq %rsi
    popq %rsi
    popq %rdi
    popq %rdi

    movq %rax, %rdx   # long targetIdx = ASM_hash(table, word)
    imulq $8, %rdx
    addq 24(%rdi), %rdx

    movq %rdx, %r14    # saving this here for the case when match is found and prev = NULL

    movq (%rdx), %rdx    # Node * head = table->array[targetIdx];
    movq $0x0, %rcx    # Node * prev = NULL;
    movq $0, %r8       # bool found = false;

    movq %rdi, %rbx   # rbx = table
    movq %rsi, %r12   # r12 = word

while_delete:
    cmpq $0x0, %rdx   # while (head != NULL)
    je break_while_delete

    movq (%rdx), %rdi   # (%rdx) contains pointer to head->word
    movq %r12, %rsi     # r10 = char * word

    pushq %rdx
    pushq %rdx
    pushq %rcx
    pushq %r8

    xorq %rax, %rax
    call strcmp

    popq %r8
    popq %rcx
    popq %rdx
    popq %rdx

    cmpq $0, %rax    # if (strcmp(head->word, word) == 0)
    je delete_match_found

    movq %rdx, %rcx   # prev = head
    movq 16(%rdx), %rdx  # head = head->next
    jmp while_delete

delete_match_found:
   movq $1, %r8     # found = true
   movq %rdx, %r13  # Node * temp = head

   cmpq $0x0, %rcx   # if (prev == NULL)
   je delete_prev_null

   movq 16(%rdx), %rax
   movq %rax, 16(%rcx)  # prev->next = head->next

   jmp delete_temp

delete_prev_null:
   movq 16(%rdx), %rax
   movq %rax, (%r14)   # table->array[targetIdx] = head->next;

   jmp delete_temp

delete_temp:
  movq %r13, %rdi

  pushq %r8       # we need to do this since we compare to r8 later
  pushq %r8

  call free         # free(temp);

  popq %r8
  popq %r8

  movq $0x0, %r13   # temp = NULL;
  jmp break_while_delete

break_while_delete:
   cmpq $0, %r8      # if (!found)
   je delete_violation

   subq $1, 8(%rbx)   # table->nWords--;
   movq $1, %rax      # return 1(true)
   jmp finish_delete

delete_violation:
    movq $0, %rax     # return 0(false)
    jmp finish_delete

finish_delete:
    popq %r14
    popq %r13
    popq %r12
    popq %r12
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_update
ASM_update:       # bool ASM_update(Table * table, char * word, long value);
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx
    pushq %r12
    pushq %r13

    cmpq $0x0, %rdi   # if (table == NULL)
    je update_violation

    cmpq $0x0, %rsi   # if (word == NULL)
    je update_violation

    movq %rdi, %r10  # temporarily store table in r10
    movq %rsi, %rdi  # move word into rdi for strlen call

    pushq %r10
    pushq %rsi
    pushq %rdx
    pushq %rdx

    call strlen

    popq %rdx
    popq %rdx
    popq %rsi
    popq %r10

    cmpq $0, %rax    # if (strlen(word) == 0)
    je update_violation

    cmpq $0, %rdx     # if (value < 0)
    jl update_violation

    movq %r10, %rdi   # move table back into rdi

    pushq %rdi
    pushq %rsi
    pushq %rdx
    pushq %rdx

    xorq %rax, %rax
    call ASM_hash

    popq %rdx
    popq %rdx
    popq %rsi
    popq %rdi

    imulq $8, %rax
    addq 24(%rdi), %rax

    movq %rax, %rcx      # rcx holds address of the pointer to the node
    movq (%rax), %r8     # Node * elem = table->array[hashNum]

    movq %rdi, %rbx     # rbx = table
    movq %rsi, %r12     # r12 = char * word
    movq %rdx, %r13     # r13 = long value

while_update:
    cmpq $0x0, %r8     # while (elem != NULL)
    je break_while_update

    movq (%r8), %rdi   # (%r8) contains addr to elem->word
    movq %r12, %rsi    # r12 contains char * word

    pushq %rcx
    pushq %r8

    xorq %rax, %rax
    call strcmp

    popq %r8
    popq %rcx

    cmpq $0, %rax     # if strcmp(elem->word, word) == 0
    je update_match_found

    cmpq $0x0, 16(%r8)   # else if (elem->next == NULL)
    je break_while_update

    movq 16(%r8), %r8    # elem = elem->next
    jmp while_update

update_match_found:
    movq %r13, 8(%r8)  # elem->value = value
    movq $1, %rax      # return true
    jmp finish_update

break_while_update:
    movq (%rbx), %rax    # (%rbx) has table->maxWords
    cmpq 8(%rbx), %rax   # if (table->nWords > table->maxWords)
    jl update_violation

    movq $24, %rdi    # 24 = sizeof(Node)

    pushq %rcx
    pushq %r8

    xorq %rax, %rax

    call malloc      # Node * e = (Node *) malloc(sizeof(Node))

    popq %r8
    popq %rcx

    cmpq $0x0, %rax   # if (e == NULL)
    je update_violation

    movq %r12, %rdi
    movq %rax, %r9   # Node * e = (Node *) malloc(sizeof(Node))

    pushq %rcx
    pushq %r8
    pushq %r9
    pushq %r9

    xorq %rax, %rax
    call strdup

    popq %r9
    popq %r9
    popq %r8
    popq %rcx

    movq %rax, (%r9)   # e->word = strdup(word)
    movq %r13, 8(%r9)  # e->value = value;
    movq $0x0, 16(%r9)  # e->next = NULL;

    cmpq $0x0, %r8    # if (elem == NULL)
    je update_empty_list

    movq %r9, 16(%r8)  # elem->next = e
    jmp update_end

update_empty_list:
    movq %r9, (%rcx)   # table->array[hashNum] = e;
    jmp update_end

update_end:
    addq $1, 8(%rbx)    # table->nWords++;
    jmp update_violation

update_violation:
    movq $0, %rax      # return 0(false)
    jmp finish_update

finish_update:
    popq %r13
    popq %r12
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_print
ASM_print:        # void ASM_print(Table * table)
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx

    cmpq $0x0, %rdi   # if (table != NULL)
    je finish_print

    movq %rdi, %rbx    # store table in rbx
    
    # NOTE: r10 is not a callee-saved register
    movq $0, %r10        # long i = 0;
    movq 16(%rbx), %r8   # r8 = table->nBuckets

for_print:
    cmpq %r10, %r8      # while (i < table->nBuckets)
    jle finish_print

    movq %r10, %rcx
    imulq $8, %rcx
    addq 24(%rbx), %rcx
    movq (%rcx), %rcx    # Node * head = table->array[i];

    cmpq $0x0, %rcx     # if (head != NULL)
    je continue_for_print

    movq $bucketNumber, %rdi
    movq %r10, %rsi
    addq $1, %rsi

    pushq %r8
    pushq %rcx
    pushq %r10
    pushq %r10

    xorq %rax, %rax
    call printf       # printf("Bucket %d\n", (i + 1));

    popq %r10
    popq %r10
    popq %rcx
    popq %r8

while_node_print:
    cmpq $0x0, %rcx   # while (head != NULL)
    je continue_for_print

    cmpq $0x0, 16(%rcx)  # if (head->next != NULL)
    je print_tail_node

    movq $nodeFormatOne, %rdi
    movq (%rcx), %rsi
    movq 8(%rcx), %rdx

    pushq %r8
    pushq %rcx
    pushq %r10
    pushq %r10

    xorq %rax, %rax
    call printf     # printf("Node(Key=%s, Value=%ld)->", head->word, head->value);

    popq %r10
    popq %r10
    popq %rcx
    popq %r8

    jmp continue_while_node_print

print_tail_node:
    movq $nodeFormatTwo, %rdi
    movq (%rcx), %rsi
    movq 8(%rcx), %rdx

    pushq %r8
    pushq %rcx
    pushq %r10
    pushq %r10

    xorq %rax, %rax
    call printf   # printf("Node(Key=%s, Value=%ld)\n", head->word, head->value)

    popq %r10
    popq %r10
    popq %rcx
    popq %r8

    jmp continue_while_node_print

continue_while_node_print:
    movq 16(%rcx), %rcx    # head = head->next;
    jmp while_node_print

continue_for_print:
    incq %r10       # i++;
    jmp for_print

finish_print:
    popq %rbx
    popq %rbx

    leave
    ret

.globl ASM_clear
ASM_clear:        # bool ASM_clear(Table * table)
    pushq %rbp
    movq %rsp, %rbp

    pushq %rbx
    pushq %rbx

    cmpq $0x0, %rdi   # if (table != NULL)
    je clear_violation

    movq %rdi, %rbx   # store table in rbx

    movq $0, %rsi     # long i = 0;
    movq 16(%rbx), %rdx  # rdx: table->nBuckets

for_clear:
    cmpq %rsi, %rdx   # while (i < table->nBuckets)
    jle break_for_clear

    movq %rsi, %rcx
    imulq $8, %rcx
    addq 24(%rbx), %rcx

    movq %rcx, %r8      # r8 has address of head
    movq (%rcx), %rcx   # Node * head = table->array[i]
    movq $0x0, %r9      # Node * temp = NULL;

while_clear:
   cmpq $0x0, %rcx    # while (head != NULL)
   je break_while_clear

   movq %rcx, %r9    # temp = head
   movq 16(%rcx), %rcx  # head = head->next

   movq %r9, %rdi

   pushq %rsi
   pushq %rdx
   pushq %r8
   pushq %r9
   pushq %rcx
   pushq %rcx

   call free    # free(temp);

   popq %rcx
   popq %rcx
   popq %r9
   popq %r8
   popq %rdx
   popq %rsi

   jmp while_clear

break_while_clear:
    movq $0x0, (%r8)  # table->array[i] = NULL;
    addq $1, %rsi
    jmp for_clear

break_for_clear:
    movq $0, 8(%rbx)  # table->nWords = 0;
    movq $1, %rax    # return 1(true)
    jmp finish_clear

clear_violation:
    movq $0, %rax    # return 0(false)
    jmp finish_clear

finish_clear:
    popq %rbx
    popq %rbx

    leave
    ret
    