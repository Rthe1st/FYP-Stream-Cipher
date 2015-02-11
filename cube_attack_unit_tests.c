#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <winsnmp.h>
#include "minunit.h"
#include "cube_attack.h"
#include "useful.h"
#include "grain.h"

int tests_run = 0;

static char *test_generate_iv() {
    printf("testing generate_iv");
    int dimension_num = 0;
    int *dimensions = malloc(dimension_num * sizeof(int));
    uint64_t *iv = generate_iv(dimensions, dimension_num);
    mu_assert("generate_iv failed for dimensions {}", iv[0] == 0 && iv[1] == 0);
    free(dimensions);
    dimension_num = 1;
    dimensions = malloc(dimension_num * sizeof(int));
    dimensions[0] = 0;
    iv = generate_iv(dimensions, dimension_num);
    mu_assert("generate_iv failed for dimensions {0}", iv[0] == 1 && iv[1] == 0);
    dimensions[0] = 64;
    iv = generate_iv(dimensions, dimension_num);
    mu_assert("generate_iv failed for dimensions {64}", iv[0] == 0 && iv[1] == 1);
    for (int i = 0; i < 10; i++) {
        //only 50 because of the crappy "pick and test random dimension" method commented below
        //also realistically, only small number of dimensions (>10) matter
        dimension_num = (rand() % 50) + 1;//+1 to avoid testing for 0 dimensions
        free(dimensions);
        dimensions = calloc(dimension_num * sizeof(int), sizeof(int));
        int dimensions_added = 0;
        uint64_t correct_iv[2] = {0, 0};
        debug_print("dimension_num %d", dimension_num);
        while (dimensions_added < dimension_num) {
            //very inefficient: pick a dimension (at random), if already taken, pick another at random
            //low priority because unit test inefficiently isn't the end of the world
            int next_dimension = rand() % 96;
            debug_print("next_dimension %d\n", next_dimension);
            //check the dimension isn't a duplicate, not required if a hash table was used
            int already_added = 0;
            for (int g = 0; g < dimensions_added; g++) {
                if (next_dimension == dimensions[g]) {
                    debug_print("duplicate, continue\n");
                    already_added = 1;
                    break;
                }
            }
            if (!already_added) {
                dimensions[dimensions_added] = next_dimension;
                correct_iv[next_dimension / 64] += power(2, next_dimension % 64);
                debug_print("next_dimension / 64: %d, correctiv[next_dimension / 64] = %"PRIu64"\n", next_dimension / 64, correct_iv[next_dimension / 64]);
                dimensions_added++;
            }
        }
        printf("testing dimensions: ");
        for (int i = 0; i < dimension_num; i++) {
            printf("%d, ", dimensions[i]);
        }
        printf("\n");
        iv = generate_iv(dimensions, dimension_num);
        printf("correct value: {%"PRIu64", %"PRIu64"}\n", correct_iv[0], correct_iv[1]);
        printf("actual value: {%"PRIu64", %"PRIu64"}\n", iv[0], iv[1]);
        mu_assert("iv failed for random dimensions", iv[0] == correct_iv[0] && iv[1] == correct_iv[1]);
    }
    printf("done testing generate_iv\n");
    return 0;
}

char *test_get_super_poly_bit_case(uint64_t **ivs, uint64_t ivs_size, uint64_t *key, int *axises, int cube_dimension){
    printf("axises: ");
    for(int i=0;i<cube_dimension;i++){
        printf("%d, ", axises[i]);
    }
    printf("\n");
    debug_print("ivs_size: %"PRIu64"\n", ivs_size);
    int correct_super_bit = 0;
    for(int i=0;i<ivs_size;i++){
        State state = setupGrain(ivs[i], key, SETUP_CLOCK_ROUNDS);
        int next_super_bit = production_clock(&state);
        correct_super_bit = correct_super_bit ^ next_super_bit;
        printf("correct iv %d, iv[0] %"PRIu64" iv[1] %"PRIu64" superbit: %d\n", i, ivs[i][0], ivs[i][1], next_super_bit);
    }
    int actual_super_bit = get_super_poly_bit(key, axises, cube_dimension);
    printf("correct super bit: %d, actual super bit: %d\n", correct_super_bit, actual_super_bit);
    mu_assert("get_super_poly_bit is wrong", correct_super_bit == actual_super_bit);
    return 0;
}

void free_ivs_and_axises(uint64_t **ivs, int iv_count, int *axises){
    free(axises);
    for(int i=0; i<iv_count; i++){
        free(ivs[i]);
    }
    free(ivs);
}

uint64_t malloc_ivs_and_axises(uint64_t ***ivs, int cube_dimension, int **axises){
    uint64_t number_ivs = power(2, cube_dimension);
    *axises = malloc(cube_dimension * sizeof(int));
    *ivs = malloc(sizeof(uint64_t *)*number_ivs);
    for(int i=0; i<number_ivs; i++){
        (*ivs)[i] = calloc(number_ivs, sizeof(uint64_t)*2);
    }
    return number_ivs;
}


static char *test_get_super_poly_bit() {
    char *return_value = 0;
    printf("testing get_super_poly_bit\n");
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
    free_ivs_and_axises(ivs, cube_dimension, axises);
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
    free_ivs_and_axises(ivs, cube_dimension, axises);
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
    printf("pretest iv %d, iv[0] %"PRIu64" iv[1] %"PRIu64"\n", 0, ivs[0][0], ivs[0][1]);
    printf("pretest iv %d, iv[0] %"PRIu64" iv[1] %"PRIu64"\n", 1, ivs[1][0], ivs[1][1]);
    return_value = test_get_super_poly_bit_case(ivs, iv_count, key, axises, cube_dimension);
    if(return_value != 0){return return_value;};
    //----
    free_ivs_and_axises(ivs, cube_dimension, axises);
    printf("done testing get_super_poly_bit\n");
    return 0;
}

static char *cube_attack_tests() {
    mu_run_test(test_generate_iv);
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