CC = gcc
CFLAGS = -g -Wall
ASM_FLAGS = $(CFLAGS) -static # specifically for compiling ASM files

# Executables
all: c-old-test asm-old-test c-opt-test asm-opt-test

# C implementation(old)
c-old-test: src/old/hash-table-c-chain-old.c tests/old/hash-table-c-chain-test-old.c
	$(CC) $(CFLAGS) src/old/hash-table-c-chain-old.c tests/old/hash-table-c-chain-test-old.c -o c-old-test

# x86 implementation(old)
asm-old-test: src/old/hash-table-asm-chain-old.s src/old/hash-table-c-chain-old.c tests/old/hash-table-asm-chain-test-old.c
	$(CC) $(ASM_FLAGS) src/old/hash-table-asm-chain-old.s src/old/hash-table-c-chain-old.c tests/old/hash-table-asm-chain-test-old.c -o asm-old-test

# C implementation(new)
c-opt-test: src/new/hash-table-c-chain-opt.c tests/new/hash-table-c-chain-test-new.c
	$(CC) $(CFLAGS) src/new/hash-table-c-chain-opt.c tests/new/hash-table-c-chain-test-new.c -o c-opt-test

# x86 implementation(new)
asm-opt-test: src/new/hash-table-asm-chain-opt.s src/new/hash-table-c-chain-opt.c tests/new/hash-table-asm-chain-test-new.c
	$(CC) $(ASM_FLAGS) src/new/hash-table-asm-chain-opt.s src/new/hash-table-c-chain-opt.c tests/new/hash-table-asm-chain-test-new.c -o asm-opt-test

clean:
	rm -f c-old-test asm-old-test c-opt-test asm-opt-test