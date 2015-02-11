#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include "cube_attack.h"
#include "useful.h"
#include "grain.h"

#define MAX_TERM_LIMT 2
#define DIMENSION_LIMIT 3

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
    debug_print("gen_iv\n");
    uint64_t *iv = malloc(sizeof(uint64_t) * 2);
    iv[0] = 0;
    iv[1] = 0;
    debug_print("dimension count %d\n", dimension_count);
    for (int dimension = 0; dimension < dimension_count; dimension++) {
        debug_print("setting dimension[%d] which is %d\n", dimension, dimensions[dimension]);
        set_bit(iv, 1, dimensions[dimension]);
    }
    debug_print("gen iv[0] return:");
    printBits(4, &(iv[0]));
    debug_print(", in dec: %"
            PRIu64
            "\n", iv[0]);
    debug_print("gen iv[1] return:");
    printBits(4, &(iv[1]));
    debug_print(", in dec: %"
            PRIu64
            "\n", iv[1]);
    return iv;
}

int is_super_poly_linear(int *cube_axes, int cube_dimension) {
    int is_linear = 1;
    //100 is arbitrary. Pick a better number based (or change test method) based on research papers
    for (int i = 0; i < 1000; i++) {
        uint64_t *key1 = generate_key();
        uint64_t *key2 = generate_key();
        uint64_t combined_keys[] = {key1[0] ^ key2[0], key1[1] ^ key2[1]};
        int summed_after = get_super_poly_bit(key1, cube_axes, cube_dimension) ^get_super_poly_bit(key2, cube_axes, cube_dimension);
        int summed_before = get_super_poly_bit(combined_keys, cube_axes, cube_dimension);
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
        if (dimensions[dimension_count - 1] == iv_init_length - 1) {
            dimension_count--;
        }
        dimensions[dimension_count - 1]++;
    }
    return max_terms;
}

Max_term *construct_max_term(int *cube_axes, int cube_dimensions) {
    //iv gives a linear super poly, so now find what its terms are
    uint64_t term_key[2] = {0, 0};
    int *terms = malloc(sizeof(int) * key_length);//this could be much smaller, depends on iv
    //find if there's a +1 in the super poly or not
    int plusOne = get_super_poly_bit(term_key, cube_axes, cube_dimensions);
    int numberOfTerms = 0;
    for (int i = 0; i < key_length; i++) {
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
    uint64_t **derivative_ivs = malloc(number_of_derivatives * sizeof(uint64_t));
    for (int i = 0; i < number_of_derivatives; i++) {
        derivative_ivs[i] = calloc(2, sizeof(uint64_t));
    }
    int mask_result[cube_dimension];//only a maximum, wont be this big every time
    //to get all combinations of axes, use this as a binary mask
    for (uint64_t current_mask = 0; current_mask < number_of_derivatives; current_mask++) {
        debug_print("next mask: %"
                PRIu64
                "\n", current_mask);
        //apply mask
        int masked_axis_count = 0;
        for (int axis_index = 0; axis_index < cube_dimension; axis_index++) {
            //extract mask value per axis
            if ((current_mask & power(2, axis_index)) == 1) {
                debug_print("mask_index %d is 1 adding axis %d\n", axis_index, iv_cube_axes[axis_index]);
                mask_result[masked_axis_count] = iv_cube_axes[axis_index];
                masked_axis_count++;
            }
        }
        //used the masked axis as IV to generate a derivative bit
        uint64_t *current_iv = generate_iv(mask_result, masked_axis_count);
        State state = setupGrain(current_iv, key, SETUP_CLOCK_ROUNDS);
        int next_super_bit = production_clock(&state);
        super_poly_bit = super_poly_bit ^ next_super_bit;
        debug_print("get_superbit iv[0]: %"
                PRIu64
                " iv[1]: %"
                PRIu64
                " superbit: %d\n", current_iv[0], current_iv[1], next_super_bit);
    }
    return super_poly_bit;
}

/*
int *moebius_transformation(int *cube_out, int cube_dimension) {
    for (int i = 0; i < cube_dimension; i++) {
        int sz = (int) power(2, i);
        int pos = 0;
        while (pos < power(2, cube_dimension)) {
            for (int j = 0; j < sz - 1; j++) {
                cube_out[pos + sz + j] = cube_out[pos + j] ^ cube_out[pos + sz + j];
            }
            pos = pos + 2 * sz;
        }
    }
    return cube_out;
}*/
/*
void do_mobius_transform() {
    int cube_axis[] = {5, 8, 50, 60};
    int cube_dimension = sizeof cube_axis / sizeof cube_axis[0];
    int derivative_count = power(2, cube_dimension);
    int derivative_results[derivative_count];
    uint64_t key[2] = {0, 0};
    get_super_poly_bit(derivative_results, key, cube_axis, cube_dimension);
    int cube_out[derivative_count];
    for (int i = 0; i < derivative_count; i++) {
        cube_out[i] = derivative_results[i];
    }
    moebius_transformation(derivative_results, cube_dimension);
}*/
int do_cube_attack() {
    srand((unsigned int) time(NULL));
    int *numberOfMaxTerms = malloc(sizeof(int));
    Max_term **max_terms;
    printf("finding max terms\n");
    //passing the int* is so hacky, find a better way to get returned array length
    max_terms = find_max_terms(numberOfMaxTerms, MAX_TERM_LIMT, DIMENSION_LIMIT);
    char *fileOutPath = "C:\\Users\\User\\Documents\\GitHub\\FYP-Stream-Cipher\\max_terms.txt";
    FILE *outFp = fopen(fileOutPath, "w+");
    printf("number of maxterms found: %d\n", *numberOfMaxTerms);
    fputs("notes all terms form a linear equations (i.e. only maxTerms are found)\n", outFp);
    for (int i = 0; i < *numberOfMaxTerms; i++) {
        fputs("iv: ", outFp);
        uint64_t *iv = max_terms[i]->iv;
        printf("iv pre file print: ");
        printBits(4 * 2, iv);
        for (int bit_index = 0; bit_index < iv_length; bit_index++) {
            fputc('0' + get_bit(iv, bit_index), outFp);
        }
        fputc('\n', outFp);
        fputs("terms: ", outFp);
        fprintf(outFp, " (%d)", max_terms[i]->numberOfTerms);
        for (int term = 0; term < max_terms[i]->numberOfTerms; term++) {
            fprintf(outFp, "%d ", max_terms[i]->terms[term]);
        }
        fputc('\n', outFp);
        fputs("PlusOne: ", outFp);
        fputc('0' + max_terms[i]->plusOne, outFp);
        fputc('\n', outFp);
    }
    fclose(outFp);
    return 0;
};