#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#include "minunit.h"
#include "../cube_attack/cube_attack.h"
#include "../cipher_io/useful.h"
#include "../ciphers/grain.h"
#include "../ciphers/dummy_cipher.h"

static int test_increase_dimensions(){
    Cipher_info * cipher_info = grain_info();
    int *dimension_count = malloc(sizeof(int));
    int *dimensions = malloc(sizeof(int)*cipher_info->iv_size);
    dimensions[0] = 1;
    *dimension_count = 1;
    increase_dimensions(dimensions, dimension_count, cipher_info);
    for(int i=0;i<*dimension_count;i++){
        printf("dimensions[%d] = %d\n", i, dimensions[i]);
    }
    printf("dim count %d\n", *dimension_count);
    mu_assert("increase_dimension failed, dimensions[0] == 2 && dimension_count == 1", dimensions[0] == 2 && *dimension_count == 1);
    *dimension_count = 2;
    dimensions[0] = 94; dimensions[1] = 95;
    increase_dimensions(dimensions, dimension_count, cipher_info);
    for(int i=0;i<*dimension_count;i++){
        printf("dimensions[%d] = %d\n", i, dimensions[i]);
    }
    printf("dim count %d\n", *dimension_count);
    mu_assert("increase_dimension failed, dimensions[0] == 0, dimensions[0] == 1, dimensions[0] == 2 && dimension_count == 3",
            dimensions[0] == 0 && dimensions[1] == 1 && dimensions[2] == 2 && *dimension_count == 3);
    return 0;
}

static int test_get_super_poly_bit_case(uint64_t **ivs, uint64_t ivs_size, uint64_t *key, int *axises, int cube_dimension){
    Cipher_info* cipher_info = grain_info();
    printf("testing get_super_poly_bit\n");
    printf("axises: ");
    for(int i=0;i<cube_dimension;i++){
        printf("%d, ", axises[i]);
    }
    printf("\n");
    debug_print("ivs_size: %d\n", cipher_info->iv_size);
    int correct_super_bit = 0;
    for(int i=0;i<ivs_size;i++){
        Grain_state state = setupGrain(ivs[i], key, cipher_info->init_clocks);
        int next_super_bit = production_clock(&state);
        correct_super_bit = correct_super_bit ^ next_super_bit;
        printf("correct iv %d superbit: %d iv:\n", i, next_super_bit);
        print_uint64_t_array(ivs[i], cipher_info->iv_size/64);
    }
    int actual_super_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    printf("correct super bit: %d, actual super bit: %d\n", correct_super_bit, actual_super_bit);
    mu_assert("get_super_poly_bit is wrong", correct_super_bit == actual_super_bit);
    return 0;
}

void free_ivs_and_axises(uint64_t ***ivs, int iv_count, int **axises){
    free(*axises);
    for(int i=0; i<iv_count; i++){
        free((*ivs)[i]);
    }
    free(*ivs);
}

uint64_t malloc_ivs_and_axises(uint64_t ***ivs, int cube_dimension, int **axises){
    uint64_t number_ivs = power(2, cube_dimension);
    *axises = malloc(cube_dimension * sizeof(int));
    *ivs = malloc(sizeof(uint64_t *)*(size_t)number_ivs);
    for(int i=0; i<number_ivs; i++){
        (*ivs)[i] = calloc(2, sizeof(uint64_t));
    }
    return number_ivs;
}

static int test_get_super_poly_bit() {
    int return_value = 0;
    uint64_t key[2] = {0,0};//tests should work with w/e key
    //setup
    int cube_dimension;
    uint64_t iv_count;
    int *axises;
    uint64_t **ivs;
    //one dimension tests
    cube_dimension = 1;
    iv_count = malloc_ivs_and_axises(&ivs, cube_dimension, &axises);
    axises[0] = 1;
    ivs[0][0] = 0; ivs[0][1] = 0;
    ivs[1][0] = power(2, 1); ivs[1][1] = 0;
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //---------
    axises[0] = 57;
    ivs[0][0] = 0;ivs[0][1] = 0;
    ivs[1][0] = power(2, 57);ivs[1][1] = 0;
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //----
    free_ivs_and_axises(&ivs, cube_dimension, &axises);
    //two dimensions
    cube_dimension = 2;
    iv_count = malloc_ivs_and_axises(&ivs, cube_dimension, &axises);
    axises[0] = 1;axises[1] = 64;
    ivs[0][0] = 0;ivs[0][1] = 0;
    ivs[1][0] = power(2, 1);ivs[1][1] = 0;
    ivs[2][0] = 0;ivs[2][1] = power(2, 64-64);
    ivs[3][0] = power(2, 1);ivs[3][1] = power(2, 64-64);
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //---------
    axises[0] = 33;axises[1] = 34;
    ivs[0][0] = 0;ivs[0][1] = 0;
    ivs[1][0] = power(2, 33);ivs[1][1] = 0;
    ivs[2][0] = power(2, 34);ivs[2][1] = 0;
    ivs[3][0] = power(2, 33)+ power(2, 34);ivs[3][1] = 0;
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //----
    free_ivs_and_axises(&ivs, cube_dimension, &axises);
    //three dimensions
    cube_dimension = 3;
    iv_count = malloc_ivs_and_axises(&ivs, cube_dimension, &axises);
    axises[0] = 0;axises[1] = 65;axises[2] = 127;
    ivs[0][0] = 0;          ivs[0][1] = 0;
    ivs[1][0] = power(2, 0);ivs[1][1] = 0;
    ivs[2][0] = 0;          ivs[2][1] = power(2, 65-64);
    ivs[3][0] = 0;          ivs[3][1] = power(2, 127-64);
    ivs[4][0] = power(2, 0);ivs[4][1] = power(2, 65-64);
    ivs[5][0] = power(2, 0);ivs[5][1] = power(2, 127-64);
    ivs[6][0] = 0;          ivs[6][1] = power(2, 65-64)+power(2, 127-64);
    ivs[7][0] = power(2, 0);ivs[7][1] = power(2, 65-64)+power(2, 127-64);
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //----
    free_ivs_and_axises(&ivs, cube_dimension, &axises);
    printf("done testing get_super_poly_bit\n");
    return 0;
}

static int test_get_super_poly_bit_dummy_cipher(){
    printf("testing get_super_poly_bit with dummy cipher\n");
    Cipher_info * cipher_info = dummy_info();
    uint64_t key[1] = {0};
    int axises[5] = {0,0,0,0,0};
    int cube_dimension;
    //0 clock + 1 for output
    //check key[0] is extracted correctly
    cipher_info->init_clocks = 0;
    key[0] = 0;
    axises[0] = 0;
    cube_dimension = 1;
    int super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 0 clock, key 00000 iv 00001, superpoly bit != key[0]", super_poly_bit == 0);
    key[0] = 1;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 0 clock, key 00001 iv 00001, superpoly bit != key[0]", super_poly_bit == 1);
    //check a constant is returned for too many axises
    key[0] = 0;
    axises[0] = 0; axises[1] = 1;
    cube_dimension = 2;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 0 clock, key 00000 iv 00011, superpoly bit != free term", super_poly_bit == 0);
    key[0] = power(2, 5)-1;
    axises[0] = 0; axises[1] = 1;
    cube_dimension = 2;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 0 clock, key 11111 iv 00011, superpoly bit != free term", super_poly_bit == 0);
    //5 clocks + 1 for output
    cipher_info->init_clocks = 5;
    //check keybit is extract correctly when using multiple iv
    key[0] = power(2,5)-2;//0b11110
    axises[0] = 0; axises[1] = 1; axises[2] = 2;
    cube_dimension = 3;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 5 clocks, key 11110 iv 00111, superpoly bit != key[0]", super_poly_bit == 0);
    key[0] = 21;//0b10101
    axises[0] = 0; axises[1] = 1; axises[2] = 2;
    cube_dimension = 3;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 5 clocks, key 10101 iv 00111, superpoly bit != key[0]", super_poly_bit == 1);
    //checking non-linear super polys are correct
    key[0] = power(2, 5)-1;
    axises[0] = 0; axises[1] = 1;
    cube_dimension = 2;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 5 clocks, key 11111 iv 00011, superpoly bit != (KEY[0]&KEY[1])", super_poly_bit == 1);
    key[0] = power(2, 5) - 2;
    axises[0] = 0; axises[1] = 1;
    cube_dimension = 2;
    super_poly_bit = get_super_poly_bit(key, axises, cube_dimension, cipher_info);
    mu_assert("failed for 5 clocks, key 10111 iv 00011, superpoly bit != (KEY[1]&KEY[2])^KEY[3]", super_poly_bit == 0);
    printf("done testing get_super_poly_bit with dummy cipher\n");
    return 0;
}

static int test_construct_max_term(){
    Cipher_info * cipher_info = dummy_info();
    printf("testing construct_max_terms with dummy cipher\n");
    //0 clock + 1 for output
    //checking linear terms are found
    cipher_info->init_clocks = 0;
    int axises[5] = {0,0,0,0,0};
    int cube_dimension;
    cube_dimension = 1;
    axises[0] = 0;
    Max_term* terms = construct_max_term(axises, cube_dimension, cipher_info);
    mu_assert("failed for 0 clocks, iv 00001 should gives terms: key[0]", terms->numberOfTerms == 1 && terms->terms[0] == 0 && terms->plusOne == 0);
    cube_dimension = 5;
    axises[0] = 1; axises[1] = 2;axises[2] = 3;axises[3] = 4;axises[4] = 5;
    terms = construct_max_term(axises, cube_dimension, cipher_info);
    mu_assert("failed for 0 clocks, iv 01111100 should gives terms: none", terms->numberOfTerms == 0 && terms->plusOne == 0);
    //5 clocks + 1 for output
    //checking non-linear terms are discarded
    cipher_info->init_clocks = 5;
    cube_dimension = 1;
    axises[0] = 1;
    terms = construct_max_term(axises, cube_dimension, cipher_info);
    mu_assert("failed for 8 clocks, iv 00010 should gives terms: key[1]", terms->numberOfTerms == 1 && terms->terms[0] == 1 && terms->plusOne == 0);
    printf("done testing construct_max_terms with dummy cipher\n");
    return 0;
}

static int test_is_super_poly_linear(){
    printf("testing is_superpoly_linear with dummy cipher\n");
    Cipher_info * cipher_info = dummy_info();
    //0 clocks + output bit
    cipher_info->init_clocks = 0;
    int axises[5] = {0,0,0,0,0};
    int cube_dimension;
    cube_dimension = 1;
    axises[0] = 0;
    mu_assert("failed 0 clocks for iv 00001, should be linear", is_super_poly_linear(axises, cube_dimension, cipher_info));
    //5 clocks + output bit
    cipher_info->init_clocks = 5;
    cube_dimension = 1;
    axises[0] = 2;
    mu_assert("failed 0 clocks for iv 00100, should be non-linear", !is_super_poly_linear(axises, cube_dimension, cipher_info));
    printf("done testing is_super_poly_linear\n");
    return 0;
}

int check_max_terms(int clocks, Max_term * correct_max_terms, Max_term * actual_max_terms, int expected_amount, int number_being_tested, int iv_size){
    mu_assert("failed for %d clocks, max_term_count is %d not %d", HASH_COUNT(actual_max_terms) == expected_amount, clocks, HASH_COUNT(actual_max_terms), expected_amount);
    for(int i=0;i<number_being_tested; i++){
        Max_term correct_max_term = correct_max_terms[i];
        Max_term* actual_max_term = get_max_term(&actual_max_terms, correct_max_term.iv, iv_size);
        mu_assert("failed for %d clocks, no maxterm with iv %"PRIu64, actual_max_term != NULL, clocks, correct_max_term.iv[0]);
        mu_assert("failed for %d clocks,  max term iv %"PRIu64" no. of terms == %d not %d", actual_max_term->numberOfTerms == correct_max_term.numberOfTerms, clocks, correct_max_term.iv[0],  actual_max_term->numberOfTerms, correct_max_term.numberOfTerms);
        for(int term_num=0; term_num<correct_max_term.numberOfTerms;term_num++) {
            mu_assert("failed for %d clocks, max term iv %"
                              PRIu64
                              " term[0] is %d not %d", actual_max_term->terms[term_num] == correct_max_term.terms[term_num], clocks,
                      correct_max_term.iv[0], actual_max_term->terms[term_num], correct_max_term.terms[term_num]);
        }
        mu_assert("failed for %d clocks, max term iv %"PRIu64" has plusOne == %d not %d", actual_max_term->plusOne == correct_max_term.plusOne, clocks, correct_max_term.iv[0], actual_max_term->plusOne, correct_max_term.plusOne);
    }
    delete_hash_and_free(&actual_max_terms);
    return 0;
}

static int test_find_max_terms(){
    printf("testing find_max_terms\n");
    size_t dimension_limit = 5;
    int max_term_limit = 5;//set arbitrarily high because we can afford to find all max_terms (2^5 iv combinations)
    Cipher_info* cipher_info = dummy_info();
    uint64_t  iv[2] = {0,0};
    int terms[5] = {0};
    //testing for 0 clocks
    int expected_max_term_amount = 1;
    Max_term * correct_max_terms = malloc(sizeof(Max_term)*expected_max_term_amount);
    //---
    iv[0] = 1;
    terms[0] = 0;
    correct_max_terms[0] = *make_max_term(iv, terms, 0, 1, cipher_info->iv_size);
    //---
    cipher_info->init_clocks = 0;
    Max_term* actual_max_terms = find_max_terms(max_term_limit, dimension_limit, cipher_info);
    int success = check_max_terms(cipher_info->init_clocks, correct_max_terms, actual_max_terms, expected_max_term_amount, expected_max_term_amount, cipher_info->iv_size);
    if(success){
        return 1;
    }
    //testing for 5 clocks
    expected_max_term_amount = 3;
    correct_max_terms = malloc(sizeof(Max_term)*expected_max_term_amount);
    //---
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
    actual_max_terms = find_max_terms(max_term_limit, dimension_limit, cipher_info);
    success = check_max_terms(cipher_info->init_clocks, correct_max_terms, actual_max_terms, expected_max_term_amount, expected_max_term_amount, cipher_info->iv_size);
    if(success){
        return 1;
    }
    printf("testing find_max_terms done\n");
    return 0;
}

int run_cube_attack_unit_tests() {
    test_case test_cases[6] = {test_increase_dimensions, test_get_super_poly_bit, test_get_super_poly_bit_dummy_cipher, test_construct_max_term,
                                test_is_super_poly_linear, test_find_max_terms};
    return run_cases(test_cases, (sizeof test_cases/ sizeof(test_case)));
}