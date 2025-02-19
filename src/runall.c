/* Shreyas Viswanathan, runall.c 
 * Last updated Feb 18, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Constant definitions */

#define RED  "\x1b[31m"
#define RESET "\x1b[0m"

/* This program is a utility to run the tests associated with both the old and 
 * optimized C and x86-64 implementation of the hash table. There are a total of 4
 * tests, and this program first checks whether the corresponding executable exists
 * before running the test. If the executable doesn't exist, it throws an error and
 * aborts the program right away.
 */
int main(int argc, char **argv) {
    /* Checks for executables associated with the old implementations in C and x86-64 Assembly */

    // Old C implementation tests
    if (access("./c-old-test", F_OK) == 0) {
        printf(RED "Running tests for old C implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./c-old-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: c-old-test not found");
        exit(1);
    }

    // Old x86-64 Assembly tests
    if (access("./asm-old-test", F_OK) == 0) {
        printf(RED "Running tests for old x86-64 implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./asm-old-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: asm-old-test not found");
        exit(1);
    }

    /* Checks for executables associated with the optimized implementations in C and x86-64 Assembly */

    // Optimized C implementation tests
    if (access("./c-opt-test", F_OK) == 0) {
        printf(RED "Running tests for optimized C implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./c-opt-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: c-opt-test not found");
        exit(1);
    }

    // Optimized x86-64 Assembly implementation tests
    if (access("./asm-opt-test", F_OK) == 0) {
        printf(RED "Running tests for optimized x86-64 implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./asm-opt-test");
        printf("**************************************************\n");
    } else {
        perror("Error: asm-opt-test not found");
        exit(1);
    }

    return 0;
} /* main() */