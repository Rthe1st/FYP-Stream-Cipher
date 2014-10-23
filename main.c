#include<stdio.h>
#include "grain.h"



int main()
{
    //i think iv should int to this, but spec code implementation does not
    //(uint64_t)(power(2, 31)-1) << 32

    //power(2, 32)-1 is spec way
    uint64_t iv[] = {0, (uint64_t)(power(2, 31)-1) << 32};
    uint64_t key[] = {0,0};
    initAndClock(40, iv, key);
    return 0;
}