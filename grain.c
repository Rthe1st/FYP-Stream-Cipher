#include "grain.h"
#include<stdio.h>
//register[0] is [63] = 2^63, [62] = 2^62... [0] = 2^0
//register[1] is 2^128^....2^64

/*lifted from 'the c programming language'*/
uint64_t power(int base, int n)
{
    uint64_t power = 1;
    for (int i = 1; i <= n; ++i)
        power = power * base;
    return power;
}

void initialisation_clock(State* state){
    int hBit = h(state->lfsr, state->nlfsr);
    printf("hbit: %d\n", hBit);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    printf("keybit: %d\n", keyBit);
    int linearFeedBack = linearFeedback(state->lfsr);
    printf("linear feedback bit: %d\n", linearFeedBack);
    linearFeedBack = keyBit ^ linearFeedBack;
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, state->lfsr[0] & 1);
    printf("non linear feedback bit: %d\n", nonLinearFeedback);
    nonLinearFeedback = keyBit ^ nonLinearFeedback;
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
}

int production_clock(State* state) {
    int hBit = h(state->lfsr, state->nlfsr);
    int keyBit = preOutput(hBit, state->lfsr, state->nlfsr);
    int linearFeedBack = linearFeedback(state->lfsr);
    int nonLinearFeedback = nonLinearFeeback(state->nlfsr, state->lfsr[0] & 1);
    updateSRState(state->lfsr, linearFeedBack);
    updateSRState(state->nlfsr, nonLinearFeedback);
    printf("keybit: %d", keyBit);
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
    printf("last lfsr bit: %d\n", lastLfsrBit);
    printf("linear bit: %d\n", linearBit);
    printf("nonlinear bit: %d\n", nonLinearBit);
    return linearBit ^ lastLfsrBit ^ nonLinearBit;
}

int linearFeedback(uint64_t lfsr[]){
    return (int)(!!(1 & lfsr[0])
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

/*lifted from stackoverflow
http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format*/
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}

void printState(uint64_t state[]){
    for(int g = 0; g < 2; g++) {
        uint64_t curState = state[g];
        printBits(8, &curState);
    }
    putchar('\n');
}

void initAndClock(int outputBytes, uint64_t iv[], uint64_t key[]){
    //add stuff to error if bad size iv or key
    //and put a mask in for iv so the last 32 bits dont need to defined on input
    uint64_t lfsr[] = {iv[0], iv[1]};
    uint64_t nlfsr[] = {key[0], key[1]};
    State state = {lfsr, nlfsr};
    State* state_p = &state;
    printf("initial state: \n");
    printf("lfsr state:\n");
    printState(lfsr);
    printf("nlfsr state:\n");
    printState(nlfsr);
    printf("begin clocking\n");
    for(int i = 0; i < 256; i++) {
        printf("clock number %d\n", i);
        initialisation_clock(state_p);
        printf("lfsr state:\n");
        printState(lfsr);
        printf("nlfsr state:\n");
        printState(nlfsr);
    }
    printf("initilisation down, clocked 256 times\n");
    int byteIndex = 0;
    uint8_t keyBytes[outputBytes];//change to match output param
    for(int i=0; i< outputBytes;i++)
        keyBytes[i] = 0;
    int bitNo = 0;
    for(int i = 0; i < outputBytes*8; i++) {
        printf("clock number %d\n", i);
        int keyBit = production_clock(state_p);
        printf("keyBitNo: %d\n", bitNo);
        printf("byte index: %d\n", byteIndex);
        printf("keyBytes[byteIndex] pre keybit add\n");
        printBits(1, &keyBytes[i]);
        printf("(uint8_t)(power(2, 7-bitNo))\n");
        uint8_t  t = (uint8_t)(power(2, 7-bitNo));
        printBits(1, &t);
        printf("keyBytes[byteIndex] & (power(2, 7-bitNo) %u\n", (unsigned int)(keyBytes[byteIndex] & (uint8_t)(power(2, 7-bitNo))));
        printf("(power(2, 7-bitNo) * keyBit) %d\n", (int)(power(2, 7-bitNo) * keyBit));
        keyBytes[byteIndex] =  keyBytes[byteIndex] | (power(2, 7-bitNo) * keyBit);
        printf("keyBytes[byteIndex] post keybit add\n");
        printBits(1, &keyBytes[i]);
        bitNo += 1;
        if(bitNo %8 == 0 && bitNo != 0) {
            bitNo = 0;
            byteIndex++;
        }
        printf("keybits[i] %d\n", keyBit);
        printf("lfsr state:\n");
        printState(lfsr);
        printf("nlfsr state:\n");
        printState(nlfsr);
    }
    printf("keybits: \n");
    printf("in bin:\n");
    for(int i = 0; i < outputBytes; i++)
        printBits(1, &keyBytes[i]);
    printf("\nin hex:\n");
    for(int i = 0; i < outputBytes; i++)
        printf("%02x ", keyBytes[i]);
    putchar('\n');
}