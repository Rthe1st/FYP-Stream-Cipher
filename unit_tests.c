#include <stdio.h>
#include "minunit.h"
#include "grain.h"

int tests_run = 0;

//key:        00000000000000000000000000000000
//IV :        000000000000000000000000
//keystream:  f09b7bf7d7f6b5c2de2ffc73ac21397f

//key:        0123456789abcdef123456789abcdef0
//IV :        0123456789abcdef12345678
//keystream:  afb5babfa8de896b4b9c6acaf7c4fbfd

static char * test_foo() {
    int foo = 1;
    mu_assert("error, foo != 7", foo == 7);
    return 0;
}

static

static char * all_tests() {
    mu_run_test(test_foo);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}