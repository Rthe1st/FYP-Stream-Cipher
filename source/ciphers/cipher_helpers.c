#include <inttypes.h>
#include <stdlib.h>

#include "cipher_helpers.h"
#include "../cipher_io/useful.h"

void set_bit(uint64_t *bits, const int bit_value, int bit_index){
    //extract the bit we're setting to see if it needs to be changed
    //if so, change it buy adding/subtracting the appropriate power of 2
    const int bits_element = bit_index/64;
    debug_print("bits[0] %"PRIu64" bits[1] %"PRIu64" bit value %d bit index %d\n", bits[0], bits[1], bit_value, bit_index);
    bit_index = bit_index%64;
    const int is_one = (int) (1 & (bits[bits_element] >> bit_index));
    debug_print("isone %d\n", is_one);
    const uint64_t bit_to_add = ((uint64_t) 1) << bit_index;
    if(!is_one && (bit_value == 1)){
        bits[bits_element] += bit_to_add;
    }else if(is_one && (bit_value == 0)){
        bits[bits_element] -= bit_to_add;
    }
    debug_print("bits[0] %"PRIu64" bits[1] %"PRIu64"\n", bits[0], bits[1]);
}

int get_bit(const uint64_t *const bits, const int bit_index){
    debug_print("bits[0] %"PRIu64" bits[1] %"PRIu64" bit index %d\n", bits[0], bits[1], bit_index);
    //to get bit n: shift right n places so bit n is at index 0, then & with 1 to extract it
    return (int)(1 & (bits[bit_index/64] >> (bit_index%64)));
}

//generate random key (not cryptographically secure)
uint64_t *generate_key(int key_size) {
    int number_of_64_bits_needed = (key_size/64) + 1;
    uint64_t *key = malloc(sizeof(uint64_t) * number_of_64_bits_needed);
    for(int i=0;i<number_of_64_bits_needed;i++){
        key[i] = 0;
    }
    for (int keybit = 0; keybit < key_size; keybit++) {
        set_bit(key, rand() % 2, keybit);
    }
    return key;
}

uint64_t *generate_iv(int *dimensions, int dimension_count, int iv_size) {
    int number_of_64_bits_needed = (iv_size/64) +1;
    uint64_t *iv = malloc(sizeof(uint64_t) * number_of_64_bits_needed);
    for(int i=0;i<number_of_64_bits_needed;i++){
        iv[i] = 0;
    }
    for (int dimension = 0; dimension < dimension_count; dimension++) {
        set_bit(iv, 1, dimensions[dimension]);
    }
    return iv;
}

//mask is only uint64_t because computing superpoly for 64 or more dimensions in infeasible
uint64_t *iv_from_mask(const uint64_t mask, const int const *dimensions, const int dimension_count, int iv_size) {
    int mask_result[dimension_count];
    int masked_axis_count = 0;
    for (int axis_index = 0; axis_index < dimension_count; axis_index++) {
        //extract mask value per axis
        if ((mask >> axis_index) & 1) {
            debug_print("mask_index %d is 1 adding axis %d\n", axis_index, dimensions[axis_index]);
            mask_result[masked_axis_count] = dimensions[axis_index];
            masked_axis_count++;
        }
    }
    //used the masked axis as IV to generate a derivative bit
    return generate_iv(mask_result, masked_axis_count, iv_size);
}

void print_uint64_t_array(const uint64_t *const array, int elements){
    for(int g = 0; g < elements; g++) {
        uint64_t curState = array[g];
        printBits(8, &curState);
    }
    debug_print("\n");
    for(int i=0;i<elements;i++){
        debug_print("array[g]: %"PRIu64" bits: \n", array[i]);
    }

}