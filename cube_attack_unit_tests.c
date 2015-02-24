#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#include "minunit.h"
#include "cube_attack.h"
#include "useful.h"
#include "grain.h"

int tests_run = 0;

char *test_increase_dimensions(){
    int *dimension_count = malloc(sizeof(int));
    int *dimensions = malloc(sizeof(int)*cipher_info->iv_size);
    dimensions[0] = 1;
    *dimension_count = 1;
    increase_dimensions(dimensions, dimension_count);
    for(int i=0;i<*dimension_count;i++){
        printf("dimensions[%d] = %d\n", i, dimensions[i]);
    }
    printf("dim count %d\n", *dimension_count);
    mu_assert("increase_dimension failed, dimensions[0] == 2 && dimension_count == 1", dimensions[0] == 2 && *dimension_count == 1);
    *dimension_count = 2;
    dimensions[0] = 94; dimensions[1] = 95;
    increase_dimensions(dimensions, dimension_count);
    for(int i=0;i<*dimension_count;i++){
        printf("dimensions[%d] = %d\n", i, dimensions[i]);
    }
    printf("dim count %d\n", *dimension_count);
    mu_assert("increase_dimension failed, dimensions[0] == 0, dimensions[0] == 1, dimensions[0] == 2 && dimension_count == 3",
            dimensions[0] == 0 && dimensions[1] == 1 && dimensions[2] == 2 && *dimension_count == 3);
    return 0;
}

char *test_get_super_poly_bit_case(uint64_t **ivs, uint64_t ivs_size, uint64_t *key, int *axises, int cube_dimension){
    printf("testing get_super_poly_bit\n");
    printf("axises: ");
    for(int i=0;i<cube_dimension;i++){
        printf("%d, ", axises[i]);
    }
    printf("\n");
    debug_print("ivs_size: %"PRIu64"\n", ivs_size);
    int correct_super_bit = 0;
    for(int i=0;i<ivs_size;i++){
        Grain_state state = setupGrain(ivs[i], key, SETUP_CLOCK_ROUNDS);
        int next_super_bit = production_clock(&state);
        correct_super_bit = correct_super_bit ^ next_super_bit;
        printf("correct iv %d superbit: %d iv:\n", i, next_super_bit);
        print_uint64_t_array(ivs[i], 2);
    }
    int actual_super_bit = get_super_poly_bit(key, axises, cube_dimension);
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

static char *test_get_super_poly_bit() {
    char *return_value = 0;
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
    printf("pretest iv 0:");
    print_uint64_t_array(ivs[0], 2);
    printf("pretest iv 1");
    print_uint64_t_array(ivs[1], 2);
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //----
    free_ivs_and_axises(&ivs, cube_dimension, &axises);
    printf("done testing get_super_poly_bit\n");
    return 0;
}

static char *cube_attack_tests() {
    cipher_info = grain_info();
    mu_run_test(test_increase_dimensions);
    mu_run_test(test_get_super_poly_bit);
    return 0;
}

int main(int argc, char **argv) {
    srand(time(NULL));
    char *result = cube_attack_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}