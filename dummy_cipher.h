#ifndef DUMMY_CIPHER
#define DUMMY_CIPHER

/*

This is a list of correct formuale for output bits after n clocks
Use to verify cube attack results

Number of init rounds   Recursirve Formulae Formulae
1                       IV[0]KEY[0]+KEY[3]KEY[4]+KEY[6]
Full: IV[0]KEY[0]
        +KEY[3]KEY[4]+KEY[6]
2                       IV[1]KEY[1]+KEY[4]KEY[5]+KEY[7]
Full: IV[1]KEY[1]
        +KEY[4]KEY[5]+KEY[7]
3                       IV[2]KEY[2]+KEY[4]KEY[5]+BIT[1]
Full: IV[2]KEY[2]
        +IV[0]KEY[0]
        +KEY[4]KEY[5]+KEY[3]KEY[4]+KEY[6]
4                       IV[3]KEY[3]+KEY[5]KEY[6]+BIT[2]
Full: IV[3]KEY[3]
        +IV[1]KEY[1]
        +KEY[5]KEY[6]+KEY[4]KEY[5]+KEY[7]
5                       IV[4]KEY[4]+KEY[6]KEY[7]+BIT[3]
Full: IV[4]KEY[4]
        +IV[2]KEY[2]
        +IV[0]KEY[0]
        +KEY[6]KEY[7]+KEY[4]KEY[5]+KEY[3]KEY[4]+KEY[6]
6                       IV[5]KEY[5]+KEY[7]BIT[1]+BIT[4]
Full: IV[5]KEY[5]
        +IV[0]KEY[7]KEY[0]
        +IV[3]KEY[3]
        +IV[1]KEY[1]
        +KEY[5]KEY[6]+KEY[4]KEY[5]+KEY[7]+KEY[7]KEY[3]KEY[4]+KEY[7]KEY[6]
7                       IV[6]KEY[6]+BIT[1]BIT[2]+BIT[5]
Full: IV[6]KEY[6]
+IV[1]KEY[1]KEY[3]KEY[4]+IV[1]KEY[1]KEY[6]
+IV[0](KEY[0]KEY[4]KEY[5]+KEY[0]KEY[7]+KEY[0])
+IV[4]KEY[4]
+IV[2]KEY[2]
+IV[1]IV[0]KEY[1]KEY[0]
+KEY[6]KEY[7]+KEY[3]KEY[4]+KEY[6]+KEY[4]KEY[5]+KEY[3]KEY[4]KEY[4]KEY[5]+KEY[6]KEY[4]KEY[5]+KEY[3]KEY[4]KEY[7]+KEY[6]KEY[7]
8                       IV[7]KEY[7]+BIT[2]BIT[3]+BIT[6]
IV[7]KEY[7]
IV[2](KEY[2]KEY[4]KEY[5]+KEY[2]KEY[7])
+IV[1]KEY[1]KEY[4]KEY[5]
+IV[5]KEY[5]
+IV[0](KEY[0]KEY[7]+KEY[0]KEY[4]KEY[5]+KEY[0]KEY[7])
+IV[3]KEY[3]
+IV[1]IV[2]KEY[1]KEY[2]
+IV[1]IV[0]KEY[1]KEY[0]
+IV[1](KEY[1]+KEY[1]KEY[3]KEY[4]+KEY[1]KEY[6])
+KEY[4]KEY[5]+KEY[7]+KEY[5]KEY[6]+KEY[3]KEY[4]KEY[7]+KEY[6]KEY[7]+KEY[3]KEY[4]KEY[4]KEY[5]+KEY[6]KEY[4]KEY[5]+KEY[4]KEY[5]KEY[4]KEY[5]+KEY[3]KEY[4]KEY[7]+KEY[6]KEY[7]+KEY[4]KEY[5]KEY[7]
9                       IV[3]IV[4]BIT[1]+BIT[3]BIT[4]+BIT[7]
IV[3]IV[4]IV[0]KEY[0]
+IV[3]IV[4]KEY[3]KEY[4]+IV[3]IV[4]KEY[6]
+(IV[2]KEY[2]+KEY[4]KEY[5]+IV[0]KEY[0]+KEY[3]KEY[4]+KEY[6])(IV[3]KEY[3]+KEY[5]KEY[6]+IV[1]KEY[1]+KEY[4]KEY[5]+KEY[7])
+IV[6]KEY[6]
+(IV[0]KEY[0]+KEY[3]KEY[4]+KEY[6])(IV[1]KEY[1]+KEY[4]KEY[5]+KEY[7])
+IV[4]KEY[4]
+IV[2]KEY[2]
+IV[0]KEY[0]
+KEY[3]KEY[4]+KEY[6]+KEY[4]KEY[5]+KEY[6]KEY[7]
10                      IV[4]IV[5]BIT[2]+BIT[4]BIT[5]+BIT[8]
IV[1]IV[4]IV[5]KEY[1]
+IV[4]IV[5](KEY[4]KEY[5]+KEY[7])
+(IV[3]KEY[3]+KEY[5]KEY[6]+IV[1]KEY[1]+KEY[4]KEY[5]+KEY[7])(IV[4]KEY[4]+KEY[6]KEY[7]+IV[2]KEY[2]+KEY[4]KEY[5]+IV[0]KEY[0]+KEY[3]KEY[4]+KEY[6])
+IV[7]KEY[7]
+(IV[1]KEY[1]+KEY[4]KEY[5]+KEY[7])(IV[2]KEY[2]+KEY[4]KEY[5]+IV[0]KEY[0]+KEY[3]KEY[4]+KEY[6])
+IV[5]KEY[5]
+IV[0]KEY[0]KEY[7]
+IV[3]KEY[3]
+IV[1]KEY[1]
+KEY[4]KEY[5]+KEY[7]+KEY[5]KEY[6]+KEY[3]KEY[4]KEY[7]+KEY[6]KEY[7]
*/;

typedef struct Dummy_state{
    unsigned int iv;
    unsigned int key;
} Dummy_state;

int dummy_clock(State * const state);

void dummy_update_SR(unsigned int shift_register, const int newBit);

int dummy_feedback(const Dummy_state state);

Dummy_state setup_dummy(const unsigned int iv, const unsigned int key, const int clock_number);

void dummy_init_and_clock(int *const output, const size_t outputSize, const unsigned int iv, const unsigned int key, int init_clocks);

#endif