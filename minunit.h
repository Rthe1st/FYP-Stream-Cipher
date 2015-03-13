#include <stdio.h>

typedef int (*test_case)();

int run_cases(test_case * test_cases, int num_of_test_cases);

#define mu_assert(message, test, ...)\
do {\
    if (!test){\
        printf(message, ##__VA_ARGS__);\
        return 1;\
    }\
} while (0)

extern int tests_run;