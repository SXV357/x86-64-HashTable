#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// utility for running all tests

#define RED  "\x1b[31m"
#define RESET "\x1b[0m"

int main(int argc, char **argv) {
    // Checks for c-old-test
    if (access("./c-old-test", F_OK) == 0) {
        printf(RED "Running tests for old C implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./c-old-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: c-old-test not found");
        exit(1);
    }

    // Checks for asm-old-test
    if (access("./asm-old-test", F_OK) == 0) {
        printf(RED "Running tests for old x86-64 implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./asm-old-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: asm-old-test not found");
        exit(1);
    }

    // Checks for c-opt-test
    if (access("./c-opt-test", F_OK) == 0) {
        printf(RED "Running tests for new C implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./c-opt-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: c-opt-test not found");
        exit(1);
    }

    // Checks for asm-opt-test
    if (access("./asm-opt-test", F_OK) == 0) {
        printf(RED "Running tests for new x86-64 implementation" RESET "\n\n");
        printf("**************************************************\n");
        system("./asm-opt-test");
        printf("**************************************************\n\n");
    } else {
        perror("Error: asm-opt-test not found");
        exit(1);
    }

    return 0;
}