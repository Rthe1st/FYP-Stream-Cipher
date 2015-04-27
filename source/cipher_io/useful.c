#include <stdint.h>
#include "useful.h"

/*copied from 'the c programming language'*/
uint64_t power(int base, int n) {
    uint64_t power = 1;
    for (int i = 1; i <= n; ++i)
        power = power * base;
    return power;
}

int hexToBin(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    }else if (hex >= 'a' && hex <= 'f') {
        return (hex - 'a') + 10;//(offset from 'a') plus a's value in hexadecimal
    }else{
        return -1;
    }
};

char binToHex(int bin) {
    if (bin >= 0 && bin <= 9)
        return (char) (bin + '0');
    else if (bin >= 10 && bin <= 15)
        return (char) (bin + 'a');
    else
        return 'z';
};

//takes an array of valid hexadecimal chars ['a', '1', 'd']
//converts them into an array of integers, each representing a 4 digit binary number [10, 1, 13]
//you cannot convert from hexadecimal to decimal one character at a time
void hexArrayToBin(uint8_t *bin, char *hex, size_t size) {
    for (size_t i = 0; i < size; i++) {
        bin[i] = (uint8_t) hexToBin(hex[i]);
    }
}

//opposite to hexArraytoBin, same caveats apply
//each binary element 4 bits.
void binArrayToHex(char *hex, uint8_t *bin, size_t size) {
    for (size_t i = 0; i < size; i++) {
        hex[i] = (uint8_t) binToHex(bin[i]);
    }
}

/*from stackoverflow
http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format*/
void printBits(size_t const size, void const *const ptr) {
    unsigned char *b = (unsigned char *) ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = b[i] & (1 << j);
            byte >>= j;
            debug_print("%u", byte);
        }
    }
    debug_print("\n");
}

void printkeyStream(int *keyStream, size_t size) {
    printf("keybits: \n");
    printf("in bin:\n");
    for (int i = 0; i < size; i++)
        printBits(1, &keyStream[i]);
    printf("\nin hex:\n");
    for (int i = 0; i < size; i++)
        printf("%01x ", keyStream[i]);
    printf("\n");
}