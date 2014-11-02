#ifndef USEFUL_H
#define USEFUL_H

#include<stdio.h>

#define DEBUG 0
//http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
#define debug_print(...) \
            do { if (DEBUG) printf(__VA_ARGS__); } while (0)
uint64_t power(int base, int n);
int hexToBin(char hex);
void printBits(size_t const size, void const * const ptr);
char binToHex(int bin);
void hexStringToBin(uint8_t* bin, char* hex, size_t size);
void printkeyStream(int* keyStream, size_t size);
int xor_bits(uint64_t x);
int and_bits(uint64_t x, uint64_t mask);
#endif