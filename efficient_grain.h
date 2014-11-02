#ifndef EFFICIENT_GRAIN_H
#define EFFICIENT_GRAIN_H
#include <stdint.h>
#include "grain.h"
#include<stdio.h>
#include "useful.h"
#include "grain.h"

void efficientInitialisationClock(State* state);
int efficientProductionClock(State* state);

int efficientNonLinearFeeback(uint64_t nlfsr[], int lastLfsrBit);

int efficientLinearFeedback(uint64_t* lfsr);

int efficientH(uint64_t lfsr[], uint64_t nlfsr[]);

int efficientPreOutput(int hBit, uint64_t* lfsr, uint64_t* nlfsr );

void efficientInitAndClock(int output[], size_t outputSize, uint64_t iv[], size_t iv_array_size, uint64_t key[], size_t key_array_size);
#endif