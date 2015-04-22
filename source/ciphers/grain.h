#ifndef GRAIN_H
#define GRAIN_H

#include <stdint.h>

#include "cipher_helpers.h"

typedef struct Grain_state {
    uint64_t* lfsr;
    uint64_t* nlfsr;
} Grain_state;

extern int GRAIN_FULL_INIT_CLOCKS;
extern int NO_H;
extern int NO_LFSR_F;
extern int NO_NLFSR_F;
extern int MAKE_NLFSR_LINEAR;
extern int NO_INIT_LFSR_F;
extern int NO_INIT_NLFSR_F;

Cipher_info* grain_info();

void set_bit(uint64_t * const bits, const int bit_value, int bit_index);

int get_bit(const uint64_t *const bits, const int bit_index);

void initialisation_clock(Grain_state * state);

int production_clock(Grain_state * state);

void updateSRState(uint64_t *shiftRegister, int newBit);

int nonLinearFeeback(const uint64_t * const nlfsr, const int lastLfsrBit);

int linearFeedback(const uint64_t * const lfsr);

int h(const uint64_t * const lfsr, const uint64_t * const nlfsr);

int preOutput(const int hBit, const uint64_t * const lfsr, const uint64_t * const nlfsr);

void grainInitAndClock(int *const output, const size_t outputSize, const uint64_t *const iv, const uint64_t *const key, int initClocks);

Grain_state setupGrain(const uint64_t * const iv, const uint64_t * const key, const int clock_number);

void freeGrainState(Grain_state * state);

#endif
