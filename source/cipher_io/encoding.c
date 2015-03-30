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
    int bitNum = 0;
    while((next = getc(inFp))!= EOF){
        int nextBin = next-'0';
        decodedCharacter = decodedCharacter|((production_clock(&state)^ nextBin)<<bitNum);
        if(bitNum == 7){
            bitNum = 0;
            fputc(decodedCharacter, outFp);
            decodedCharacter = 0;
        }else {
            bitNum++;
        }
    }
}
/* UNTESTED
void encryptHex(int streamLength, Grain_state state, char* plainText, FILE* fp){
    for (int i = 0; plainText[i] != EOF; i++) {
        int bin1 = 0;
        int bin2 = 0;
        for (int bit = 0; bit < 4; bit++) {
            bin1 = bin1 & (production_clock(&state) ^ (plainText[i] & power(2, i)));
        }
        for (int bit = 0; bit < 4; bit++) {
            bin2 = bin2 & (production_clock(&state) ^ ((plainText[i] & power(2, i+4))>>4));
        }
        fputc(binToHex(bin1), fp);
        fputc(binToHex(bin2), fp);
    }
}
void decryptHex(int streamLength, Grain_state state, char* plainText, FILE* fp){
    for (int i = 0; plainText[i] != EOF; i+=2) {
        char decodedCharacter = 'a';
        for(int hex=0; hex<2;hex++){
            int bin = hexToBin(hex);
            for(int bit=0;bit<4;bit++){
                decodedCharacter = decodedCharacter&(production_clock(&state)&((bin&power(2,bit+(4)))<<(hex*4)));
            }
        }
        fputc(decodedCharacter, fp);
    }
}*/