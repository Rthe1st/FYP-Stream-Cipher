#include<stdio.h>
#include "grain.h"

void printState(uint64_t state[]){
    for(int g = 0; g < 2; g++) {
        uint64_t curState = state[g];
        for (int i = 0; i < 64; i++) {
            printf("%d", (int)(curState & 1));
            state[g] >>= 1;
        }
    }
    putchar('\n');
}

int main()
{
    uint64_t lfsr[] = {0,0};
    uint64_t nlfsr[] = {0,0};
    State state = {lfsr, nlfsr};
    State* state_p = &state;
    for(int i = 0; i < 20; i++) {
        initialisation_clock(state_p);
        printState(lfsr);
        printState(nlfsr);
    }
    int keyBit;
    for(int i = 0; i < 20; i++) {
        keyBit = production_clock(state_p);
        printState(lfsr);
        printState(nlfsr);
        printf("%d\n", keyBit);
    }
    putchar('\n');
    return 0;
}