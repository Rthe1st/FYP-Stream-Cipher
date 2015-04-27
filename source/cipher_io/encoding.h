#ifndef ENDCODING_H
#define ENDCODING_H

#include <stdio.h>
#include "../ciphers/grain.h"

void encryptBinary(Grain_state state, FILE* inFp, FILE* outFp);
void decryptBinary(Grain_state state, FILE* inFp, FILE* outFp);
#endif