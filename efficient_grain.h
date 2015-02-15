#ifndef EFFICIENT_GRAIN_H
#define EFFICIENT_GRAIN_H
#include <stdint.h>
#include "grain.h"
#include<stdio.h>
#include "useful.h"
#include "grain.h"

int and_bits(const uint64_t sum_powers_of_2, const uint64_t bits);

void efficientInitialisationClock(State* state);

int efficientProductionClock(State* state);

void efficientUpdateSRState(uint64_t *shiftRegister, const int newBit);

int efficientNonLinearFeeback(uint64_t nlfsr[], int last_lfsr_bit);

int efficientLinearFeedback(uint64_t* lfsr);

int efficientH(uint64_t lfsr[], uint64_t nlfsr[]);

int efficientPreOutput(const int h_bit, const uint64_t * const lfsr, const uint64_t * const nlfsr );

void efficientInitAndClock(int * const output, const size_t outputSize, const uint64_t * const iv, const uint64_t * const key);

#endif