#include "grain.h"
#include<stdio.h>
#include "useful.h"
#include "efficient_grain.h"

//register[0] is [63] = 2^63, [62] = 2^62... [0] = 2^0
//register[1] is 2^128^....2^64

void efficientInitialisationClock(State* state){
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = efficientPreOutput(hBit, state->lfsr, state->nlfsr);//efficientP
    int linearFeedBack = efficientLinearFeedback(state->lfsr);
    linearFeedBack = keyBit ^ linearFeedBack;
    int nonLinearFeedback = efficientNonLinearFeeback(state->nlfsr, state->lfsr[0] & 1);
    nonLinearFeedback = keyBit ^ nonLinearFeedback;
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
}

int efficientProductionClock(State* state) {
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = efficientPreOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = efficientLinearFeedback(state->lfsr);
    int nonLinearFeedback = efficientNonLinearFeeback(state->nlfsr, state->lfsr[0] & 1);
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    debug_print("keybit: %d", keyBit);
    debug_print("lfsr state:\n");
    printState(state->lfsr);
    debug_print("nlfsr state:\n");
    printState(state->nlfsr);
    return keyBit;
}

int efficientNonLinearFeeback(uint64_t nlfsr[], int lastLfsrBit) {
    int linearBit = xor_bits(0x100000004000001 & nlfsr[0]) ^ xor_bits(0x108000000 & nlfsr[1]);
    int nonLinearBit = ((power(2, 3) & nlfsr[0]) && (power(2, 67-64) & nlfsr[1]))
            ^ and_bits(nlfsr[0], 0x2800)
            ^ and_bits(nlfsr[0], 0x60000)
            ^ and_bits(nlfsr[0], 0x800000008000000)
            ^ and_bits(nlfsr[0], 0x1010000000000)
            ^ ((power(2, 61) & nlfsr[0]) && (power(2, 65-64) & nlfsr[1]))
            ^ and_bits(nlfsr[1], 0x100010)
            ^ and_bits(nlfsr[1], 0xb1000000)
            ^ and_bits(nlfsr[0],0x3400000)
            ^ and_bits(nlfsr[1], 0x44040);
    return linearBit ^ lastLfsrBit ^ nonLinearBit;
}

int efficientLinearFeedback(uint64_t* lfsr){
    return xor_bits(0x4000000081 & lfsr[0]) ^ xor_bits(0x100020040 & lfsr[1]);
}

int efficientH(uint64_t lfsr[], uint64_t nlfsr[]){
    return (int)(((power(2, 12) & nlfsr[0]) && (power(2, 8) & lfsr[0]))
            ^ ((power(2, 13) & lfsr[0]) && (power(2, 20) & lfsr[0]))
            ^ ((power(2, 95-64) & nlfsr[1]) && (power(2, 42) & lfsr[0]))
            ^ ((power(2, 60) & lfsr[0]) && (power(2, 79-64) & lfsr[1]))
            ^ ((power(2, 12) & nlfsr[0]) && (power(2, 95-64) & nlfsr[1]) && (power(2, 94-64) & lfsr[1])));
}

int efficientPreOutput(int hBit, uint64_t* lfsr, uint64_t* nlfsr ){
    return hBit ^ !!(power(2, 93-64) & lfsr[1]) ^ xor_bits(0x201000008004&nlfsr[0]) ^ xor_bits(0x2000201&nlfsr[1]);
}

/*iv[] shoudl be given as 32 0's followed by the actual iv*/
void efficientInitAndClock(int output[], size_t outputSize, uint64_t iv[], size_t iv_array_size, uint64_t key[], size_t key_array_size){
    uint64_t ivMask[2] = {0, (uint64_t)(power(2, 31)-1) << 32};
    uint64_t lfsr[] = {iv[0], iv[1] | ivMask[1]};
    uint64_t nlfsr[] = {key[0], key[1]};
    State state = {lfsr, nlfsr};
    State* state_p = &state;
    for(int i = 0; i < 256; i++) {
        efficientInitialisationClock(state_p);
    }
    int outputIndex = 0;
    for(int i=0; i< outputSize;i++)
        output[i] = 0;
    for(int i = 0; i < outputSize; i++) {
        for(int bitNo=3; bitNo>=0; bitNo--){
            int keyBit = efficientProductionClock(state_p);
            output[outputIndex] =  output[outputIndex] | (power(2, bitNo) * keyBit);
        }
        outputIndex++;
    }
}