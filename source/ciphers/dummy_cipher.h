#ifndef DUMMY_CIPHER
#define DUMMY_CIPHER

#include <inttypes.h>

#include "cipher_helpers.h"

/*

This is a list of correct formuale for output bits after n clocks
Use to verify cube attack results

Number of init rounds   Recursirve Formulae Formulae
1                       IV[0]KEY[0]+KEY[1]KEY[2]+KEY[3]
IV[0](KEY[0])
+KEY[1]KEY[2]+KEY[3]
2                       IV[1]KEY[1]+KEY[2]KEY[3]+KEY[4]
IV[1]KEY[1]
+KEY[2]KEY[3]+KEY[4]
3                       IV[2]KEY[2]+KEY[3]KEY[4]+BIT1
IV[2](KEY[2])
+IV[0](KEY[0])
+KEY[1]KEY[2]+KEY[3]+KEY[3]KEY[4]
4                       IV[3]KEY[3]+KEY[4]BIT1+BIT2
IV[0]KEY[4]KEY[0]
+IV[1]KEY[1]
+IV[3]KEY[3]
+KEY[2]KEY[3]+KEY[4]+KEY[4]KEY[1]KEY[2]+KEY[4]KEY[3]
5                       IV[4]KEY[4]+BIT1BIT2+BIT3
+IV[0](KEY[0]KEY[2]KEY[3]+KEY[0]+KEY[0]KEY[4])
+IV[1](KEY[1]KEY[1]KEY[2]+KEY[1]KEY[3])
        //cancels to IV[1](KEY[1]KEY[2]+KEY[1]KEY[3])
+IV[1]IV[0](KEY[0]KEY[1])
+IV[2](KEY[2])
+IV[4]KEY[4]
+KEY[4]KEY[1]KEY[2]+KEY[4]KEY[3]+KEY[2]KEY[3]KEY[1]KEY[2]+KEY[2]KEY[3]KEY[3]+KEY[1]KEY[2]+KEY[3]+KEY[3]KEY[4]
        //cancels to KEY[1]KEY[2]+KEY[2]KEY[3]+KEY[3]+KEY[1]KEY[2]KEY[3]+KEY[1]KEY[2]KEY[4]

6                       IV[1]IV[2]BIT1+BIT2BIT3+BIT4
IV[0](KEY[0]KEY[2]KEY[3]+KEY[4]KEY[0]+KEY[4]KEY[0])
        //cancels to IV[0](KEY[0]KEY[2]KEY[3])
+IV[1](KEY[1]KEY[1]KEY[2]+KEY[1]KEY[3]+KEY[1]KEY[3]KEY[4]+KEY[1])
        //cancels to IV[1](KEY[1]KEY[2]+KEY[1]KEY[3]+KEY[1]KEY[3]KEY[4]+KEY[1])
+IV[2](KEY[2]KEY[2]KEY[3]+KEY[2]KEY[4])
        /cancels to IV[2](KEY[2]KEY[3]+KEY[2]KEY[4])
+IV[3]KEY[3]
+IV[0]IV[1](KEY[0]KEY[1])
+IV[1]IV[2](KEY[3])
+IV[0]IV[1]IV[2](KEY[0])
+KEY[4]+KEY[4]KEY[3]+KEY[2]KEY[3]+KEY[4]KEY[1]KEY[2]+KEY[4]KEY[1]KEY[2]+KEY[4]KEY[3]+KEY[4]KEY[3]KEY[4]+KEY[2]KEY[3]KEY[1]KEY[2]+KEY[2]KEY[3]KEY[3]+KEY[2]KEY[3]KEY[3]KEY[4]
        //cancels to KEY[4]+KEY[3]KEY[4]+KEY[1]KEY[2]KEY[3]+KEY[2]KEY[3]KEY[4]
*/

//uint64_t's to make it compatible with get/set bit function etc that use uint64_t
typedef struct Dummy_state{
        uint64_t iv;
        uint64_t key;
} Dummy_state;

Cipher_info* dummy_info();

int dummy_clock(Dummy_state * const state);

void dummy_update_SR(uint64_t * shift_register, const int newBit);

int dummy_feedback(const Dummy_state * const state);

Dummy_state setup_dummy(const uint64_t iv, const uint64_t key, const int clock_number);

void dummy_init_and_clock(int *const output, const size_t outputSize, const uint64_t *const iv, const uint64_t *const key, int init_clocks);
#endif