#include <stdint.h>
#include "useful.h"

/*lifted from 'the c programming language'*/
uint64_t power(int base, int n)
{
    uint64_t power = 1;
    for (int i = 1; i <= n; ++i)
        power = power * base;
    return power;
}

int hexToBin(char hex){
    switch(hex){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': return 10;
        case 'b': return 11;
        case 'c': return 12;
        case 'd': return 13;
        case 'e': return 14;
        case 'f': return 15;
        default: return -1;
    }
};

char binToHex(int bin){
    switch(bin){
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        case 10: return 'a';
        case 11: return 'b';
        case 12: return 'c';
        case 13: return 'd';
        case 14: return 'e';
        case 15: return 'f';
        default: return 'z';
    }
};

void hexStringToBin(uint8_t* bin, char* hex, size_t size){
    for(size_t i=0; i<size; i++){
        bin[i] = (uint8_t)hexToBin(hex[i]);
    }
}

void binStringToHex(char* hex, uint8_t* bin, size_t size){
    for(size_t i=0; i<size; i++){
        hex[i] = (uint8_t)binToHex(bin[i]);
    }
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
            debug_print("%u", byte);
        }
    }
    debug_print("\n");
}

void printkeyStream(int* keyStream, size_t size) {
    debug_print("keybits: \n");
    debug_print("in bin:\n");
    for (int i = 0; i < size; i++)
        printBits(1, &keyStream[i]);
    debug_print("\nin hex:\n");
    for (int i = 0; i < size; i++)
        debug_print("%01x ", keyStream[i]);
    debug_print("\n");
}

//http://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation

//This is better when most bits in x are 0
//It uses 3 arithmetic operations and one comparison/branch per "1" bit in x.
//x-1 makes the lowest 1 bit a 0 and all lower 0's into 1's
//and'ing this with x means the lowest 1 and all subsequent 0's become 0
//so whole 'lumps' of zeros are processed at once, good for when only a few 1's
int popcount_4(uint64_t x) {
    int count;
    for (count=0; x; count++)
        x &= x-1;
    return count;
}

//the above modded for doing xor
//0^0...^0 always gives 0
//and 1^0 = 1, so only the number of 1's matters
int xor_bits(uint64_t x) {
    int result = 0;
    while(x) {
        x &= x - 1;
        result = !result;
    }
    return result;
}

int and_bits(uint64_t x, uint64_t mask){
    return mask == (x&mask);
}