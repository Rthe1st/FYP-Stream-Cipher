#include <stdlib.h>

#include "../ciphers/cipher_helpers.h"
#include "mobius_cube_attack.h"
#include "cube_attack.h"
#include "../cipher_io/useful.h"

const int MIN_NUM_AXES = 0;

//returns 1 if dimension count increase, 0 otherwise
int mobius_increase_dimensions(int *dimensions, int *dimension_count, const Cipher_info * const cipher_info, int skip_param) {
    //dimensions elements should be in ascending order
    for(int i =0;i<*dimension_count;i++){
        dimensions[i]+=skip_param;
    }
    if(dimensions[*dimension_count-1] > cipher_info->iv_size-1){
        return 1;
    }else{
        return 0;
    }
}

//PSEUDO CODE FOR FINDING MAXTERMS WITH MOBIUS TRANSFORM
//pick as larger number of axis as possible (try 5 to start?)
//i.e. 0, 1, 2, 3, 4
//compute twice for 3 times for 2 different keys (and then the sum of those keys)
//check linearity
//if linear, check for terms
//  if terms, add as max term
//  either way add axis set to list of "invalid subsets" which must be avoided in future
//for all non-linear, starting from set with most members
//  add more axis, recurse this proccess
//  but take care to reuse computed sub-cubes and not compute set with a subset in "invalid subset" list
//when all non-linear have been searched recursively
//repeat with new set of axis that shares no members with the old one
//  list of old axis must be kept so they arn't accidently added to the new set later
//  this would be duplicating previous results


Max_term *mobius_find_max_terms(int max_term_limit, int dimension_limit, const Cipher_info * const cipher_info, int skip_param) {
    int *dimensions = calloc(dimension_limit+1, sizeof(int));
    Max_term *max_terms = NULL;
    uint64_t zeroed_key[2] = {0, 0};
    for (int i = 0; i < dimension_limit; i++) {
        dimensions[i] = i;
    }
    while (HASH_COUNT(max_terms) < max_term_limit) {
        uint64_t *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, MIN_NUM_AXES, zeroed_key, cipher_info);
        printf("zero poly done\n");
        uint64_t *linear_results = mobius_is_super_poly_linear(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
        printf("linear test done\n");
        Max_term * cube_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
        printf("terms done\n");
        for(uint64_t mask=1;mask<power(2, dimension_limit);mask++){
            printf(".");
            uint64_t * iv = iv_from_mask(mask, dimensions, dimension_limit, cipher_info->iv_size);
            Max_term * current_max_term = get_max_term(&cube_max_terms, iv, cipher_info->iv_size);
            free(iv);
            //delete from previous hash before adding it to the next
            //because both make use of the same struct fields hh
            HASH_DEL(cube_max_terms, current_max_term);
            int is_valid_max_term = current_max_term->numberOfTerms > 0 && get_bit(linear_results, mask) == 1;
            if(is_valid_max_term && add_max_term(&max_terms, current_max_term, cipher_info->iv_size)){
                    printf("\n");
                    printf("max_terms->max_term_count %d\n", HASH_COUNT(max_terms));
            }else{
                free_max_term(current_max_term);
            }
        }
        free(zeroed_key_poly);
        free(linear_results);
        if(mobius_increase_dimensions(dimensions, &dimension_limit, cipher_info, skip_param)){
            break;
        }
    }
    free(dimensions);
    printf("overall max term count %d\n", HASH_COUNT(max_terms));
    return max_terms;
}

int* random_picker(int lower, int upper){
    int range = upper-lower;
    int* list = malloc(sizeof(int)*(range-1));
    for(int i=lower;i<upper; i++){
        list[i] = i;
    }
    for(int g=0;g<2;g++) {
        for (int i = lower; i < upper; i++) {
            int first = rand()%range;
            int second = rand()%range;
            int temp = list[first];
            list[first] = list[second];
            list[second] = temp;
        }
    }
    return list;
}

Max_term * mobius_find_max_terms_guessing(int max_term_limit, size_t dimension_limit, int max_number_of_tries, const Cipher_info * const cipher_info) {
    srand(1);
    int *dimensions = calloc(dimension_limit, sizeof(int));
    Max_term *max_terms = NULL;
    uint64_t zeroed_key[2] = {0, 0};
    int number_of_tries = 0;
    while (HASH_COUNT(max_terms) < max_term_limit && number_of_tries < max_number_of_tries) {
        printf("num of tries %d\n", number_of_tries);
        int * random_dim = random_picker(0, cipher_info->iv_size);
        for (int i = 0; i < dimension_limit; i++) {
            dimensions[i] = random_dim[i];
        }
        free(random_dim);
        uint64_t *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, MIN_NUM_AXES, zeroed_key, cipher_info);
        printf("zero poly done\n");
        uint64_t *linear_results = mobius_is_super_poly_linear(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
        printf("linear test done\n");
        Max_term * cube_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
        printf("terms done\n");
        for(uint64_t mask=1;mask<power(2, dimension_limit);mask++){
            printf(".");
            uint64_t * iv = iv_from_mask(mask, dimensions, dimension_limit, cipher_info->iv_size);
            Max_term * current_max_term = get_max_term(&cube_max_terms, iv, cipher_info->iv_size);
            free(iv);
            //delete from previous hash before adding it to the next
            //because both make use of the same struct fields hh
            HASH_DEL(cube_max_terms, current_max_term);
            int is_valid_max_term = current_max_term->numberOfTerms > 0 && get_bit(linear_results, mask) == 1;
            if(is_valid_max_term && add_max_term(&max_terms, current_max_term, cipher_info->iv_size)){
                printf("\n");
                printf("max_terms->max_term_count %d\n", HASH_COUNT(max_terms));
            }else{
                free_max_term(current_max_term);
            }
        }
        free(zeroed_key_poly);
        free(linear_results);
        number_of_tries++;
    }





    free(dimensions);
    printf("overall max term count %d\n", HASH_COUNT(max_terms));
    return max_terms;}


//0 means is not linear
//1 means it is
uint64_t *mobius_is_super_poly_linear(uint64_t *zeroed_key_super_poly, int *cube_axes, int cube_dimension, const Cipher_info * const cipher_info) {
    uint64_t *results = malloc(sizeof(uint64_t) * ((power(2, cube_dimension)/64)+1));
    for (int i = 0; i < power(2, cube_dimension); i++) {
        set_bit(results, 1, i);
    }
    int number_of_keys = 15;
    uint64_t **keys = malloc(sizeof(size_t) * number_of_keys);
    for (int i = 0; i < number_of_keys; i++) {
        keys[i] = generate_key(cipher_info->key_size);
    }
    //for n key comparisons, rather then computing 3n superpolys
    //reuse old key values in different combinations
    //i.e. key1+key2, key1+key3
    //TRADE OFF: there is a trade off between time and memory
    //better to compute keys 1 to n, then loop and do the linear check
    //but this require storing power(2, axis)*(n+2) values
    //to save space, compute key a, compare to keys 1-n
    //require power(2, axis)*4 values to be stored but superpolys are recomputed multiple times
    for (int key_index_1 = 0; key_index_1 < number_of_keys; key_index_1++) {
        uint64_t *key_1_superpolys = mobius_transform(cube_axes, cube_dimension, MIN_NUM_AXES, keys[key_index_1], cipher_info);
        for (int key_index_2 = key_index_1 + 1; key_index_2 < number_of_keys; key_index_2++) {
            uint64_t *key_2_superpolys = mobius_transform(cube_axes, cube_dimension, MIN_NUM_AXES, keys[key_index_2], cipher_info);
            uint64_t combined_keys[2] = {keys[key_index_1][0] ^ keys[key_index_2][0], keys[key_index_1][1] ^ keys[key_index_2][1]};
            uint64_t *pre_combined_superpolys = mobius_transform(cube_axes, cube_dimension, MIN_NUM_AXES, combined_keys, cipher_info);
            int all_results_non_linear = 0;
            for (int i = 0; i < power(2, cube_dimension); i++) {
                if ((get_bit(pre_combined_superpolys, i) ^ get_bit(zeroed_key_super_poly, i)) !=  (get_bit(key_1_superpolys, i) ^ get_bit(key_2_superpolys, i))) {
                    if (get_bit(results, i) == 1) {
                        all_results_non_linear += 1;
                        set_bit(results, 0, i);
                    }
                }
            }
            free(pre_combined_superpolys);
            free(key_2_superpolys);
            if (all_results_non_linear == power(2, cube_dimension)) {
                free(key_1_superpolys);
                for (int i = 0; i < number_of_keys; i++) {
                    free(keys[i]);
                }
                free(keys);
                return results;
            }
        };
        free(key_1_superpolys);
    }
    for (int i = 0; i < number_of_keys; i++) {
        free(keys[i]);
    }
    free(keys);
    return results;
}

Max_term* mobius_construct_max_terms(uint64_t *zeroed_super_polys, int *dimensions, int dimension_count, const Cipher_info * const cipher_info) {
    uint64_t term_key[2] = {0, 0};
    Max_term *max_terms = NULL;
    //start at one to avoid repeatedly calculating poly for no iv axes
    for (int g = 1; g < power(2, dimension_count); g++) {
        Max_term *max_term = malloc(sizeof(Max_term));
        max_term->iv = iv_from_mask(g, dimensions, dimension_count, cipher_info->iv_size);
        max_term->terms = malloc(sizeof(int) * cipher_info->key_size);
        max_term->numberOfTerms = 0;
        max_term->plusOne = get_bit(zeroed_super_polys, g);
        add_max_term(&max_terms, max_term, cipher_info->iv_size);
    }
    for (int key_bit = 0; key_bit < cipher_info->key_size; key_bit++) {
        //increase the key bit that's set to 1
        term_key[0] = 0;
        term_key[1] = 0;
        set_bit(term_key, 1, key_bit);
        uint64_t *key_super_poly = mobius_transform(dimensions, dimension_count, MIN_NUM_AXES, term_key, cipher_info);
        for (int mask = 1; mask < power(2, dimension_count); mask++) {
            uint64_t* iv = iv_from_mask(mask, dimensions, dimension_count, cipher_info->iv_size);
            Max_term *current_max_term = get_max_term(&max_terms, iv, cipher_info->iv_size);
            if (get_bit(key_super_poly, mask) + get_bit(zeroed_super_polys, mask) == 1) {
                current_max_term->terms[current_max_term->numberOfTerms] = key_bit;
                current_max_term->numberOfTerms++;
            }
            free(iv);
        }
        free(key_super_poly);
    }
    return max_terms;
}

//the returned array will be very large (2^num_of_axis)-min_num_axis
//to save memory, the set of axis that each superpoly is stored implictly
//calculate it by looking at the binary representation of index+power(2, min_num_axis)

uint64_t *mobius_transform(const int const *cube_axis, const int num_of_axis, const int min_num_axes, uint64_t *key, const Cipher_info *const cipher_info) {
    uint64_t derivative_count = power(2, num_of_axis);
    uint64_t* derivative_results = malloc(sizeof(uint64_t) * ((derivative_count/64)+1));
    for (uint64_t axis_mask = 0; axis_mask < derivative_count; axis_mask++) {
        uint64_t *current_iv = iv_from_mask(axis_mask, cube_axis, num_of_axis, cipher_info->iv_size);
        int output[1] = {0};
        cipher_info->run_cipher(output, 1, current_iv, key, cipher_info->init_clocks);
        debug_print("output %d\n", output[0]);
        set_bit(derivative_results, output[0], axis_mask);
        //derivative_results[axis_mask] = output[0];
        free(current_iv);
    }
    //now transform output bits into superpoly bits
    for (int current_dimension = 0; current_dimension < num_of_axis; current_dimension++) {
        int size = (int) power(2, current_dimension);
        for (int pos = 0; pos < power(2, num_of_axis); pos += 2 * size) {
            for (int j = 0; j < size; j++) {
                int new_value = get_bit(derivative_results, pos + j) ^ get_bit(derivative_results, pos + size + j);
                set_bit(derivative_results, new_value, pos + size + j);
            }
        }
    }
    return derivative_results;
}