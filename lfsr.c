#include <math.h>
#include "lfsr.h"
LFSR lfsr_setup(int bits[], int feedback_flags[]){
    LFSR new_lfsr = {
        .size = sizeof(bits) / sizeof(bits[0]),
        .bits = bits,
        //.feedback_flags = feedback_flags,
        .start = 0
    };
    new_lfsr.end = new_lfsr.size-1;
}

int lfsr_clock(LFSR* lfsr){
    int sum = 0;
    for(int i = 0; i < lfsr->size; i++) {
        sum += lfsr->feedback_flags[i] * lfsr->bits[i];
    }
    int feedback = floor(sum / 2.0);
    int output = lfsr->bits[1];
    lfsr_insert(lfsr, feedback);
    return feedback;
}

 void lfsr_insert(LFSR* lfsr, int bit){
     lfsr->start--;
     if(lfsr->start < 0) {
         lfsr->start = lfsr->size - 1;
         lfsr->end = lfsr->start - 1;
     }else if(lfsr->start == 0)
         lfsr->end = lfsr->size-1;
     else
         lfsr->end = lfsr->start-1;
     lfsr->bits[lfsr->start] = bit;
 }
