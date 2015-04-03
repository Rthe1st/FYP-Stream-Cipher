#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "minunit.h"
#include "../cube_attack/mobius_cube_attack.h"
#include "../cube_attack/cube_attack.h"
#include "../cipher_io/useful.h"
#include "../ciphers/dummy_cipher.h"
#include "cube_attack_unit_tests.h"

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
    uint64_t zeroed_key[1] = {0};
    //---
    int expected_max_term_amount = 1;
    uint64_t iv[2] = {0,0};
    int terms[5] = {0};
    Max_term * correct_max_terms = malloc(sizeof(Max_term)*expected_max_term_amount);
    //--
    iv[0] = 1; iv[1] = 0;
    terms[0] = 0;
    correct_max_terms[0] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    cipher_info->init_clocks = 0;
    int dimension_limit = 1;
    int dimensions[10] = {0};
    uint64_t *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, 0, zeroed_key, cipher_info);
    Max_term* actual_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    int success = check_max_terms(cipher_info->init_clocks, correct_max_terms, actual_max_terms, expected_max_term_amount, expected_max_term_amount, cipher_info->iv_size);
    if(success){
        return 1;
    }
    //---
    expected_max_term_amount = 7;
    correct_max_terms = malloc(sizeof(Max_term)*expected_max_term_amount);
    //---
    iv[0] = 6; iv[1] = 0;
    terms[0] = 3;
    correct_max_terms[0] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    iv[0] = 7; iv[1] = 0;
    terms[0] = 0;
    correct_max_terms[1] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    iv[0] = 1; iv[1] = 0;
    correct_max_terms[2] = *make_max_term(iv, terms, 0, 0, cipher_info->iv_size);
    //---
    cipher_info->init_clocks = 5;
    zeroed_key_poly = mobius_transform(dimensions, dimension_limit, 0, zeroed_key, cipher_info);
    dimension_limit = 3;
    dimensions[0] = 0; dimensions[1] = 1; dimensions[2] = 2;
    actual_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
    success = check_max_terms(cipher_info->init_clocks, correct_max_terms, actual_max_terms, expected_max_term_amount, 3, cipher_info->iv_size);
    if(success){
        return 1;
    }
    //---
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
    uint64_t iv[2] = {0};
    int terms[5] = {0};
    //testing for 0 clocks
    int expected_max_term_amount = 1;
    Max_term * correct_max_terms = malloc(sizeof(Max_term)*expected_max_term_amount);
    //--
    iv[0] = 1; iv[1] = 0;
    terms[0] = 0;
    correct_max_terms[0] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    cipher_info->init_clocks = 0;
    int dimension_limit = 1;
    int max_term_limit = 1;
    Max_term* actual_max_terms = mobius_find_max_terms(max_term_limit,dimension_limit, cipher_info);
    printf("exp amt %d\n", expected_max_term_amount);
    int success = check_max_terms(cipher_info->init_clocks, correct_max_terms, actual_max_terms, expected_max_term_amount, expected_max_term_amount, cipher_info->iv_size);
    if(success){
        return 1;
    }
    //---
    //testing 5
    expected_max_term_amount = 3;
    correct_max_terms = malloc(sizeof(Max_term)*expected_max_term_amount);
    iv[0] = 8; iv[1] = 0;
    terms[0] = 3;
    correct_max_terms[0] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    iv[0] = 6; iv[1] = 0;
    terms[0] = 3;
    correct_max_terms[1] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    iv[0] = 7; iv[1] = 0;
    terms[0] = 0;
    correct_max_terms[2] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    cipher_info->init_clocks = 5;
    dimension_limit = 3;
    max_term_limit = 3;
    actual_max_terms = mobius_find_max_terms(max_term_limit, dimension_limit, cipher_info);
    success = check_max_terms(cipher_info->init_clocks, correct_max_terms, actual_max_terms, expected_max_term_amount, expected_max_term_amount, cipher_info->iv_size);
    if(success){
        return 1;
    }
    printf("done mobois find max terms\n");
    return 0;
}

int run_mobius_cube_attack_unit_tests() {
    test_case test_cases[4] = {test_mobius_transform, test_mobius_construct_max_terms, test_mobius_is_super_poly_linear, test_mobius_find_max_terms};
    return run_cases(test_cases, (sizeof test_cases/ sizeof(test_case)));
}