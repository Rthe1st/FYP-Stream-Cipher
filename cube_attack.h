#include <stdint.h>

#ifndef CUBE_ATTACK_H
#define CUBE_ATTACK_H

extern const int SETUP_CLOCK_ROUNDS;
extern const int MAX_TERM_LIMIT;
extern const int DIMENSION_LIMIT;

typedef struct Max_term{
    uint64_t* iv;//make sure these are unsigned
    int* terms;//the key terms that when summed equal the IV's superpoly bit (assuming only linear equations used)
    int plusOne;
    int numberOfTerms;
} Max_term;

typedef struct Max_terms_list {
    Max_term *max_terms;
    int max_term_count;
} Max_terms_list;

//generate random key (not cryptographicly secure)
uint64_t *generate_key();

uint64_t *generate_iv(int *dimensions, int dimension_count);

int is_super_poly_linear(int *cube_axes, int cube_dimension);

Max_terms_list *find_max_terms(int max_term_limit, size_t dimension_limit);

Max_term *construct_max_term(int *cube_axes, int cube_dimensions);

int get_super_poly_bit(uint64_t *key, int *iv_cube_axes, int cube_dimension);
/*
int *moebius_transformation(int *cube_out, int cube_dimension);
void do_mobius_transform();*/

int do_cube_attack();

#endif
