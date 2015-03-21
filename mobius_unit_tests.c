#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "minunit.h"
#include "mobius_cube_attack.h"
#include "cube_attack.h"
#include "useful.h"
#include "dummy_cipher.h"

int tests_run = 0;

static int test_mobius_construct_max_terms(){
    printf("testing construct max terms\n");
    Cipher_info * cipher_info = dummy_info();
    int dimension_limit = 1;
    int dimensions[10] = {0};
    uint64_t zeroed_key[1] = {0};
    cipher_info->init_clocks = 0;
    int *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, dimension_limit, zeroed_key, cipher_info);
    Max_terms_list* cube_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    mu_assert("1 clock, maxterm count != 1\n", cube_max_terms->max_term_count == 2);
    Max_term chosen_max_term = cube_max_terms->max_terms[0];
    mu_assert("1 clock, maxterm[0], num terms == %d instead of 1\n", chosen_max_term.numberOfTerms == 1, chosen_max_term.numberOfTerms);
    mu_assert("1 clock, maxterm[0], terms[0] != 0\n", chosen_max_term.terms[0] == 0);
    mu_assert("1 clock, maxterm[0], plusone != 0\n", chosen_max_term.plusOne == 0);
    dimension_limit = 3;
    dimensions[0] = 1; dimensions[1] = 2; dimensions[2] = 3;
    dimensions[0] = 1; dimensions[1] = 2; dimensions[2] = 3;
    cipher_info->init_clocks = 5;
    mu_assert("6 clocks, max term count != 3\n", cube_max_terms->max_term_count == 8);
    chosen_max_term = cube_max_terms->max_terms[7];
    mu_assert("failed for 6 init clocks results[7](111) num terms != 1\n", chosen_max_term.numberOfTerms == 1);
    mu_assert("failed for 6 init clocks results[7](111) term 1 != 0\n",chosen_max_term.terms[0] == 0);
    chosen_max_term = cube_max_terms->max_terms[6];
    mu_assert("failed for 6 init clocks results[6](110) num of terms != 1\n", chosen_max_term.numberOfTerms == 1);
    mu_assert("failed for 6 init clocks results[6](110) term[0] != 3\n",chosen_max_term.terms[0] == 3);
    chosen_max_term = cube_max_terms->max_terms[3];
    mu_assert("failed for 6 init clocks results[3](11) num terms != 0\n", chosen_max_term.numberOfTerms == 0);
    printf("done testing onstruct max terms\n");
    return 0;
}

static int test_mobius_transform(){
    printf("testing mobius transform\n");
    Cipher_info * cipher_info = dummy_info();
    cipher_info->init_clocks = 0;
    int axis[10] = {0};
    axis[0] = 0;
    int num_of_axis = 1;
    uint64_t key[1] = {0};
    int* results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("superpoly results[0] != 0 && results[1] != 0 for key {0} clocks 1, axis {0}\n", results[0] == 0 && results[1] == 0);
    free(results);
    cipher_info->init_clocks = 5;
    key[0] = power(2, 5)-1;
    num_of_axis = 1;
    axis[0] = 3;
    results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("superpoly results[1] == 1 for key {power(2, 5)-1} clocks 6, axis {3}\n", results[1] == 1);
    free(results);
    cipher_info->init_clocks = 5;
    num_of_axis = 3;
    axis[0] = 0; axis[1] = 1; axis[2] = 2;
    key[0] = power(2, 1) + power(2, 3);
    results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("failed for 6 init clocks results[7](111)== 1\n", results[7]== 1);
    mu_assert("failed for 6 init clocks results[6](110)== 1\n", results[6]== 1);
    mu_assert("failed for 6 init clocks results[3](11)== 0\n", results[3]== 0);
    printf("done testing mobius transform\n");
    return 0;
}

int main(int argc, char **argv) {
    test_case test_cases[2] = {test_mobius_transform, test_mobius_construct_max_terms};
    run_cases(test_cases, (sizeof test_cases/ sizeof(test_case)));
}