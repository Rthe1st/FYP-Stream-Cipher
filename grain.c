#include "grain.h"

int power(int a, int b){
    if(b == 0){
        return 0;
    }else{
        return a + power(a, b-1);
    }
}

void initialisation_clock(State* state){
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = keyBit ^ linearFeedback(state->lfsr);
    int nonLinearFeedback = keyBit ^ nonLinearFeeback(state->nlfsr, linearFeedBack);
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
}

int production_clock(State* state) {
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = linearFeedback(state->lfsr);
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, linearFeedBack);
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    return keyBit;
}

void updateSRState(uint64_t shiftRegister[], int newBit){
    shiftRegister[1] = shiftRegister[1] << 1 & (shiftRegister[0] & 1);
    shiftRegister[0] = shiftRegister[0] << 1 &  newBit;
}

int nonLinearFeeback(uint64_t nlfsrState[], int lastLfsrBit) {
    uint64_t linearFeedBackMasks[] = {
            power(2,0) | power(2,26) | power(2,56),
            power(2, 90-64) | power(2, 95-64)
    };
    uint64_t feedbackBits[2];
    int bitCount = 0;
    for(int i=0; i< sizeof(nlfsrState)/sizeof(nlfsrState[0]);i++){
        feedbackBits[i] = nlfsrState[i] & linearFeedBackMasks[i];
        for(int bitIndex = 0; bitIndex < 64; bitIndex++){
            bitCount += 1&feedbackBits[i];
            feedbackBits[i] >>= 1;
        }
    }
    //now non linear
    uint64_t nonLinearFeedbackBit = ((power(2, 3) & nlfsrState[0]) && (power(2, 66) & nlfsrState[1]))
            ^ ((power(2, 11) & nlfsrState[0]) && (power(2, 13) & nlfsrState[0]))
            ^ ((power(2, 17) & nlfsrState[0]) && (power(2, 18) & nlfsrState[0]))
            ^ ((power(2, 27) & nlfsrState[0]) && (power(2, 59) & nlfsrState[0]))
            ^ ((power(2, 48) & nlfsrState[0]) && (power(2, 40) & nlfsrState[0]))
            ^ ((power(2, 61) & nlfsrState[0]) && (power(2, 64-64) & nlfsrState[1]))
            ^ ((power(2, 68-64) & nlfsrState[1]) && (power(2, 84-64) & nlfsrState[1]))
            ^ ((power(2, 88-64) & nlfsrState[1]) && (power(2, 92-64) & nlfsrState[1]) && (power(2, 93-64) & nlfsrState[1]) && (power(2, 95-64) & nlfsrState[1]))
            ^ ((power(2, 22) & nlfsrState[0]) && (power(2, 24) & nlfsrState[0]) && (power(2, 25) & nlfsrState[0]))
            ^ ((power(2, 70-64) & nlfsrState[1]) && (power(2, 78-64) & nlfsrState[1]) && (power(2, 82-64) & nlfsrState[1]));
    return (int)((bitCount + lastLfsrBit + nonLinearFeedbackBit)%2);
}

int linearFeedback(uint64_t lfsrState[]){
    uint64_t feedBackMasks[] = {
            power(2, 7) | power(2,38),
            power(2, 70-64) | power(2,81-64) | power(2,96-64)
    };
    uint64_t feedbackBits[2];
    int bitCount = 0;
    for(int i=0; i< sizeof(lfsrState)/sizeof(lfsrState[0]);i++){
        feedbackBits[i] = lfsrState[i] & feedBackMasks[i];
        for(int bitIndex = 0; bitIndex < 64; bitIndex++){
            bitCount += 1&feedbackBits[i];
            feedbackBits[i] >>= 1;
        }
    }
    return bitCount%2;
}

int h(uint64_t lfsr[], uint64_t nlfsr[]){
    return (int)(((power(2, 12) & nlfsr[0]) && (power(2, 8) & lfsr[0]))
            ^ ((power(2, 13) & lfsr[0]) && (power(2, 20) & lfsr[0]))
            ^ ((power(2, 95-64) & nlfsr[0]) && (power(2, 42) & lfsr[0]))
            ^ ((power(2, 60) & lfsr[0]) && (power(2, 79-64) & lfsr[1]))
            ^ ((power(2, 12) & nlfsr[0]) && (power(2, 95-64) & nlfsr[1]) && (power(2, 94-64) & lfsr[1])));
}

int preOutput(int hBit, uint64_t lfsr[], uint64_t nlfsr[] ){
    return (int)(hBit ^ (power(2, 93-64) | lfsr[1])
            ^ (power(2, 2) | nlfsr[0])
            ^ (power(2, 15) | nlfsr[0])
            ^ (power(2, 36) | nlfsr[0])
            ^ (power(2, 45) | nlfsr[0])
            ^ (power(2, 64-64) | nlfsr[1])
            ^ (power(2, 73-64) | nlfsr[1])
            ^ (power(2, 89-64) | nlfsr[1]))%2;
}
