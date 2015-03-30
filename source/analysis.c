#include <stdio.h>
#include <math.h>

typedef struct {
    int bitCount;
    int oneCount;
    int error;
} StreamProfile;

StreamProfile profileBits(FILE* fp){
    StreamProfile profile = {0,0,0};
    char nextBit = fgetc (fp);
    while(nextBit  != EOF){
        profile.bitCount++;
        int bit = nextBit - ((int) '0');
        if(bit == 1){
            profile.oneCount++;
        }
        if(bit != 1 && bit != 0){
            printf("file should only contain 1's and 0's. Found a %c", nextBit + (int)'0');
            profile.error = 1;
            return profile;
        }
        nextBit = fgetc (fp);
    }
    fclose(fp);
    return profile;
}

void analyse(StreamProfile profile){
    //calculate chance count of 1's is > x, cumulative distribution function for binomails, adjusted for 0.5 p and n = bits read
   // int erfParam =
}

int main(int argc, char **argv) {
    FILE *fp = fopen("C:\\Users\\User\\Documents\\GitHub\\FYP-Stream-Cipher\\test.txt", "r");//lol unhardcode
    StreamProfile profile = profileBits(fp);
    analyse(profile);
    return 0;
}