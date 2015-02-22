#include "dummy_cipher.h"

int dummy_clock(State * const state) {
    int feedback = dummy_feedback(state);
    int iv_feedback = (iv & power(2,3)) & (iv & power(2, 4));
    dummy_update_SR(state->iv, iv_feedback);
    dummy_update_SR(state->key, feedback);
    return feedback;
}

void dummy_update_SR(unsigned int shift_register, const int newBit){
    shift_register = (shift_register >> 1) | (power(2, 7) * newBit);
}

int dummy_feedback(const Dummy_state state){
    return ((iv & power(2, 0)) & (key & power(2, 0)))
            ^ ((key & power(2, 3)) & (key & power(2, 4)))
            ^ (key & power(2, 6));
}

Dummy_state setup_dummy(const unsigned int iv, const unsigned int key, const int clock_number) {
    Dummy_state state = {iv, key};
    debug_print("begin clocking\n");
    for(int i = 0; i < clock_number; i++) {
        debug_print("clock number %d\n", i);
        dummy_clock(&state);
    }
    return state;
}

void dummy_init_and_clock(int *const output, const size_t outputSize, const unsigned int iv, const unsigned int key, int init_clocks){
    Dummy_state state = setup_dummy(iv, key, init_clocks);
    debug_print("initilisation done\n");
    int outputIndex = 0;
    for(int i = 0; i < outputSize; i++) {
        output[i] = 0;
        for(int bitNo=3; bitNo>=0; bitNo--){
            int keyBit = dummy_clock(&state);
            debug_print("clock number %d\n", (i*4)+bitNo);
            debug_print("keyBitNo: %d\n", bitNo);
            output[outputIndex] =  output[outputIndex] | (int)(power(2, bitNo) * keyBit);
        }
        outputIndex++;
    }
}