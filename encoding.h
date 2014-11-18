#ifndef ENDCODING_H
#define ENDCODING_H

#include <stdio.h>
#include "grain.h"

void encryptBinary(State state, FILE* inFp, FILE* outFp);
void decryptBinary(State state, FILE* inFp, FILE* outFp);
//void encryptHex(int streamLength, State state, char* plainText, FILE* fp);
//void decryptHex(int streamLength, State state, char* plainText, FILE* fp);
#endif