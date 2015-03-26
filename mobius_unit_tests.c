#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "minunit.h"
#include "mobius_cube_attack.h"
#include "cube_attack.h"
#include "useful.h"
#include "dummy_cipher.h"

int tests_run = 0;

static int test_mobius_is_super_poly_linear(){
    srand((unsigned int) time(NULL));
    printf("testing mobius_is_super_poly_linear\n");
    Cipher_info * cipher_info = dummy_info();
    int dimension_limit = 1;
    int dimensions[10] = {0};
    uint64_t zeroed_key[1] = {0};
    cipher_info->init_clocks = 0;
    uint64_t *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, 0, zeroed_key, cipher_info);
    uint64_t * linear_results = mobius_is_super_poly_linear(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    int* expected_results = malloc(sizeof(int)*2);
    expected_results[0] = 0;
    expected_results[1] = 1;
    for(int i=0; i<power(2, dimension_limit); i++){
        mu_assert("clock 1, linear result[%d] == %d, expected %d\n", get_bit(linear_results, i) == expected_results[i], i, get_bit(linear_results, i), expected_results[i]);
    }
    free(linear_results);
    free(zeroed_key_poly);
    free(expected_results);
    dimension_limit = 4;
    dimensions[0] = 0; dimensions[1] = 1; dimensions[2] = 2; dimensions[3] = 3;
    cipher_info->init_clocks = 5;
    zeroed_key_poly = mobius_transform(dimensions, dimension_limit, 0, zeroed_key, cipher_info);
    linear_results = mobius_is_super_poly_linear(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    expected_results = malloc(sizeof(int)*16);
    expected_results[0] = 0;
    expected_results[1] = 0;
    expected_results[2] = 0;
    expected_results[3] = 0;
    expected_results[4] = 0;
    expected_results[5] = 1;
    expected_results[6] = 1;
    expected_results[7] = 1;
    expected_results[8] = 1;
    for(int i=9;i<16;i++){
        expected_results[i] = 1;
    }
    for(int i=0; i<power(2, dimension_limit); i++){
        mu_assert("clock 6, linear result[%d] == %d, expected %d\n", get_bit(linear_results, i) == expected_results[i], i, get_bit(linear_results, i), expected_results[i]);
    }
    free(linear_results);
    free(zeroed_key_poly);
    printf("done testing mobius_is_super_poly_linear\n");
    return 0;
};

static int test_mobius_construct_max_terms(){
    printf("testing construct max terms\n");
    Cipher_info * cipher_info = dummy_info();
    int dimension_limit = 1;
    int dimensions[10] = {0};
    uint64_t zeroed_key[1] = {0};
    cipher_info->init_clocks = 0;
    uint64_t *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, 0, zeroed_key, cipher_info);
    Max_terms_list* cube_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    mu_assert("1 clock, maxterm count of %d != 2\n", cube_max_terms->max_term_count == 1, cube_max_terms->max_term_count);
    Max_term* chosen_max_term = cube_max_terms->max_terms[0];
    mu_assert("1 clock, maxterm[0], num terms == %d instead of 1\n", chosen_max_term->numberOfTerms == 1, chosen_max_term->numberOfTerms);
    mu_assert("1 clock, maxterm[0], terms[0] == %d, not 3\n", chosen_max_term->terms[0] == 0, chosen_max_term->terms[0]);
    mu_assert("1 clock, maxterm[0], plusone != 0\n", chosen_max_term->plusOne == 0);
    free(zeroed_key_poly);
    for(int i=0;i<cube_max_terms->max_term_count;i++){
        free_max_term(cube_max_terms->max_terms[i]);
    }
    free(cube_max_terms);
    dimension_limit = 3;
    dimensions[0] = 0; dimensions[1] = 1; dimensions[2] = 2;
    cipher_info->init_clocks = 5;
    zeroed_key_poly = mobius_transform(dimensions, dimension_limit, 0, zeroed_key, cipher_info);
    cube_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    mu_assert("6 clocks, max term count != 7\n", cube_max_terms->max_term_count == 7);
    chosen_max_term = cube_max_terms->max_terms[6];
    mu_assert("failed for 6 init clocks results[6](111) num terms != 1\n", chosen_max_term->numberOfTerms == 1);
    mu_assert("failed for 6 init clocks results[6](111) term[1] == %d not 0\n",chosen_max_term->terms[0] == 0, chosen_max_term->terms[0]);
    chosen_max_term = cube_max_terms->max_terms[5];
    mu_assert("failed for 6 init clocks results[5](110) num of terms != 1\n", chosen_max_term->numberOfTerms == 1);
    mu_assert("failed for 6 init clocks results[5](110) term[0] != 3\n",chosen_max_term->terms[0] == 3);
    chosen_max_term = cube_max_terms->max_terms[2];
    mu_assert("failed for 6 init clocks results[2](011) num terms != 0\n", chosen_max_term->numberOfTerms == 0);
    for(int i=0;i<cube_max_terms->max_term_count;i++){
        free_max_term(cube_max_terms->max_terms[i]);
    }
    free(cube_max_terms);
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
    uint64_t * results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("superpoly results[0] != 0 && results[1] != 0 for key {0} clocks 1, axis {0}\n", get_bit(results, 0) == 0 && get_bit(results, 1) == 0);
    free(results);
    key[0] = power(2, 0) + power(2,3);
    num_of_axis = 1;
    axis[0] = 0;
    results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("superpoly results[0] == %d, expected %d && results[1] == %d, expected %d for key {power(2, 0) + power(2,3)} clocks 1, axis {0}\n", get_bit(results, 0) == 1 && get_bit(results, 1) == 1,get_bit(results, 0), 1, get_bit(results, 1), 1);
    free(results);
    cipher_info->init_clocks = 5;
    key[0] = power(2, 5)-1;
    num_of_axis = 1;
    axis[0] = 3;
    results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("superpoly results[1] == 1 for key {power(2, 5)-1} clocks 6, axis {3}\n", get_bit(results, 1) == 1);
    free(results);
    cipher_info->init_clocks = 5;
    num_of_axis = 3;
    axis[0] = 0; axis[1] = 1; axis[2] = 2;
    key[0] = power(2, 1) + power(2, 3);
    results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("failed for 6 init clocks results[0](0)== 1\n", get_bit(results, 0)== 0);
    mu_assert("failed for 6 init clocks results[6](110)== 1\n", get_bit(results, 6)== 1);
    mu_assert("failed for 6 init clocks results[7](111)== 1\n", get_bit(results, 7)== 0);
    key[0] = power(2, 0) + power(2, 1);
    results = mobius_transform(axis, num_of_axis, num_of_axis, key, cipher_info);
    mu_assert("failed for 6 init clocks results[1](11) == %d, expected 1\n", get_bit(results, 1) == 0, get_bit(results, 1));
    printf("done testing mobius transform\n");
    return 0;
}

int test_mobius_find_max_terms(){
    printf("testing mobois find max terms\n");
    Cipher_info * cipher_info = dummy_info();
    cipher_info->init_clocks = 0;
    int dimension_limit = 1;
    int max_term_limit = 1;
    Max_terms_list* max_terms_list = mobius_find_max_terms(max_term_limit,dimension_limit, cipher_info);
    Max_term ** max_terms = max_terms_list->max_terms;
    int expected_max_term_count;
    int expected_term_count;
    int expected_term_values[5];
    int expected_iv;
    expected_max_term_count = 1;
    expected_term_count = 1;
    expected_term_values[0] = 0;
    expected_iv = 1;//1 not 0 because the bit index indicates the iv, so 00001 is iv[0], 00010 is iv[1], 00000 is no ivs used
    mu_assert("failed for 0 clocks, max_term_count was %d not %d\n", max_terms_list->max_term_count == expected_max_term_count, max_terms_list->max_term_count, expected_max_term_count);
    mu_assert("failed for 0 clocks, max_term[0] term count was %d not %d\n", max_terms[0]->numberOfTerms == expected_term_count, max_terms[0]->numberOfTerms, expected_term_count);
    mu_assert("failed for 0 clocks, max_term[0] term[0] was %d not %d\n", max_terms[0]->terms[0] == expected_term_values[0], max_terms[0]->terms[0], expected_term_values[0]);
    mu_assert("failed for 0 clocks, max_term[0] iv was %"PRIu64" not %d\n", max_terms[0]->iv[0] == expected_iv, max_terms[0]->iv[0], expected_iv);
    cipher_info->init_clocks = 5;
    max_terms_list = mobius_find_max_terms(5, 4, cipher_info);
    max_terms = max_terms_list->max_terms;
    Max_term chosen_max_term = *max_terms[0];
    expected_max_term_count = 3;
    printf("num of max terms %d\n", max_terms_list->max_term_count);
    mu_assert("failed for 6 clocks, max_term_count was %d not %d\n", max_terms_list->max_term_count == expected_max_term_count, max_terms_list->max_term_count, expected_max_term_count);
    expected_term_count = 1;
    printf("chiosen.number of terms %d\n", chosen_max_term.numberOfTerms);
    mu_assert("failed for 6 clocks, max_term[0] term count was %d not %d\n", chosen_max_term.numberOfTerms == expected_term_count, chosen_max_term.numberOfTerms, expected_term_count);
    expected_term_values[0] = 3;
    mu_assert("failed for 6 clocks, max_term[0] term[0] was %d not %d\n", chosen_max_term.terms[0] ==  expected_term_values[0], chosen_max_term.terms[0], expected_term_values[0]);
    expected_iv = 6;
    mu_assert("failed for 6 clocks, max_term[0] iv was %"PRIu64" not %d\n", chosen_max_term.iv[0] == expected_iv, chosen_max_term.iv[0], expected_iv);
    //
    chosen_max_term = *max_terms[1];
    expected_term_count = 1;
    mu_assert("failed for 6 clocks, max_term[1] term count was %d not %d\n", chosen_max_term.numberOfTerms == expected_term_count, chosen_max_term.numberOfTerms, expected_term_count);
    expected_term_values[0] = 0;
    mu_assert("failed for 6 clocks, max_term[1] term[0] was %d not %d\n", chosen_max_term.terms[0] == expected_term_values[0], chosen_max_term.terms[0], expected_term_values[0]);
    expected_iv = 7;
    mu_assert("failed for 6 clocks, max_term[1] iv was %"PRIu64" not %d\n", chosen_max_term.iv[0] == expected_iv, chosen_max_term.iv[0], expected_iv);
    //
    chosen_max_term = *max_terms[2];
    expected_term_count = 1;
    mu_assert("failed for 6 clocks, max_term[2] term count was %d not %d\n", chosen_max_term.numberOfTerms == expected_term_count, chosen_max_term.numberOfTerms, expected_term_count);
    expected_term_values[0] = 3;
    mu_assert("failed for 6 clocks, max_term[2] term[0] was %d not %d\n", chosen_max_term.terms[0] ==  expected_term_values[0], chosen_max_term.terms[0], expected_term_values[0]);
    expected_iv = 8;
    mu_assert("failed for 6 clocks, max_term[2] iv was %"PRIu64" not %d\n", chosen_max_term.iv[0] == expected_iv, chosen_max_term.iv[0], expected_iv);
    printf("done mobois find max terms\n");
    return 0;
}

int main(int argc, char **argv) {
    test_case test_cases[4] = {test_mobius_transform, test_mobius_construct_max_terms, test_mobius_is_super_poly_linear, test_mobius_find_max_terms};
    run_cases(test_cases, (sizeof test_cases/ sizeof(test_case)));
}