#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "cube_attack.h"
#include "../cipher_io/useful.h"
#include "../ciphers/cipher_helpers.h"
#include "../ciphers/grain.h"
#include "../ut_lib/uthash.h"

//basic algorithm
//1) Pick an IV
//2) Pick N pairs of keys
//3) For each pair of keys, find super poly for each and the combined one, check if the same
//4) If any aren't the same, not linear, disregard IV
//5) If all match, assume linear. Find super poly form by finding value for key 0 through 111111..11
//6) Store the super polys form
//7) repeat until lots of linear super polys found
///--below is not implemented
//8) Online, find super poly value for each IV with a linear super poly.
//9) solve linear equations

const int MAX_TERM_LIMIT = 10;
const int DIMENSION_LIMIT = 2;

int is_super_poly_linear(int *cube_axes, int cube_dimension, const Cipher_info * const cipher_info) {
    int is_linear = 1;
    //get the constants value, needed because summed_before cancels the 2 cosntants in sums
    //but summed after does not, and so free bit must be canceled manualy
    uint64_t zeroed_key[2] = {0, 0};
    int zeroed_key_poly = get_super_poly_bit(zeroed_key, cube_axes, cube_dimension, cipher_info);
    for (int i = 0; i < 20; i++) {
        uint64_t *key1 = generate_key(cipher_info->key_size);
        uint64_t *key2 = generate_key(cipher_info->key_size);
        uint64_t combined_keys[] = {key1[0] ^ key2[0], key1[1] ^ key2[1]};
        int summed_after = get_super_poly_bit(key1, cube_axes, cube_dimension, cipher_info) ^ get_super_poly_bit(key2, cube_axes, cube_dimension, cipher_info);
        int summed_before = get_super_poly_bit(combined_keys, cube_axes, cube_dimension, cipher_info) ^ zeroed_key_poly;
        free(key1);
        free(key2);
        if (summed_after != summed_before) {
            is_linear = 0;
            break;
        }
    }
    return is_linear;
}

//returns 1 if dimension count increases, 0 otherwise
int increase_dimensions(int *dimensions, int *dimension_count, const Cipher_info * const cipher_info) {
    //dimensions elements should be in ascending order
    for (int i = (*dimension_count) - 1; i >= 0; i--) {
        //starting with largest, check if it can be increased
        if (dimensions[i] != cipher_info->iv_size - (*dimension_count - i)) {
            //if it can be, increase it and make all elements above it sequential
            dimensions[i]++;
            for (int g = i + 1; g < *dimension_count; g++) {
                dimensions[g] = dimensions[g - 1] + 1;
            }
            return 0;
        }
    }
    //if no elements can be increased, more dimensions are needed
    (*dimension_count)++;
    for (int i = 0; i < *dimension_count; i++) {
        dimensions[i] = i;
    }
    return 1;
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

Max_term * find_max_terms(int max_term_limit, size_t dimension_limit, const Cipher_info * const cipher_info) {
    int *dimensions = calloc(dimension_limit+1, sizeof(int));
    int dimension_count = 1;
    Max_term * max_terms = NULL;
    while (HASH_COUNT(max_terms) < max_term_limit && dimension_count <= dimension_limit) {
        clock_t time_taken = clock();
        if (is_super_poly_linear(dimensions, dimension_count, cipher_info)){
            printf("linear test took : %d\n", clock()-time_taken);
            clock_t max_term_construct_time = clock();
            Max_term *potential_max_term = construct_max_term(dimensions, dimension_count, cipher_info);
            printf("max term construction took: %d\n", clock()-max_term_construct_time);
            int added = 0;
            if (potential_max_term->numberOfTerms > 0) {
                if(add_max_term(&max_terms, potential_max_term, cipher_info->iv_size)){
                    printf("max_terms_list->max_term_count %d\n", HASH_COUNT(max_terms));
                    added = 1;
                }
            }
            if(!added){
                free_max_term(potential_max_term);
            }
        }
        printf("Testing IV for maxterm took overall: %d\n", clock()-time_taken);
        increase_dimensions(dimensions, &dimension_count, cipher_info);
    }
    printf("ended\n");
    free(dimensions);
    return max_terms;
}

Max_term * find_max_terms_guessing(int max_term_limit, size_t dimension, int max_number_of_tries, const Cipher_info * const cipher_info) {
    srand(1);
    Max_term * max_terms = NULL;
    int number_of_tries = 0;
    int* dimensions = malloc(dimension*sizeof(int));
    while (HASH_COUNT(max_terms) < max_term_limit && number_of_tries < max_number_of_tries) {
        for(int i =0;i<dimension;i++){
            dimensions[i] = rand()%cipher_info->iv_size;
        }
        clock_t time_taken = clock();
        if (is_super_poly_linear(dimensions, dimension, cipher_info)){
            printf("linear test took : %d\n", clock()-time_taken);
            clock_t max_term_construct_time = clock();
            Max_term *potential_max_term = construct_max_term(dimensions, dimension, cipher_info);
            printf("max term construction took: %d\n", clock()-max_term_construct_time);
            int added = 0;
            if (potential_max_term->numberOfTerms > 0) {
                if(add_max_term(&max_terms, potential_max_term, cipher_info->iv_size)){
                    printf("max_terms_list->max_term_count %d\n", HASH_COUNT(max_terms));
                    added = 1;
                }
            }
            if(!added){
                free_max_term(potential_max_term);
            }
        }
        printf("Testing IV for maxterm took overall: %d\n", clock()-time_taken);
        number_of_tries++;
    }
    printf("ended\n");
    free(dimensions);
    return max_terms;
}

Max_term *construct_max_term(int *cube_axes, int cube_dimensions, const Cipher_info * const cipher_info) {
    //iv gives a linear super poly, so now find what its terms are
    uint64_t term_key[2] = {0, 0};
    int *terms = malloc(sizeof(int) * cipher_info->key_size);//this could be much smaller, depends on iv
    int plusOne = get_super_poly_bit(term_key, cube_axes, cube_dimensions, cipher_info);
    int numberOfTerms = 0;
    for (int i = 0; i < cipher_info->key_size; i++) {
        //increase the key bit that's set to 1
        term_key[0] = 0;
        term_key[1] = 0;
        set_bit(term_key, 1, i);
        if ((plusOne + get_super_poly_bit(term_key, cube_axes, cube_dimensions, cipher_info)) == 1) {
            terms[numberOfTerms] = i;
            numberOfTerms++;
        }
    }
    Max_term *max_term = malloc(sizeof(Max_term));
    max_term->iv = generate_iv(cube_axes, cube_dimensions, cipher_info->iv_size);
    max_term->terms = terms;
    max_term->plusOne = plusOne;
    max_term->numberOfTerms = numberOfTerms;
    return max_term;
}

Max_term * make_max_term(uint64_t * iv, int* terms, int plus_one, int number_of_terms, int iv_size){
    Max_term * max_term = malloc(sizeof(Max_term));
    max_term->iv = malloc(sizeof(uint64_t )*iv_size);
    for(int i = 0; i<iv_size; i++){
        max_term->iv[i] = iv[i];
    }
    max_term->terms = malloc(sizeof(int)*number_of_terms);
    for(int i = 0; i<number_of_terms; i++){
        max_term->terms[i] = terms[i];
    }
    max_term->plusOne = plus_one;
    max_term->numberOfTerms = number_of_terms;
    return max_term;
}

int add_max_term(Max_term ** list, Max_term* max_term, int iv_size){
    Max_term * existing_max_term;
    HASH_FIND(hh, *list, max_term->iv, sizeof(uint64_t)*((iv_size/64)+1), existing_max_term);
    if(existing_max_term == NULL) {
        HASH_ADD_KEYPTR(hh, *list, max_term->iv, sizeof(uint64_t)*((iv_size/64)+1), max_term);
        return 1;
    }else{
        return 0;
    }
}

void delete_hash_and_free(Max_term ** max_terms){
    Max_term * current_max_term, *tmp;
    HASH_ITER(hh, *max_terms, current_max_term, tmp) {
        HASH_DEL(*max_terms,current_max_term);
        free(current_max_term);
    }
}

Max_term *get_max_term(Max_term **max_terms_list, uint64_t *iv, int iv_size){
    Max_term * max_term;
    HASH_FIND(hh, *max_terms_list, iv, sizeof(uint64_t)*((iv_size/64)+1), max_term);
    return max_term;
}

void free_max_term(Max_term * max_term){
    free(max_term->terms);
    free(max_term->iv);
    free(max_term);
}

int get_super_poly_bit(uint64_t *key, int *iv_cube_axes, int cube_dimension, const Cipher_info * const cipher_info) {
    int super_poly_bit = 0;
    //WARNING: this will break if used with more then 64 axes
    //due to size of number-of_derivatives
    uint64_t number_of_derivatives = power(2, cube_dimension);
    for (uint64_t current_mask = 0; current_mask < number_of_derivatives; current_mask++) {
        debug_print("next mask: %"PRIu64"\n", current_mask);
        uint64_t *current_iv = iv_from_mask(current_mask, iv_cube_axes, cube_dimension, cipher_info->iv_size);
        int output[1] = {0};
        cipher_info->run_cipher(output, 1, current_iv, key, cipher_info->init_clocks);
        free(current_iv);
        super_poly_bit = super_poly_bit ^ output[0];
        debug_print("superbit: %d for iv:\n", output[0]);
    }
    return super_poly_bit;
}

void print_max_terms(Max_term *max_terms, char* fileOutPath, const Cipher_info * const cipher_info){
    FILE *outFp = fopen(fileOutPath, "w+");
    fprintf(outFp, "number of maxterms found: %d\n", HASH_COUNT(max_terms));
    fputs("notes all terms form a linear equations (i.e. only maxTerms are found)\n", outFp);
    for(Max_term* current_max_term=max_terms; current_max_term != NULL; current_max_term=current_max_term->hh.next) {
        fputs("iv: ", outFp);
        uint64_t *iv = current_max_term->iv;
        printf("iv pre file print: ");
        printBits(4 * 2, iv);
        for (int bit_index = 0; bit_index < cipher_info->iv_size; bit_index++) {
            fputc('0' + get_bit(iv, bit_index), outFp);
        }
        fputc('\n', outFp);
        fputs("terms: ", outFp);
        fprintf(outFp, " (%d)", current_max_term->numberOfTerms);
        for (int term = 0; term < current_max_term->numberOfTerms; term++) {
            fprintf(outFp, "%d ", current_max_term->terms[term]);
        }
        fputc('\n', outFp);
        fputs("PlusOne: ", outFp);
        fputc('0' + current_max_term->plusOne, outFp);
        fputc('\n', outFp);
    }
    fclose(outFp);
}