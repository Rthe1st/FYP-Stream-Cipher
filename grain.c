#include "grain.h"
#include<stdio.h>
#include "useful.h"

//register[0] is [63] = 2^63, [62] = 2^62... [0] = 2^0
//register[1] is 2^128^....2^64

void initialisation_clock(State* state){
    int hBit = h(state->lfsr, state->nlfsr);
    debug_print("hbit: %d\n", hBit);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    debug_print("keybit: %d\n", keyBit);
    int linearFeedBack = linearFeedback(state->lfsr);
    debug_print("linear feedback bit: %d\n", linearFeedBack);
    linearFeedBack = keyBit ^ linearFeedBack;
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, state->lfsr[0] & 1);
    debug_print("non linear feedback bit: %d\n", nonLinearFeedback);
    nonLinearFeedback = keyBit ^ nonLinearFeedback;
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("lfsr state:\n");
    printState(state->lfsr);
    debug_print("nlfsr state:\n");
    printState(state->nlfsr);
}

int production_clock(State* state) {
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = linearFeedback(state->lfsr);
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, state->lfsr[0] & 1);
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("keybit: %d", keyBit);
    debug_print("lfsr state:\n");
    printState(state->lfsr);
    debug_print("nlfsr state:\n");
    printState(state->nlfsr);
    return keyBit;
}

void updateSRState(uint64_t shiftRegister[], int newBit){
    shiftRegister[0] = (shiftRegister[0] >> 1) | (power(2, 63) * (shiftRegister[1] & 1));
    shiftRegister[1] = (shiftRegister[1] >> 1) | (power(2, 63) * newBit);
}

int nonLinearFeeback(uint64_t nlfsr[], int lastLfsrBit) {
    int linearBit = (int)(!!(1 & nlfsr[0])
            ^ !!(power(2, 26) & nlfsr[0])
            ^ !!(power(2, 56) & nlfsr[0])
            ^ !!(power(2, 91-64) & nlfsr[1])
            ^ !!(power(2, 96-64) & nlfsr[1]));
    int nonLinearBit = ((power(2, 3) & nlfsr[0]) && (power(2, 67-64) & nlfsr[1]))
            ^ ((power(2, 11) & nlfsr[0]) && (power(2, 13) & nlfsr[0]))
            ^ ((power(2, 17) & nlfsr[0]) && (power(2, 18) & nlfsr[0]))
            ^ ((power(2, 27) & nlfsr[0]) && (power(2, 59) & nlfsr[0]))
            ^ ((power(2, 40) & nlfsr[0]) && (power(2, 48) & nlfsr[0]))
            ^ ((power(2, 61) & nlfsr[0]) && (power(2, 65-64) & nlfsr[1]))
            ^ ((power(2, 68-64) & nlfsr[1]) && (power(2, 84-64) & nlfsr[1]))
            ^ ((power(2, 88-64) & nlfsr[1]) && (power(2, 92-64) & nlfsr[1]) && (power(2, 93-64) & nlfsr[1]) && (power(2, 95-64) & nlfsr[1]))
            ^ ((power(2, 22) & nlfsr[0]) && (power(2, 24) & nlfsr[0]) && (power(2, 25) & nlfsr[0]))
            ^ ((power(2, 70-64) & nlfsr[1]) && (power(2, 78-64) & nlfsr[1]) && (power(2, 82-64) & nlfsr[1]));
    //debug_print("last lfsr bit: %d\n", lastLfsrBit);
    //debug_print("linear bit: %d\n", linearBit);
    //debug_print("nonlinear bit: %d\n", nonLinearBit);
    return linearBit ^ lastLfsrBit ^ nonLinearBit;
}

int linearFeedback(uint64_t lfsr[]){
    return (int)(!!(power(2, 0) & lfsr[0])
            ^ !!(power(2, 7) & lfsr[0])
            ^ !!(power(2, 38) & lfsr[0])
            ^ !!(power(2, 70-64) & lfsr[1])
            ^ !!(power(2, 81-64) & lfsr[1])
            ^ !!(power(2, 96-64) & lfsr[1]));
}

int h(uint64_t lfsr[], uint64_t nlfsr[]){
    return (int)(((power(2, 12) & nlfsr[0]) && (power(2, 8) & lfsr[0]))
            ^ ((power(2, 13) & lfsr[0]) && (power(2, 20) & lfsr[0]))
            ^ ((power(2, 95-64) & nlfsr[1]) && (power(2, 42) & lfsr[0]))
            ^ ((power(2, 60) & lfsr[0]) && (power(2, 79-64) & lfsr[1]))
            ^ ((power(2, 12) & nlfsr[0]) && (power(2, 95-64) & nlfsr[1]) && (power(2, 94-64) & lfsr[1])));
}

int preOutput(int hBit, uint64_t lfsr[], uint64_t nlfsr[] ){
    return (int)(!!hBit ^ !!(power(2, 93-64) & lfsr[1])
            ^ !!(power(2, 2) & nlfsr[0])
            ^ !!(power(2, 15) & nlfsr[0])
            ^ !!(power(2, 36) & nlfsr[0])
            ^ !!(power(2, 45) & nlfsr[0])
            ^ !!(power(2, 64-64) & nlfsr[1])
            ^ !!(power(2, 73-64) & nlfsr[1])
            ^ !!(power(2, 89-64) & nlfsr[1]));
}

void printState(uint64_t state[]){
    for(int g = 0; g < 2; g++) {
        uint64_t curState = state[g];
        printBits(8, &curState);
    }
    debug_print("\n");
}

/*iv[] shoudl be given as 32 0's followed by the actual iv*/
void initAndClock(int output[], size_t outputSize, uint64_t iv[], size_t iv_array_size, uint64_t key[], size_t key_array_size){
    uint64_t ivMask[2] = {0, (uint64_t)(power(2, 31)-1) << 32};
    uint64_t lfsr[] = {iv[0], iv[1] | ivMask[1]};
    uint64_t nlfsr[] = {key[0], key[1]};
    State state = {lfsr, nlfsr};
    State* state_p = &state;
    debug_print("initial state: \n");
    debug_print("lfsr state:\n");printState(lfsr);
    debug_print("nlfsr state:\n");printState(nlfsr);
    debug_print("begin clocking\n");
    for(int i = 0; i < 256; i++) {
        debug_print("clock number %d\n", i);
        initialisation_clock(state_p);
    }
    debug_print("initilisation done\n");
    int outputIndex = 0;
    for(int i=0; i< outputSize;i++)
        output[i] = 0;
    for(int i = 0; i < outputSize; i++) {
        for(int bitNo=3; bitNo>=0; bitNo--){
            int keyBit = production_clock(state_p);
            debug_print("clock number %d\n", (i*4)+bitNo);
            debug_print("keyBitNo: %d\n", bitNo);
            output[outputIndex] =  output[outputIndex] | (power(2, bitNo) * keyBit);
        }
        outputIndex++;
    }
}