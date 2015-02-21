#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "cube_attack.h"
#include "useful.h"
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

const int SETUP_CLOCK_ROUNDS  = 10;
const int MAX_TERM_LIMIT = 2;
const int DIMENSION_LIMIT = 2;

//generate random key (not cryptographically secure)
uint64_t *generate_key() {
    uint64_t *key = malloc(sizeof(uint64_t) * 2);
    key[0] = 0, key[1] = 0;
    for (int keybit = 0; keybit < 128; keybit++) {
        set_bit(key, rand() % 2, keybit);
    }
    return key;
}

uint64_t *generate_iv(int *dimensions, int dimension_count) {
    uint64_t *iv = malloc(sizeof(uint64_t) * 2);
    iv[0] = 0;
    iv[1] = 0;
    for (int dimension = 0; dimension < dimension_count; dimension++) {
        set_bit(iv, 1, dimensions[dimension]);
    }
    return iv;
}

//mask is expected to be the same form as key/iv (i.e. 2 uint64_ts, where [0] is the lower bits
uint64_t * iv_from_mask(uint64_t * mask, int * dimensions, int dimension_count){
    int mask_result[dimension_count];
    int masked_axis_count = 0;
    for (int axis_index = 0; axis_index < dimension_count; axis_index++) {
        //extract mask value per axis
        if ((mask[axis_index/64] >> axis_index) & 1) {
            debug_print("mask_index %d is 1 adding axis %d\n", axis_index, dimensions[axis_index]);
            mask_result[masked_axis_count] = dimensions[axis_index];
            masked_axis_count++;
        }
    }
    //used the masked axis as IV to generate a derivative bit
    return generate_iv(mask_result, masked_axis_count);
}


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
        uint64_t *key1 = generate_key();
        uint64_t *key2 = generate_key();
        uint64_t combined_keys[] = {key1[0] ^ key2[0], key1[1] ^ key2[1]};
        int summed_after = get_super_poly_bit(key1, cube_axes, cube_dimension) ^ get_super_poly_bit(key2, cube_axes, cube_dimension);
        int summed_before = get_super_poly_bit(combined_keys, cube_axes, cube_dimension) ^ zeroed_key_poly;
        free(key1);
        free(key2);
        if (summed_after != summed_before) {
            is_linear = 0;
            break;
        }
    }
    return is_linear;
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

Max_term **find_max_terms(int *max_term_count, int max_term_limit, size_t dimension_limit) {
    int *dimensions = calloc(dimension_limit, sizeof(int));
    int dimension_count = 1;
    *max_term_count = 0;
    Max_term **max_terms = malloc(sizeof(size_t) * max_term_limit);
    while (*max_term_count < max_term_limit && dimension_count < dimension_limit) {
        if (is_super_poly_linear(dimensions, dimension_count)) {
            Max_term *potential_max_term = construct_max_term(dimensions, dimension_count);
            if (potential_max_term->numberOfTerms > 0) {
                max_terms[*max_term_count] = potential_max_term;
                (*max_term_count)++;
            }
        }
        if (dimensions[dimension_count - 1] == IV_LENGTH - 1) {
            dimension_count--;
        }
        dimensions[dimension_count - 1]++;
    }
    return max_terms;
}

Max_term *construct_max_term(int *cube_axes, int cube_dimensions) {
    //iv gives a linear super poly, so now find what its terms are
    uint64_t term_key[2] = {0, 0};
    int *terms = malloc(sizeof(int) * KEY_LENGTH);//this could be much smaller, depends on iv
    //find if there's a +1 in the super poly or not
    int plusOne = get_super_poly_bit(term_key, cube_axes, cube_dimensions);
    int numberOfTerms = 0;
    for (int i = 0; i < KEY_LENGTH; i++) {
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
    max_term->iv = generate_iv(cube_axes, cube_dimensions);
    max_term->terms = terms;
    max_term->plusOne = plusOne;
    max_term->numberOfTerms = numberOfTerms;
    debug_print("maxterm->iv: ");
    printBits(4 * 2, max_term->iv);
    return max_term;
}

int get_super_poly_bit(uint64_t *key, int *iv_cube_axes, int cube_dimension) {
    int super_poly_bit = 0;
    //WARNING: this will break if used with more then 64 axes
    //due to size of number-of_derivatives
    uint64_t number_of_derivatives = power(2, cube_dimension);
    //currently only current_maks[0] is used
    //[1] will be used to calculate when there are more then 64 axis
    //waiting until I make a type to handle 128 binary digit numbers
    for (uint64_t current_mask[2] = {0,0}; current_mask[0] < number_of_derivatives; current_mask[0]++) {
        debug_print("next mask: %"PRIu64"\n", current_mask[0]);
        uint64_t * current_iv = iv_from_mask(current_mask, iv_cube_axes, cube_dimension);
        State state = setupGrain(current_iv, key, SETUP_CLOCK_ROUNDS);
        int next_super_bit = production_clock(&state);
        super_poly_bit = super_poly_bit ^ next_super_bit;
        debug_print("get_superbit iv[0]: %"PRIu64" iv[1]: %"PRIu64" superbit: %d\n", current_iv[0], current_iv[1], next_super_bit);
    }
    return super_poly_bit;
}

