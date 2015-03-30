#ifndef MOBIUS_CUBE_ATTACK_H
#define MOBIUS_CUBE_ATTACK_H

#include <stdint.h>
#include "cube_attack.h"

Max_terms_list* mobius_construct_max_terms(uint64_t *zeroed_super_polys, int *dimensions, int dimension_count, const Cipher_info * const cipher_info);

uint64_t *mobius_is_super_poly_linear(uint64_t* zeroed_key_super_poly, int *cube_axes, int cube_dimension, const Cipher_info * const cipher_info);

Max_terms_list *mobius_find_max_terms(int max_term_limit, size_t dimension_limit, const Cipher_info * const cipher_info);

uint64_t *mobius_transform(const int const *cube_axis, const int num_of_axis, const int min_num_axes, uint64_t *key, const Cipher_info *const cipher_info);

void increase_dimensions_fixed_limit(int *dimensions, int dimension_count, const Cipher_info * const cipher_info);

#endif