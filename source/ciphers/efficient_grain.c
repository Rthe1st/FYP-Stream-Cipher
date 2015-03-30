#include<stdio.h>
#include <stdlib.h>

#include "grain.h"
#include "../cipher_io/useful.h"
#include "../generated/power_2_constants.h"
#include "efficient_grain.h"

int and_bits(const uint64_t sum_powers_of_2, const uint64_t bits){
    return (sum_powers_of_2 & bits) == sum_powers_of_2;
}

//register[0] is [63] = 2^63, [62] = 2^62... [0] = 2^0
//register[1] is 2^128^....2^64

void efficientInitialisationClock(Grain_state * state){
    int hBit = efficientH(state->lfsr, state->nlfsr);
    int keyBit = efficientPreOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = efficientLinearFeedback(state->lfsr);
    linearFeedBack = keyBit ^ linearFeedBack;
    int nonLinearFeedback = efficientNonLinearFeeback(state->nlfsr, (int)(state->lfsr[0] & 1));
    nonLinearFeedback = keyBit ^ nonLinearFeedback;
    efficientUpdateSRState(state->lfsr, linearFeedBack);
    efficientUpdateSRState(state->nlfsr, nonLinearFeedback);
}

int efficientProductionClock(Grain_state * state) {
    int hBit = efficientH(state->lfsr, state->nlfsr);
    int keyBit = efficientPreOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = efficientLinearFeedback(state->lfsr);
    int nonLinearFeedback = efficientNonLinearFeeback(state->nlfsr, (int) (state->lfsr[0] & 1));
    efficientUpdateSRState(state->lfsr, linearFeedBack);
    efficientUpdateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("keybit: %d", keyBit);
    debug_print("lfsr state:\n");
    print_uint64_t_array(state->lfsr, 2);
    debug_print("nlfsr state:\n");
    print_uint64_t_array(state->nlfsr, 2);
    return keyBit;
}

void efficientUpdateSRState(uint64_t *shiftRegister, const int newBit){
    shiftRegister[0] = (shiftRegister[0] >> 1) | (shiftRegister[1] << 63);
    shiftRegister[1] = (shiftRegister[1] >> 1) | (((uint64_t)newBit) << 63);
}

int efficientNonLinearFeeback(uint64_t nlfsr[], int last_lfsr_bit) {
    int linear_bit = (int)(1 & (
            (nlfsr[0] >> 0) ^ (nlfsr[0] >> 26) ^ (nlfsr[0] >> 56) ^ (nlfsr[1] >> (91-64)) ^ (nlfsr[1] >> (96-64))
        )
    );
    int non_linear_bit = (int)(1 & (
            (nlfsr[0] >> 3 & nlfsr[1] >> (67-64))
                    ^ and_bits(two_power_11 + two_power_13, nlfsr[0])
                    ^ and_bits(two_power_17 + two_power_18, nlfsr[0])
                    ^ and_bits(two_power_27 + two_power_59, nlfsr[0])
                    ^ and_bits(two_power_40 + two_power_48, nlfsr[0])
                    ^ (nlfsr[0] >> 61 & nlfsr[1] >> (65-64))
                    ^ and_bits(two_power_68_less_64 + two_power_84_less_64, nlfsr[1])
                    ^ and_bits(two_power_88_less_64 + two_power_92_less_64 + two_power_93_less_64 + two_power_95_less_64, nlfsr[1])
                    ^ and_bits(two_power_22 + two_power_24 + two_power_25, nlfsr[0])
                    ^ and_bits(two_power_70_less_64 + two_power_78_less_64 + two_power_82_less_64, nlfsr[1])
        )
    );
    return (linear_bit ^ last_lfsr_bit ^ non_linear_bit);
}

int efficientLinearFeedback(uint64_t* lfsr){
    return (int)(1 & (
            (lfsr[0]) ^ (lfsr[0] >> 7) ^ (lfsr[0] >> 38) ^ (lfsr[1] >> (70-64)) ^ (lfsr[1] >> (81-64)) ^ (lfsr[1] >> (96-64))
        )
    );
}

int efficientH(uint64_t lfsr[], uint64_t nlfsr[]){
    return (int)(1 & (
            (nlfsr[0] >> 12 & lfsr[0] >> 8)
            ^ and_bits(two_power_13 + two_power_20, lfsr[0])
            ^ ((nlfsr[1] >> (95-64)) & (lfsr[0] >> 42))
            ^ (lfsr[0] >> 60 & lfsr[1] >> (79-64))
            ^ (nlfsr[0] >> 12 & nlfsr[1] >> (95-64) & lfsr[1] >> (94-64))
        )
    );
}

int efficientPreOutput(const int h_bit, const uint64_t * const lfsr, const uint64_t * const nlfsr ){
    return h_bit ^ (int) (1 & (
            (lfsr[1] >> (93-64))
            ^ (nlfsr[0] >> 2) ^ (nlfsr[0] >> 15) ^ (nlfsr[0] >> 36) ^ (nlfsr[0] >> 45)
            ^ (nlfsr[1] >> (64-64)) ^ (nlfsr[1] >> (73-64)) ^ (nlfsr[1] >> (89-64))
        )
    );
}

Grain_state setupEfficentGrain(const uint64_t * const iv, const uint64_t * const key, const int clock_number){
    const uint64_t DEFAULT_END_IV_BITS = 0x7fffffff00000000;
    uint64_t* iv_copy = malloc(2*sizeof(uint64_t));
    iv_copy[0] = iv[0]; iv_copy[1] = iv[1]|DEFAULT_END_IV_BITS;
    uint64_t *key_copy = malloc(2*sizeof(uint64_t));
    key_copy[0] = key[0]; key_copy[1] = key[1];
    Grain_state state = {iv_copy, key_copy};
    debug_print("initial state: \n");
    debug_print("lfsr state:\n"); print_uint64_t_array(state.lfsr, 2);
    debug_print("nlfsr state:\n"); print_uint64_t_array(state.nlfsr, 2);
    debug_print("begin clocking\n");
    for(int i = 0; i < clock_number; i++) {
        debug_print("clock number %d\n", i);
        efficientInitialisationClock(&state);
    }

    return state;
}

void efficientInitAndClock(int * const output, const size_t outputSize, const uint64_t * const iv, const uint64_t * const key){
    Grain_state state = setupEfficentGrain(iv, key, GRAIN_FULL_INIT_CLOCKS);
    for(int i=0; i<outputSize; i++){
        int outputIndex = i/4;
        if (i%4 == 0) {
            output[outputIndex] = 0;
        }
        int keyBit = production_clock(&state);
        debug_print("clock number %d\n", i);
        output[outputIndex] = (((unsigned)output[outputIndex]) << 1) | keyBit;
    }
}