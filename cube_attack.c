#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <dxgi.h>

#include "cube_attack.h"
#include "useful.h"
#include "cipher_helpers.h"
#include "grain.h"

//basic algorithm
//1) Pick an IV
//2) Pick N pairs of keys
//3) For each pair of keys, find super poly for each and the combined one, check if the same
//4) If any aren't the same, not linear, disregard IV
//5) If all match, assume linear. Find super poly form by finding valure for key 0 through 111111..11
//6) Store the super polys form
//7) repeat until lots of linear super polys found
//8) Online, find super poly value for each IV with a linear super poly.
//9) solve linear equations

const int MAX_TERM_LIMIT = 2;
const int DIMENSION_LIMIT = 2;

Cipher_info* cipher_info;

//change this to reuse keys as suggested in mobius paper
//instead of (key1 key2), (key3,key4) do (key1, key2), (key1, key3)
int is_super_poly_linear(int *cube_axes, int cube_dimension) {
    int is_linear = 1;
    //get free bit, needed because summed_before cancels the 2 to free sums
    //but summed after does not, and so free bit must be canceled manualy
    uint64_t zeroed_key[2] = {0, 0};
    int zeroed_key_poly = get_super_poly_bit(zeroed_key, cube_axes, cube_dimension);
    //number of tests is arbitrary. Pick a better number based (or change test method) based on research papers
    for (int i = 0; i < 1000; i++) {
        uint64_t *key1 = generate_key(cipher_info->key_size);
        uint64_t *key2 = generate_key(cipher_info->key_size);
        uint64_t combined_keys[] = {key1[0] ^ key2[0], key1[1] ^ key2[1]};
        int summed_after = get_super_poly_bit(key1, cube_axes, cube_dimension) ^ get_super_poly_bit(key2, cube_axes, cube_dimension);
        int summed_before = get_super_poly_bit(combined_keys, cube_axes, cube_dimension) ^zeroed_key_poly;
        free(key1);
        free(key2);
        if (summed_after != summed_before) {
            is_linear = 0;
            break;
        }
    }
    return is_linear;
}

void increase_dimensions(int *dimensions, int *dimension_count) {
    //dimensions elements should be in ascending order
    for (int i = (*dimension_count) - 1; i >= 0; i--) {
        //starting with largest, check if it can be increased
        if (dimensions[i] != cipher_info->iv_size - (*dimension_count - i)) {
            //if it can be, increase it and make all elements above it sequential
            dimensions[i]++;
            for (int g = i + 1; g < *dimension_count; g++) {
                dimensions[g] = dimensions[g - 1] + 1;
            }
            return;
        }
    }
    //if no elements can be increased, more dimensions are needed
    (*dimension_count)++;
    for (int i = 0; i < *dimension_count; i++) {
        dimensions[i] = i;
    }
}

//find max terms
//(assuming non-random polynomial (see cube paper)
//start with iv_cube = {0}
//1)is super_poly linear?
//      YES->does it contain linear terms (/is it constant over all single bit key values)?
//            YES->BINGO you found a max term, record and continue
//2)is largest dimension == 127?
//      YES->remove largest dimension
//increase the largest dimension by 1
// go to 1)

Max_terms_list *find_max_terms(int max_term_limit, size_t dimension_limit, Cipher_info* t_cipher_info) {
    cipher_info = t_cipher_info;
    int *dimensions = calloc(dimension_limit, sizeof(int));
    int *dimension_count = malloc(sizeof(int)); *dimension_count = 1;
    Max_terms_list *max_terms_list = malloc(sizeof(Max_terms_list));
    max_terms_list->max_terms = malloc(sizeof(Max_term));
    max_terms_list->max_term_count = 0;
    while (max_terms_list->max_term_count < max_term_limit && *dimension_count < dimension_limit) {
        if (is_super_poly_linear(dimensions, *dimension_count)) {
            Max_term *potential_max_term = construct_max_term(dimensions, *dimension_count);
            if (potential_max_term->numberOfTerms > 0) {
               // max_terms_list->max_terms[max_terms_list->max_term_count] = *potential_max_term;
                //(max_terms_list->max_term_count)++;
                max_terms_list->max_term_count++;
                //todo: use size doubling buffer to save reallocing too much
                max_terms_list->max_terms = realloc(max_terms_list->max_terms, sizeof(Max_term)*max_terms_list->max_term_count);
                max_terms_list->max_terms[max_terms_list->max_term_count-1] = *potential_max_term;
            }
        }
        increase_dimensions(dimensions, dimension_count);
    }
    return max_terms_list;
}

Max_term *construct_max_term(int *cube_axes, int cube_dimensions) {
    //iv gives a linear super poly, so now find what its terms are
    uint64_t term_key[2] = {0, 0};
    int *terms = malloc(sizeof(int) * cipher_info->key_size);//this could be much smaller, depends on iv
    //find if there's a +1 in the super poly or not
    int plusOne = get_super_poly_bit(term_key, cube_axes, cube_dimensions);
    int numberOfTerms = 0;
    for (int i = 0; i < cipher_info->key_size; i++) {
        //increase the key bit that's set to 1
        term_key[0] = 0;
        term_key[1] = 0;
        set_bit(term_key, 1, i);
        if ((plusOne + get_super_poly_bit(term_key, cube_axes, cube_dimensions)) == 1) {
            terms[numberOfTerms] = i;
            numberOfTerms++;
        }
    }
    Max_term *max_term = malloc(sizeof(Max_term));
    max_term->iv = generate_iv(cube_axes, cube_dimensions, cipher_info->iv_size);
    max_term->terms = terms;
    max_term->plusOne = plusOne;
    max_term->numberOfTerms = numberOfTerms;
    debug_print("maxterm->iv: ");
    printBits(4 * cipher_info->iv_size, max_term->iv);
    return max_term;
}

int get_super_poly_bit(uint64_t *key, int *iv_cube_axes, int cube_dimension) {
    int super_poly_bit = 0;
    //WARNING: this will break if used with more then 64 axes
    //due to size of number-of_derivatives
    uint64_t number_of_derivatives = power(2, cube_dimension);
    for (uint64_t current_mask = 0; current_mask < number_of_derivatives; current_mask++) {
        debug_print("next mask: %"PRIu64"\n", current_mask);
        uint64_t *current_iv = iv_from_mask(current_mask, iv_cube_axes, cube_dimension, cipher_info->iv_size);
        int output[1] = {0};
        cipher_info->run_cipher(output, 1, current_iv, key, cipher_info->init_clocks);
        super_poly_bit = super_poly_bit ^ output[0];
        debug_print("superbit: %d for iv:\n", output[0]);
        print_uint64_t_array(current_iv, cipher_info->iv_size/64);
    }
    return super_poly_bit;
}

