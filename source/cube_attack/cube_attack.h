#include <stdint.h>
#include "../ciphers/cipher_helpers.h"

#ifndef CUBE_ATTACK_H
#define CUBE_ATTACK_H

extern const int MAX_TERM_LIMIT;
extern const int DIMENSION_LIMIT;

typedef struct Max_term{
    uint64_t* iv;//make sure these are unsigned
    int* terms;//the key terms that when summed equal the IV's superpoly bit (assuming only linear equations used)
    int plusOne;
    int numberOfTerms;
} Max_term;

typedef struct Max_terms_list {
    Max_term **max_terms;//array of pointers so its easier to free/retain individual max terms
    int max_term_count;
} Max_terms_list;

void increase_dimensions(int *dimensions, int *dimension_count, const Cipher_info * const cipher_info);

int is_super_poly_linear(int *cube_axes, int cube_dimension, const Cipher_info * const cipher_info);

Max_terms_list *find_max_terms(int max_term_limit, size_t dimension_limit, const Cipher_info * const cipher_info);

Max_term *construct_max_term(int *cube_axes, int cube_dimensions, const Cipher_info * const cipher_info);

void free_max_term(Max_term * max_term);

void free_max_term_list(Max_terms_list * max_terms_list);

int get_super_poly_bit(uint64_t *key, int *iv_cube_axes, int cube_dimension, const Cipher_info * const cipher_info);

#endif
