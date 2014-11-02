#include<stdio.h>
#include <string.h>
#include "grain.h"
#include "useful.h"
#include <math.h>


void runGrain(int argc, char* argv[]){
    if(argc != 5){
        printf("grain reqiures 3 additional parameters:\nplain text\nkey\niv\n In that order");
        return;
    }
    char* plainText = argv[2];
    char* key = argv[3];
    char* iv = argv[4];
    int outputSize = sizeof plainText/ sizeof plainText[0];
    int keyStream[outputSize];
    initAndClock(keyStream, outputSize, key, 96, iv, 128);
    /*convert input to hex (2 hex per character)
    then xor
     */
}


int main(int argc, char* argv[]) {
    if (argc == 0) {
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