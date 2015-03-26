#include <stdlib.h>
#include <dxgi.h>

#include "cipher_helpers.h"
#include "mobius_cube_attack.h"
#include "cube_attack.h"
#include "useful.h"

const int MIN_NUM_AXES = 0;

Max_terms_list *mobius_find_max_terms(int max_term_limit, size_t dimension_limit, const Cipher_info * const cipher_info) {
    int *dimensions = calloc(dimension_limit, sizeof(int));
    Max_terms_list *max_terms_list = malloc(sizeof(Max_terms_list));
    max_terms_list->max_terms = malloc(sizeof(Max_term)*0);
    max_terms_list->max_term_count = 0;
    uint64_t zeroed_key[2] = {0, 0};
    for (int i = 0; i < dimension_limit; i++) {
        dimensions[i] = i;
    }
    while (max_terms_list->max_term_count < max_term_limit && dimensions[dimension_limit - 1] < cipher_info->iv_size) {
        uint64_t *zeroed_key_poly = mobius_transform(dimensions, dimension_limit, MIN_NUM_AXES, zeroed_key, cipher_info);
        uint64_t *linear_results = mobius_is_super_poly_linear(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
        Max_terms_list* cube_max_terms = mobius_construct_max_terms(zeroed_key_poly, dimensions, dimension_limit, cipher_info);
        for (int i = 0; i < cube_max_terms->max_term_count; i++) {
            printf("checking cube terms, overall max term count %d\n", max_terms_list->max_term_count);
            Max_term* current_max_term = cube_max_terms->max_terms[i];
            if (current_max_term->numberOfTerms > 0 && get_bit(linear_results, i+1) == 1) {
                max_terms_list->max_term_count++;
                //todo: use size doubling buffer to save reallocing too much
                max_terms_list->max_terms = realloc(max_terms_list->max_terms, sizeof(Max_term) * max_terms_list->max_term_count);
                max_terms_list->max_terms[max_terms_list->max_term_count-1] = current_max_term;
            }else{
                free_max_term(current_max_term);
            }
        }
        free(zeroed_key_poly);
        free(linear_results);
        free_max_term_list(cube_max_terms);
        //this wont search the dimensio space exhaustivly
        //because say cube {1,2,3} and {4,5,6} will be search but not {1,2,4}
        //to correct this, come up with a system for heuistly finding good cubes
        //or searching exhustivly and remebering past sub cubes for reuse
        for (int i = 0; i < dimension_limit; i++) {
            dimensions[i] += dimension_limit;
        }
    }
    free(dimensions);
    printf("overall max term count %d\n", max_terms_list->max_term_count);
    return max_terms_list;
}

//0 means is not linear
//1 means it is
uint64_t *mobius_is_super_poly_linear(uint64_t *zeroed_key_super_poly, int *cube_axes, int cube_dimension, const Cipher_info * const cipher_info) {
    uint64_t *results = malloc(sizeof(uint64_t) * ((power(2, cube_dimension)/64)+1));
    for (int i = 0; i < power(2, cube_dimension); i++) {
        set_bit(results, 1, i);
    }
    //number of tests is arbitrary. Pick a better number based (or change test method) based on research papers
    int number_of_keys = 10;
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

Max_terms_list* mobius_construct_max_terms(uint64_t *zeroed_super_polys, int *dimensions, int dimension_count, const Cipher_info * const cipher_info) {
    uint64_t term_key[2] = {0, 0};
    Max_terms_list *max_terms_list = malloc(sizeof(Max_terms_list));
    max_terms_list->max_term_count = power(2, dimension_count)-1;
    max_terms_list->max_terms = malloc( sizeof(Max_term)*max_terms_list->max_term_count);
    //start at one to avoid repeatedly calculating poly for no iv axes
    for (int g = 1; g < power(2, dimension_count); g++) {
        Max_term *max_term = malloc(sizeof(Max_term));
        max_term->iv = iv_from_mask(g, dimensions, dimension_count, cipher_info->iv_size);
        max_term->terms = malloc(sizeof(int) * cipher_info->key_size);
        max_term->numberOfTerms = 0;
        max_term->plusOne = get_bit(zeroed_super_polys, g);
        max_terms_list->max_terms[g-1] = max_term;
    }
    for (int key_bit = 0; key_bit < cipher_info->key_size; key_bit++) {
        //increase the key bit that's set to 1
        term_key[0] = 0;
        term_key[1] = 0;
        set_bit(term_key, 1, key_bit);
        uint64_t *key_super_poly = mobius_transform(dimensions, dimension_count, MIN_NUM_AXES, term_key, cipher_info);
        for (int g = 1; g < power(2, dimension_count); g++) {
            Max_term *current_max_term = max_terms_list->max_terms[g-1];
            if (get_bit(key_super_poly, g) + get_bit(zeroed_super_polys, g) == 1) {
                current_max_term->terms[current_max_term->numberOfTerms] = key_bit;
                current_max_term->numberOfTerms++;
            }
        }
        free(key_super_poly);
    }
    return max_terms_list;
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

//optimise/choosing next axis set to transform
//1)for each row that isnt linear, its set of axis + (some axis not in original set) should be transformed
//2)for each row that is linear but doesnt have terms, remeber it and never transform any axis set which is a super set

//rule 1 should pick the largest sub set that was non-linear and use that
//then work its way back useing smaller and smaller non-linear sets that were found

//rule 2 can actualy be made use of in the transformation
//for axis {23, 35, 46, 96} if {23, 35} was linear but had no terms (i.e. constant)
//then {23, 35, 46}, {23, 35, 96} and {23, 35, 46, 96} can all be skipped

//it MAY be preferable to use multiple disjoint cubes to get as many "invalid subsets" as possilbe
//prior to rcursing, to reduce the number of axis that can be added each time

//the returned array will be very large (2^num_of_axis)-min_num_axis
//to save memory, the set of axis that each superpoly is stored implictly
//calculate it by looking at the binary representation of index+power(2, min_num_axis)

//min_num_axis is used to saved storage space because low number of axis elements are
//very unlikly to find a maxterm

uint64_t *mobius_transform(const int const *cube_axis, const int num_of_axis, const int min_num_axes, uint64_t *key, const Cipher_info *const cipher_info) {
    uint64_t derivative_count = power(2, num_of_axis);
    uint64_t* derivative_results = malloc(sizeof(uint64_t) * ((derivative_count/64)+1));
    //int *derivative_results = malloc(sizeof(int) * derivative_count);
    //get_super_poly_bit(key, cube_axis, cube_dimension);
    //double check this is correct for algorithm
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