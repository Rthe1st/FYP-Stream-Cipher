#ifndef GRAIN_H
#define GRAIN_H

#include <stdint.h>

typedef struct State{
    uint64_t* lfsr;
    uint64_t* nlfsr;
} State;

extern const int KEY_LENGTH;

extern const int IV_LENGTH;

extern const int LFSR_LENGTH;

extern const int NFSR_LENGTH;

extern const int INT64_IN_KEY;//KEY_LENGTH/64

extern const int INT64_IN_IV;//IV_LENGTH/64

extern const int INIT_CLOCKS;

void set_bit(uint64_t * const bits, const int bit_value, int bit_index);

int get_bit(const uint64_t *const bits, const int bit_index);

void initialisation_clock(State* state);

int production_clock(State* state);

void updateSRState(uint64_t *shiftRegister, int newBit);

int nonLinearFeeback(const uint64_t * const nlfsr, const int lastLfsrBit);

int linearFeedback(const uint64_t * const lfsr);

int h(const uint64_t * const lfsr, const uint64_t * const nlfsr);

int preOutput(const int hBit, const uint64_t * const lfsr, const uint64_t * const nlfsr);

void initAndClock(int *const output, const size_t outputSize, const uint64_t *const iv, const uint64_t *const key);

void printState(const uint64_t * const state);

State setupGrain(const uint64_t * const iv, const uint64_t * const key, const int clock_number);

#endif
