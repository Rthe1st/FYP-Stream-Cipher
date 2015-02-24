#ifndef ENDCODING_H
#define ENDCODING_H

#include <stdio.h>
#include "grain.h"

void encryptBinary(Grain_state state, FILE* inFp, FILE* outFp);
void decryptBinary(Grain_state state, FILE* inFp, FILE* outFp);
//void encryptHex(int streamLength, Grain_state state, char* plainText, FILE* fp);
//void decryptHex(int streamLength, Grain_state state, char* plainText, FILE* fp);
#endif