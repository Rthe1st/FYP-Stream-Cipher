#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>

#include "minunit.h"
#include "../ciphers/grain.h"
#include "../cipher_io/useful.h"

static int testSetBit() {
    uint64_t testBits[2] = {0, 0};
    set_bit(testBits, 1, 0);
    mu_assert("setBit error, setting index 0 to 1", testBits[0] == 1);
    set_bit(testBits, 0, 0);
    mu_assert("setBit error, setting index 0 to 0", testBits[0] == 0);
    set_bit(testBits, 1, 64);
    mu_assert("setBit error, setting index 64 to 1", testBits[1] == 1);
    set_bit(testBits, 0, 64);
    mu_assert("setBit error, setting index 64 to 0", testBits[1] == 0);
    testBits[0] = 0;
    testBits[1] = 0;
    //set random indexs to 1 (then back to 0)
    printf("testing with all 0s key\n");
    for (int i = 0; i < 10; i++) {
        int randomIndex = rand() % 128;
        set_bit(testBits, 0, randomIndex);
        int testBitElement = randomIndex >= 64;
        printf("setting random bit %d\n", randomIndex);
        set_bit(testBits, 1, randomIndex);
        mu_assert("setBit error, setting random index to 1", testBits[testBitElement] == power(2, randomIndex % 64));
        set_bit(testBits, 0, randomIndex);
        mu_assert("setBit error, setting random index to 0", testBits[testBitElement] == 0);
        //now test setting to 0 whilst already 0
        set_bit(testBits, 0, randomIndex);
        mu_assert("setBit error, setting random index to 0 (already 0)", testBits[testBitElement] == 0);
    }
    testBits[0] = ~testBits[0];
    testBits[1] = ~testBits[1];
    printf("testing with all 1s key\n");
    //set random indexes to 0 (then back to 1)
    for (int i = 0; i < 10; i++) {
        int randomIndex = rand() % 128;
        int testBitElement = randomIndex >= 64;
        printf("setting random bit %d\n", randomIndex);
        uint64_t correctValue = testBits[testBitElement] - power(2, randomIndex % 64);
        set_bit(testBits, 0, randomIndex);
        mu_assert("setBit error, setting random index to 0", testBits[testBitElement] == correctValue);
        correctValue = testBits[testBitElement] + power(2, randomIndex % 64);
        set_bit(testBits, 1, randomIndex);
        mu_assert("setBit error, setting random index to 1", testBits[testBitElement] == correctValue);
        //now test setting to 1 whilst already 1
        set_bit(testBits, 1, randomIndex);
        mu_assert("setBit error, setting random index to 1 (already 1)", testBits[testBitElement] == correctValue);
    }

    return 0;
}

static int testGetBit() {
    uint64_t testBits[2] = {0, 0};
    int fetchedBit = get_bit(testBits, 0);
    mu_assert("getBit error, getting bit index 0, expected 0", fetchedBit == 0);
    testBits[0] = 1;
    fetchedBit = get_bit(testBits, 0);
    mu_assert("getBit error, getting bit index 0, expected 1", fetchedBit == 1);
    testBits[1] = 0;
    fetchedBit = get_bit(testBits, 64);
    mu_assert("getBit error, getting bit index 64, expected 0", fetchedBit == 0);
    testBits[1] = 1;
    fetchedBit = get_bit(testBits, 64);
    mu_assert("getBit error, getting bit index 64, expected 1", fetchedBit == 1);
    //to test setting to 1 and 0, run with all bits as 0 and all bits as 1
    testBits[0] = 0;
    testBits[1] = 0;
    for (int i = 0; i < 10; i++) {
        int randomIndex = rand() % 128;
        printf("getting random bit %d\n", randomIndex);
        fetchedBit = get_bit(testBits, randomIndex);
        mu_assert("getBit error, getting random index", fetchedBit == 0);
    }
    testBits[0] = ~testBits[0];
    testBits[1] = ~testBits[1];
    for (int i = 0; i < 10; i++) {
        int randomIndex = rand() % 128;
        printf("getting random bit %d\n", randomIndex);
        fetchedBit = get_bit(testBits, randomIndex);
        mu_assert("getBit error, getting random index", fetchedBit == 1);
    }
    return 0;
}

static int testVector(uint8_t *expectedOutput, uint64_t *key, uint64_t *iv) {
    size_t outputSize = sizeof expectedOutput / sizeof expectedOutput[0];
    int keyStream[outputSize];
    grainInitAndClock(keyStream, outputSize*4, key, iv, GRAIN_FULL_INIT_CLOCKS);
    printkeyStream(keyStream, outputSize);
    for (int i = 0; i < (sizeof keyStream / sizeof keyStream[0]); i++) {
        if (expectedOutput[i] != keyStream[i]) {
            return 0;
        }
    }
    return 1;
}

static int testGrain() {
    //key:        00000000000000000000000000000000
    //IV :        000000000000000000000000
    //keystream:  c0207f221660650b6a952ae26586136fa0904140c8621cfe8660c0dec0969e9436f4ace92cf1ebb7
    char rawExpectedOutput[] = "c0207f221660650b6a952ae26586136fa0904140c8621cfe8660c0dec0969e9436f4ace92cf1ebb7";
    size_t number_of_output_bits = (sizeof rawExpectedOutput / sizeof rawExpectedOutput[0]);
    uint8_t parsedExpectedOutput[number_of_output_bits];
    hexArrayToBin(parsedExpectedOutput, rawExpectedOutput, number_of_output_bits);
    uint64_t key[2] = {(uint64_t) 0, (uint64_t) 0};
    uint64_t iv[2] = {(uint64_t) 0, (uint64_t) 0};
    mu_assert("grain error, keyStream != expectedOutput, vector 1", testVector(parsedExpectedOutput, key, iv));
    return 0;
}

static int testLinearFeedback() {
    uint64_t lfsr[2] = {0, 0};
    mu_assert("linear feedback failed for all 0", linearFeedback(lfsr) == 0);
    lfsr[0] = 0xffffffffffffffff;
    lfsr[1] = 0xffffffffffffffff;
    mu_assert("linear feedback failed for all 1's", linearFeedback(lfsr) == 0);
    lfsr[0] = power(2, 0);
    lfsr[1] = power(2, 81 - 64);
    mu_assert("linear feedback failed for 1 in lfsr[0] and lfsr[1]", linearFeedback(lfsr) == 0);
    lfsr[0] = 0;
    lfsr[1] = power(2, 81 - 64);
    mu_assert("linear feedback failed for 1 in lfsr[1] 0 in lfsr[0]", linearFeedback(lfsr) == 1);
    lfsr[0] = power(2, 0);
    lfsr[1] = 0;
    mu_assert("linear feedback failed for in lfsr[0] 0 in lfsr[1]", linearFeedback(lfsr) == 1);
    return 0;
}

static int testPreoutput() {
    uint64_t lfsr[2] = {0,0};
    uint64_t nlfsr[2] = {0,0};
    int hBit = 0;
    mu_assert("preoutput failed for all 0", preOutput(hBit, lfsr, nlfsr) == 0);
    lfsr[0] = 0xffffffffffffffff;
    lfsr[1] = 0xffffffffffffffff;
    nlfsr[0] = 0xffffffffffffffff;
    nlfsr[1] = 0xffffffffffffffff;
    hBit = 1;
    mu_assert("preoutput failed for all 1's", preOutput(hBit, lfsr, nlfsr) == 1);
    lfsr[0] = 0;
    lfsr[1] = power(2, 93 - 64);
    nlfsr[0] = 0;
    nlfsr[1] = 0;
    hBit = 0;
    mu_assert("preoutput failed for lfsr[93] = 1", preOutput(hBit, lfsr, nlfsr) == 1);
    lfsr[0] = 0;
    lfsr[1] = 0;
    nlfsr[0] = power(2, 15);
    nlfsr[1] = 0;
    hBit = 0;
    mu_assert("preoutput failed for nlfsr[15] = 1", preOutput(hBit, lfsr, nlfsr) == 1);
    lfsr[0] = 0;
    lfsr[1] = power(2, 93 - 64);
    nlfsr[0] = power(2, 36);
    nlfsr[1] = power(2, 64 - 64) + power(2, 73 - 64) + power(2, 89 - 64);
    hBit = 1;
    mu_assert("preoutput failed for lfsr[93], nfsr[36,64,73,89], hbit = 1", preOutput(hBit, lfsr, nlfsr) == 0);
    return 0;
}

int run_cipher_unit_tests(){
    srand(time(NULL));
    test_case test_cases[5] = {testSetBit, testGetBit, testLinearFeedback, testPreoutput, testGrain};
    return run_cases(test_cases, (sizeof test_cases/ sizeof(test_case)));
}