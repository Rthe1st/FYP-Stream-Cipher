#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "attack_main.h"
#include "cube_attack.h"
#include "useful.h"
#include "grain.h"

int main(int argc, char **argv) {
    /*if(argc != 2){
        printf("1 argument expected");
        return EXIT_FAILURE;
    }*/
    srand((unsigned int) time(NULL));
    int *numberOfMaxTerms = malloc(sizeof(int));
    printf("finding max terms\n");
    //passing the int* is so hacky, find a better way to get returned array length
    Max_term **max_terms;
    if(argc >= 2 && strcmp(argv[1], "-mobius") == 0) {

    }else{
        max_terms = find_max_terms(numberOfMaxTerms, MAX_TERM_LIMIT, DIMENSION_LIMIT);
    }
    char *file_out_path = "C:\\Users\\User\\Documents\\GitHub\\FYP-Stream-Cipher\\max_terms.txt";
    print_max_terms(max_terms, *numberOfMaxTerms, file_out_path);
    return EXIT_SUCCESS;
};

void print_max_terms(Max_term **max_terms, int number_of_max_terms, char* fileOutPath){
    FILE *outFp = fopen(fileOutPath, "w+");
    printf("number of maxterms found: %d\n", number_of_max_terms);
    fputs("notes all terms form a linear equations (i.e. only maxTerms are found)\n", outFp);
    for (int i = 0; i < number_of_max_terms; i++) {
        fputs("iv: ", outFp);
        uint64_t *iv = max_terms[i]->iv;
        printf("iv pre file print: ");
        printBits(4 * 2, iv);
        for (int bit_index = 0; bit_index < IV_LENGTH; bit_index++) {
            fputc('0' + get_bit(iv, bit_index), outFp);
        }
        fputc('\n', outFp);
        fputs("terms: ", outFp);
        fprintf(outFp, " (%d)", max_terms[i]->numberOfTerms);
        for (int term = 0; term < max_terms[i]->numberOfTerms; term++) {
            fprintf(outFp, "%d ", max_terms[i]->terms[term]);
        }
        fputc('\n', outFp);
        fputs("PlusOne: ", outFp);
        fputc('0' + max_terms[i]->plusOne, outFp);
        fputc('\n', outFp);
    }
    fclose(outFp);
}