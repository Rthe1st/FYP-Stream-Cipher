#ifndef LFSR_H
#define LFSR_H
typedef struct {
    int size;
    int start;
    int end;
    int* feedback_flags;
    int* bits;
} LFSR;

LFSR lfsr_setup(int bits[], int feedback_flags[]);
int lfsr_clock(LFSR* lfsr);
void lfsr_insert(LFSR* lfsr, int bit);

#endif