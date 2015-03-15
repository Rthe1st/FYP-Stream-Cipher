#ifndef CIPHER_HELPERS
#define CIPHER_HELPERS

#include <inttypes.h>

//parameters: output, outputSiz, iv, key, initClocks
typedef void (*init_and_clock)(int *const, const size_t, const uint64_t *const, const uint64_t *const, int);

//to be used in cube attack, a cipher must create return this type
typedef struct Cipher_info{
    int key_size;
    int iv_size;
    int init_clocks;
    init_and_clock run_cipher;
} Cipher_info;

void set_bit(uint64_t *bits, const int bit_value, int bit_index);

int get_bit(const uint64_t *const bits, const int bit_index);

//generate random key (not cryptographically secure)
uint64_t *generate_key(int key_size);

uint64_t *generate_iv(int *dimensions, int dimension_count, int iv_size);

uint64_t *iv_from_mask(const uint64_t mask, const int const *dimensions, const int dimension_count, int iv_size);

void print_uint64_t_array(const uint64_t *const array, int elements);

#endif
