#include <stdlib.h>
#include "minunit.h"

int run_cases(test_case * test_cases, int num_of_test_cases) {
    for(int i=0;i<num_of_test_cases;i++){
        if(test_cases[i]()){
            printf("Tests run: %d\n", i);
            return EXIT_FAILURE;
        }
    }
    printf("ALL TESTS PASSED\n");
    return EXIT_SUCCESS;
}