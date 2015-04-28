#include <stdlib.h>
#include "encoding.h"
#include "useful.h"

void encryptBinary(Grain_state state, FILE* inFp, FILE* outFp){
    char next;
    while((next = getc(inFp)) != EOF){
        for (int bit = 0; bit < 8; bit++) {
            int outBit = production_clock(&state) ^ !!(next & power(2, bit));
            fputc('0'+outBit, outFp);
        }
    }
}
void decryptBinary(Grain_state state, FILE* inFp, FILE* outFp){
    char next;
    char decodedCharacter = 0;
    int bitNum = 7;
    while((next = getc(inFp))!= EOF){
        int nextBin = next-'0';
        decodedCharacter = decodedCharacter|((production_clock(&state)^ nextBin)<<bitNum);
        if(bitNum == 0){
            bitNum = 7;
            fputc(decodedCharacter, outFp);
            decodedCharacter = 0;
        }else {
            bitNum--;
        }
    }
}