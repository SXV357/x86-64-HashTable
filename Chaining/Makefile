CC = gcc
CFLAGS = -g -Wall
ASM_FLAGS = $(CFLAGS) -static # specifically for compiling ASM files

# Executables
all: c-old-test asm-old-test c-opt-test asm-opt-test

# C implementation(old)
c-old-test: src/hash-table-c-chain-old.c tests/hash-table-c-chain-test.c
	$(CC) $(CFLAGS) src/hash-table-c-chain-old.c tests/hash-table-c-chain-test.c -o c-old-test

# x86 implementation(old)
asm-old-test: src/hash-table-asm-chain-old.s src/hash-table-c-chain-old.c tests/hash-table-asm-chain-test.c
	$(CC) $(ASM_FLAGS) src/hash-table-asm-chain-old.s src/hash-table-c-chain-old.c tests/hash-table-asm-chain-test.c -o asm-old-test

# C implementation(new)
c-opt-test: src/hash-table-c-chain-opt.c tests/hash-table-c-chain-test.c
	$(CC) $(CFLAGS) src/hash-table-c-chain-opt.c tests/hash-table-c-chain-test.c -o c-opt-test

# x86 implementation(new)
asm-opt-test: src/hash-table-asm-chain-opt.s src/hash-table-c-chain-opt.c tests/hash-table-asm-chain-test.c
	$(CC) $(ASM_FLAGS) src/hash-table-asm-chain-opt.s src/hash-table-c-chain-opt.c tests/hash-table-asm-chain-test.c -o asm-opt-test

clean:
	rm -f c-old-test asm-old-test c-opt-test asm-opt-test