#include <stdlib.h>

#include "dummy_cipher.h"
#include "cipher_helpers.h"
#include "cube_attack.h"
#include "useful.h"

Cipher_info* dummy_info(){
    Cipher_info* cipher_info = malloc(sizeof(*cipher_info));
    cipher_info->key_size = 5;
    cipher_info->iv_size = 5;
    cipher_info->init_clocks = 10;
    cipher_info->run_cipher = &dummy_init_and_clock;
    return cipher_info;
}

int dummy_clock(Dummy_state * const state) {
    int feedback = dummy_feedback(state);
    int iv_feedback = (get_bit(&state->iv, 1) & get_bit(&state->iv, 2));
    dummy_update_SR(&state->iv, iv_feedback);
    dummy_update_SR(&state->key, feedback);
    return feedback;
}

void dummy_update_SR(uint64_t * shift_register, const int newBit){
    *shift_register = (*shift_register >> 1) | (power(2, 4) * newBit);
}

int dummy_feedback(const Dummy_state * const state){
    return (get_bit(&state->iv, 0) & get_bit(&state->key, 0))
            ^(get_bit(&state->key, 1) & get_bit(&state->key, 2))
            ^get_bit(&state->key, 3);
}

Dummy_state setup_dummy(const uint64_t iv, const uint64_t key, const int clock_number) {
    Dummy_state state = {iv, key};
    debug_print("begin clocking\n");
    for(int i = 0; i < clock_number; i++) {
        debug_print("clock number %d\n", i);
        dummy_clock(&state);
    }
    return state;
}

//todo: generalise init and clock, use function pointer to setup and produciton clock instead
//key and iv are uint64_t * to conform to cipher_helper function pointer, but arays should constist of a single element that fits in 7 bits
void dummy_init_and_clock(int *const output, const size_t outputSize, const uint64_t *const iv, const uint64_t *const key, int init_clocks){
    Dummy_state state = setup_dummy(iv[0], key[0], init_clocks);
    debug_print("initilisation done\n");
    for(int i=0; i<outputSize; i++){
        int outputIndex = i/4;
        if (i%4 == 0) {
            output[outputIndex] = 0;
        }
        int keyBit = dummy_clock(&state);
        debug_print("clock number %d\n", i);
        output[outputIndex] = (((unsigned)output[outputIndex]) << 1) | keyBit;
    }
}