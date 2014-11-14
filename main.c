#include<stdio.h>
#include <string.h>
#include "grain.h"
#include "useful.h"
#include <math.h>
#include <stdlib.h>

//expects arguments to be grain key iv streamLength
//writes plain keystream to file
//todo: make stream length param optionally a plaintext string, or take input interactivly
//todo: make writting to file or prompt a choice
void runGrain(int argc, char *argv[]) {
    if (argc < 4) {
        printf("grain reqiures 2 additional parameters:\nkey\niv\n In that order");
        return;
    }
    char *key = argv[2];
    char *iv = argv[3];
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
    //state cant be declared in setup function because shift register array will leave dangling pointers
    uint8_t bin[32];
    hexStringToBin(bin, key, 32);
    uint64_t bin_key[2] = {0};
    for(int i=0; i<=32; i++){
        int index = 1-(i%(64/4));
        bin_key[index] = bin_key[index] << 4 | bin[i];
    }
    hexStringToBin(bin, iv, 24);
    uint64_t bin_iv[2] = {0};
    for(int i=0; i<=24; i++){
        int index = 1-(i%(64/4));
        bin_key[index] = bin_key[index] << 4 | bin[i];
    }
    State state = setupGrain(bin_iv, bin_key);
    int streamLength = atoi(argv[4]);
    //file handling
    FILE *fp = fopen("C:\\Users\\User\\Documents\\GitHub\\FYP-Stream-Cipher\\test.txt", "w+");//lol unhardcode
    for (int i = 0; i < streamLength; i++) {
        fputc((char) (((int) '0') + production_clock(&state)), fp);
    }
    fclose(fp);
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