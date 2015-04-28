#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "./ciphers/grain.h"
#include "./cipher_io/useful.h"
#include "./cipher_io/encoding.h"

//expects arguments to be grain key iv
//writes plain keystream to file
void runGrain(int argc, char *argv[]) {
    if (argc < 4) {
        printf("grain reqiures 2 additional parameters:\nkey\niv\n In that order");
        return;
    }
    char *key = argv[2];
    char *iv = argv[3];
    char *fileOutPath = 0;
    char* fileInPath = 0;
    int binary = 1;//print in binary by default
    int encrypt = 1;//by default encrypt
    for(int i=4; i < argc; i++){
        if(!strcmp(argv[i],"-fo")) {
            if (!(argc <= i + 1 || argv[i+1][0] == '-')) {
                fileOutPath = argv[i + 1];
            }
        }else if(!strcmp(argv[i],"-fi")) {
            if (!(argc <= i + 1 || argv[i+1][0] == '-')) {
                fileInPath = argv[i + 1];
            }
        }else if(!strcmp(argv[i],"-d")){
            encrypt = 0;
        }
    }
    int keyLength = 0;
    while(key[keyLength] != '\0')
        keyLength++;
    if (keyLength!=32) {
        printf("key must be %d characters long (128bits), it's %d long", 32, keyLength);
        return;
    }
    int ivLength = 0;
    while(iv[ivLength] != '\0')
        ivLength++;
    if (ivLength != 24) {
        printf("iv must be %d characters long (128bits), it's %d long", 24, ivLength);
        return;
    }
    uint8_t bin[32];
    hexArrayToBin(bin, key, 32);
    uint64_t bin_key[2] = {0};
    for(int i=0; i<=32; i++){
        int index = 1-(i%(64/4));
        bin_key[index] = bin_key[index] << 4 | bin[i];
    }
    hexArrayToBin(bin, iv, 24);
    uint64_t bin_iv[2] = {0};
    for(int i=0; i<=24; i++){
        int index = 1-(i%(64/4));
        bin_key[index] = bin_key[index] << 4 | bin[i];
    }
    Grain_state state = setupGrain(bin_iv, bin_key, GRAIN_FULL_INIT_CLOCKS);
    //file io defaults
    if(encrypt == 1){
        if(!fileInPath) {
            fileInPath = ".\\plain.txt";
        }
        if(!fileOutPath) {
            fileOutPath = ".\\cipher.txt";
        }
    }else{
        if(!fileInPath)
            fileInPath = ".\\cipher.txt";
        if(!fileOutPath)
            fileOutPath = ".\\plain.txt";
    }
    FILE* inFp = fopen(fileInPath, "r");
    FILE* outFp = fopen(fileOutPath, "w+");
    if(encrypt == 1 && binary == 1) {
        printf("encrypting bin");
        encryptBinary(state, inFp, outFp);
    }else if(encrypt == 0 && binary == 1){
        printf("decrypting bin");
        decryptBinary(state, inFp, outFp);
    }
    fclose(inFp);
    fclose(outFp);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("You must supply the name of a cipher to use");
        return -1;
    }
    if (strcmp(argv[1], "grain") == 0) {
        runGrain(argc, argv);
    } else {
        printf("your cipher is not surported");
        return -1;
    }
    return 0;
}