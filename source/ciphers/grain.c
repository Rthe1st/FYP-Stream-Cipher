#include<stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "../cipher_io/useful.h"
#include "grain.h"
#include "cipher_helpers.h"
#include "../generated/power_2_constants.h"

int GRAIN_FULL_INIT_CLOCKS = 256;

int MAKE_NLFSR_LINEAR = 0;


Cipher_info *grain_info() {
    Cipher_info *cipher_info = malloc(sizeof(*cipher_info));
    cipher_info->key_size = 128;
    cipher_info->iv_size = 96;
    cipher_info->init_clocks = 256;
    cipher_info->run_cipher = &grainInitAndClock;
    return cipher_info;
}

//register[0] is [63] = 2^63, [62] = 2^62... [0] = 2^0
//register[1] is 2^128^....2^64

void initialisation_clock(Grain_state *const state) {
    int hBit = h(state->lfsr, state->nlfsr);
    debug_print("hbit: %d\n", hBit);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    debug_print("keybit: %d\n", keyBit);
    int linearFeedBack = linearFeedback(state->lfsr);
    debug_print("linear feedback bit: %d\n", linearFeedBack);
    linearFeedBack = (keyBit ^ linearFeedBack);
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, get_bit(state->lfsr, 0));
    debug_print("non linear feedback bit: %d\n", nonLinearFeedback);
    nonLinearFeedback = (keyBit ^ nonLinearFeedback);
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("lfsr state:\n");
    print_uint64_t_array(state->lfsr, 2);
    debug_print("nlfsr state:\n");
    print_uint64_t_array(state->nlfsr, 2);
}

int production_clock(Grain_state *const state) {
    int hBit = h(state->lfsr, state->nlfsr);// && !NO_H;
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = linearFeedback(state->lfsr);
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, get_bit(state->lfsr, 0));
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("keybit: %d", keyBit);
    debug_print("lfsr state:\n");
    print_uint64_t_array(state->lfsr, 2);
    debug_print("nlfsr state:\n");
    print_uint64_t_array(state->nlfsr, 2);
    return keyBit;
}

void updateSRState(uint64_t *shiftRegister, const int newBit) {
    shiftRegister[0] = (shiftRegister[0] >> 1) | (two_power_63 * get_bit(shiftRegister, 64));
    shiftRegister[1] = (shiftRegister[1] >> 1) | (two_power_127_less_64 * newBit);
}

int nonLinearFeeback(const uint64_t *const nlfsr, const int lastLfsrBit) {
    int linearBit = get_bit(nlfsr, 0) ^get_bit(nlfsr, 26) ^get_bit(nlfsr, 56) ^get_bit(nlfsr, 91) ^get_bit(nlfsr, 96);
    int nonLinearBit = (get_bit(nlfsr, 3) && get_bit(nlfsr, 67))
            ^(get_bit(nlfsr, 11) && get_bit(nlfsr, 13))
            ^(get_bit(nlfsr, 17) && get_bit(nlfsr, 18))
            ^(get_bit(nlfsr, 27) && get_bit(nlfsr, 59))
            ^(get_bit(nlfsr, 40) && get_bit(nlfsr, 48))
            ^(get_bit(nlfsr, 61) && get_bit(nlfsr, 65))
            ^(get_bit(nlfsr, 68) && get_bit(nlfsr, 84))
            ^(get_bit(nlfsr, 88) && get_bit(nlfsr, 92) && get_bit(nlfsr, 93) && get_bit(nlfsr, 95))
            ^(get_bit(nlfsr, 22) && get_bit(nlfsr, 24) && get_bit(nlfsr, 25))
            ^(get_bit(nlfsr, 70) && get_bit(nlfsr, 78) && get_bit(nlfsr, 82));
    if(MAKE_NLFSR_LINEAR) {
        return (linearBit ^ lastLfsrBit);
    }else {
        return (linearBit ^ lastLfsrBit ^ nonLinearBit);
    }
}

int linearFeedback(const uint64_t *const lfsr) {
    return get_bit(lfsr, 0) ^ get_bit(lfsr, 7) ^ get_bit(lfsr, 38) ^ get_bit(lfsr, 70) ^ get_bit(lfsr, 81) ^ get_bit(lfsr, 96);
}

int h(const uint64_t *const lfsr, const uint64_t *const nlfsr) {
    return (get_bit(nlfsr, 12) && get_bit(lfsr, 8))
            ^ (get_bit(lfsr, 13) && get_bit(lfsr, 20))
            ^ (get_bit(nlfsr, 95) && get_bit(lfsr, 42))
            ^ (get_bit(lfsr, 60) && get_bit(lfsr, 79))
            ^ (get_bit(nlfsr, 12) && get_bit(nlfsr, 95) && get_bit(lfsr, 94));
}

int preOutput(const int hBit, const uint64_t *const lfsr, const uint64_t *const nlfsr) {
    return hBit ^ get_bit(lfsr, 93) ^ get_bit(nlfsr, 2) ^ get_bit(nlfsr, 15) ^ get_bit(nlfsr, 36)
               ^ get_bit(nlfsr, 45) ^ get_bit(nlfsr, 64) ^ get_bit(nlfsr, 73) ^ get_bit(nlfsr, 89);
}

Grain_state setupGrain(const uint64_t *const iv, const uint64_t *const key, const int clock_number) {
    const uint64_t DEFAULT_END_IV_BITS = 0x7fffffff00000000;
    uint64_t *iv_copy = malloc(2 * sizeof(uint64_t));
    iv_copy[0] = iv[0];
    iv_copy[1] = iv[1] | DEFAULT_END_IV_BITS;
    uint64_t *key_copy = malloc(2 * sizeof(uint64_t));
    key_copy[0] = key[0];
    key_copy[1] = key[1];
    Grain_state state = {iv_copy, key_copy};
    debug_print("initial state: \n");
    debug_print("lfsr state:\n");
    print_uint64_t_array(state.lfsr, 2);
    debug_print("nlfsr state:\n");
    print_uint64_t_array(state.nlfsr, 2);
    debug_print("begin clocking\n");
    for (int i = 0; i < clock_number; i++) {
        initialisation_clock(&state);
    }
    return state;
}

void grainInitAndClock(int *const output, const size_t outputSize, const uint64_t *const iv, const uint64_t *const key, int initClocks) {
    Grain_state state = setupGrain(iv, key, initClocks);
    debug_print("initilisation done\n");
    for(int i=0; i<outputSize; i++){
        int outputIndex = i/4;
        if (i%4 == 0) {
            output[outputIndex] = 0;
        }
        int keyBit = production_clock(&state);
        debug_print("clock number %d\n", i);
        output[outputIndex] = (((unsigned)output[outputIndex]) << 1) | keyBit;
    }
    freeGrainState(&state);
}

void freeGrainState(Grain_state * state){
    free(state->lfsr);
    free(state->nlfsr);
}