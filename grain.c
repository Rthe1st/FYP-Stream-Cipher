#include<stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "useful.h"
#include "grain.h"

const int KEY_LENGTH = 128;

const int IV_LENGTH = 96;

const int LFSR_LENGTH = 128;

const int NFSR_LENGTH = 128;

const int INT64_IN_KEY = 2;//KEY_LENGTH/64

const int INT64_IN_IV = 2;//IV_LENGTH/64

const int INIT_CLOCKS = 256;


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

//register[0] is [63] = 2^63, [62] = 2^62... [0] = 2^0
//register[1] is 2^128^....2^64

void initialisation_clock(State * const state){
    int hBit = h(state->lfsr, state->nlfsr);
    debug_print("hbit: %d\n", hBit);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    debug_print("keybit: %d\n", keyBit);
    int linearFeedBack = linearFeedback(state->lfsr);
    debug_print("linear feedback bit: %d\n", linearFeedBack);
    linearFeedBack = keyBit ^ linearFeedBack;
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, get_bit(state->lfsr, 0));
    debug_print("non linear feedback bit: %d\n", nonLinearFeedback);
    nonLinearFeedback = keyBit ^ nonLinearFeedback;
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("lfsr state:\n");
    printState(state->lfsr);
    debug_print("nlfsr state:\n");
    printState(state->nlfsr);
}

int production_clock(State * const state) {
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = linearFeedback(state->lfsr);
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, get_bit(state->lfsr, 0));
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("keybit: %d", keyBit);
    debug_print("lfsr state:\n");
    printState(state->lfsr);
    debug_print("nlfsr state:\n");
    printState(state->nlfsr);
    return keyBit;
}

void updateSRState(uint64_t *shiftRegister, const int newBit){
    shiftRegister[0] = (shiftRegister[0] >> 1) | (power(2, 63) * get_bit(shiftRegister, 64));
    shiftRegister[1] = (shiftRegister[1] >> 1) | (power(2, 63) * newBit);
}

int nonLinearFeeback(const uint64_t * const nlfsr, const int lastLfsrBit) {
    int linearBit = get_bit(nlfsr, 0) ^ get_bit(nlfsr, 26) ^ get_bit(nlfsr, 56) ^ get_bit(nlfsr, 91) ^ get_bit(nlfsr, 96);
    int nonLinearBit = (get_bit(nlfsr, 3) && get_bit(nlfsr, 67))
            ^ (get_bit(nlfsr, 11) && get_bit(nlfsr, 13))
            ^ (get_bit(nlfsr, 17) && get_bit(nlfsr, 18))
            ^ (get_bit(nlfsr, 27) && get_bit(nlfsr, 59))
            ^ (get_bit(nlfsr, 40) && get_bit(nlfsr, 48))
            ^ (get_bit(nlfsr, 61) && get_bit(nlfsr, 65))
            ^ (get_bit(nlfsr, 68) && get_bit(nlfsr, 84))
            ^ (get_bit(nlfsr, 88) && get_bit(nlfsr, 92) && get_bit(nlfsr, 93) && get_bit(nlfsr, 95))
            ^ (get_bit(nlfsr, 22) && get_bit(nlfsr, 24) && get_bit(nlfsr, 25))
            ^ (get_bit(nlfsr, 70) && get_bit(nlfsr, 78) && get_bit(nlfsr, 82));
    return (linearBit ^ lastLfsrBit ^ nonLinearBit);
}

int linearFeedback(const uint64_t * const lfsr){
    return get_bit(lfsr, 0) ^ get_bit(lfsr, 7) ^ get_bit(lfsr, 38) ^ get_bit(lfsr, 70) ^ get_bit(lfsr, 81) ^ get_bit(lfsr, 96);
}

int h(const uint64_t * const lfsr, const uint64_t * const nlfsr){
    return (get_bit(nlfsr, 12) && get_bit(lfsr, 8))
            ^ (get_bit(lfsr, 13) && get_bit(lfsr, 20))
            ^ (get_bit(nlfsr, 95) && get_bit(lfsr, 42))
            ^ (get_bit(lfsr, 60) && get_bit(lfsr, 79))
            ^ (get_bit(nlfsr, 12) && get_bit(nlfsr, 95) && get_bit(lfsr, 94));
}

int preOutput(const int hBit, const uint64_t * const lfsr, const uint64_t * const nlfsr){
    return hBit ^ get_bit(lfsr, 93) ^ get_bit(nlfsr, 2) ^ get_bit(nlfsr, 15) ^ get_bit(nlfsr, 36)
            ^ get_bit(nlfsr, 45) ^ get_bit(nlfsr, 64) ^ get_bit(nlfsr, 73) ^ get_bit(nlfsr, 89);
}

void printState(const uint64_t * const state){
    for(int g = 0; g < 2; g++) {
        uint64_t curState = state[g];
        printBits(8, &curState);
    }
    debug_print("\n");
}

State setupGrain(const uint64_t * const iv, const uint64_t * const key, const int clock_number) {
    const uint64_t DEFAULT_END_IV_BITS = 0x7fffffff00000000;
    uint64_t* iv_copy = malloc(2*sizeof(uint64_t));
    iv_copy[0] = iv[0]; iv_copy[1] = iv[1]|DEFAULT_END_IV_BITS;
    uint64_t *key_copy = malloc(2*sizeof(uint64_t));
    key_copy[0] = key[0]; key_copy[1] = key[1];
    State state = {iv_copy, key_copy};
    debug_print("initial state: \n");
    debug_print("lfsr state:\n"); printState(state.lfsr);
    debug_print("nlfsr state:\n"); printState(state.nlfsr);
    debug_print("begin clocking\n");
    for(int i = 0; i < clock_number; i++) {
        debug_print("clock number %d\n", i);
        initialisation_clock(&state);
    }
    return state;
}

/*iv[] should be given as 32 0's followed by the actual iv*/
void initAndClock(int *const output, const size_t outputSize, const uint64_t *const iv, const uint64_t *const key){
    State state = setupGrain(iv, key, INIT_CLOCKS);
    debug_print("initilisation done\n");
    int outputIndex = 0;
    for(int i = 0; i < outputSize; i++) {
        output[i] = 0;
        for(int bitNo=3; bitNo>=0; bitNo--){
            int keyBit = production_clock(&state);
            debug_print("clock number %d\n", (i*4)+bitNo);
            debug_print("keyBitNo: %d\n", bitNo);
            output[outputIndex] =  output[outputIndex] | (int)(power(2, bitNo) * keyBit);
        }
        outputIndex++;
    }
}
