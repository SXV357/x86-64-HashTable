CC = gcc
CFLAGS = -g -Wall
ASM_FLAGS = $(CFLAGS) -static # specifically for compiling ASM files

# Executables
all: c-old-test asm-old-test c-opt-test asm-opt-test

# C implementation(old)
c-old-test: src/Old/hash-table-old.c tests/Old/hash-table-c-test-old.c
	$(CC) $(CFLAGS) src/Old/hash-table-old.c tests/Old/hash-table-c-test-old.c -o c-old-test

# x86 implementation(old)
asm-old-test: src/Old/hash-table-old.s src/Old/hash-table-old.c tests/Old/hash-table-asm-test-old.c
	$(CC) $(ASM_FLAGS) src/Old/hash-table-old.s src/Old/hash-table-old.c tests/Old/hash-table-asm-test-old.c -o asm-old-test

# C implementation(new)
c-opt-test: src/New/hash-table-opt.c tests/New/hash-table-c-test-opt.c
	$(CC) $(CFLAGS) src/New/hash-table-opt.c tests/New/hash-table-c-test-opt.c -o c-opt-test

# x86 implementation(new)
asm-opt-test: src/New/hash-table-opt.s src/New/hash-table-opt.c tests/New/hash-table-asm-test-opt.c
	$(CC) $(ASM_FLAGS) src/New/hash-table-opt.s src/New/hash-table-opt.c tests/New/hash-table-asm-test-opt.c -o asm-opt-test

clean:
	rm -f c-old-test asm-old-test c-opt-test asm-opt-test