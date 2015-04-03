#include <stdlib.h>

#include "minunit.h"
#include "cipher_unit_tests.h"
#include "cube_attack_unit_tests.h"
#include "mobius_unit_tests.h"

int main(int argc, char **argv) {
    if(run_cipher_unit_tests()){
        return EXIT_FAILURE;
    }
    if(run_cube_attack_unit_tests()){
        return EXIT_FAILURE;
    }
    if(run_mobius_cube_attack_unit_tests()){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}