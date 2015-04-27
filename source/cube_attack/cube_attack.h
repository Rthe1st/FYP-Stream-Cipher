#include <stdint.h>
#include "../ciphers/cipher_helpers.h"
#include "../ut_lib/uthash.h"

#ifndef CUBE_ATTACK_H
#define CUBE_ATTACK_H

extern const int MAX_TERM_LIMIT;
extern const int DIMENSION_LIMIT;

typedef struct Max_term{
    uint64_t* iv;
    int* terms;
    int plusOne;
    int numberOfTerms;
    UT_hash_handle hh;
} Max_term;

int increase_dimensions(int *dimensions, int *dimension_count, const Cipher_info * const cipher_info);

int is_super_poly_linear(int *cube_axes, int cube_dimension, const Cipher_info * const cipher_info);

Max_term *find_max_terms(int max_term_limit, size_t dimension_limit, const Cipher_info * const cipher_info);

Max_term * find_max_terms_guessing(int max_term_limit, size_t dimension, int max_number_of_tries, const Cipher_info * const cipher_info);

Max_term *construct_max_term(int *cube_axes, int cube_dimensions, const Cipher_info * const cipher_info);

Max_term * make_max_term(uint64_t * iv, int* terms, int plus_one, int number_of_terms, int iv_size);

int add_max_term(Max_term ** list, Max_term* max_term, int iv_size);

void delete_hash_and_free(Max_term ** max_terms);

Max_term *get_max_term(Max_term **max_terms_list, uint64_t *iv, int iv_size);

void free_max_term(Max_term * max_term);

int get_super_poly_bit(uint64_t *key, int *iv_cube_axes, int cube_dimension, const Cipher_info * const cipher_info);

void print_max_terms(Max_term *max_terms, char* fileOutPath, const Cipher_info * const cipher_info);

#endif
