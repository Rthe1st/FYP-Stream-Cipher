#ifndef GRAIN_H
#define GRAIN_H
#include <stdint.h>
typedef struct {
    uint64_t* lfsr;//make sure these are unsigned
    uint64_t* nlfsr;
} State;

uint64_t power(int a, int b);

void initialisation_clock(State* state);

int production_clock(State* state);

void updateSRState(uint64_t shiftRegister[], int newBit);

int nonLinearFeeback(uint64_t nlfsr[], int lastLfsrBit);

int linearFeedback(uint64_t lfsr[]);

int h(uint64_t lfsr[], uint64_t nlfsr[]);

int preOutput(int hBit, uint64_t lfsr[], uint64_t nlfsr[] );

void initAndClock(int outputSize, uint64_t iv[], uint64_t key[]);
#endif