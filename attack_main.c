#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "attack_main.h"
#include "cube_attack.h"
#include "mobius_cube_attack.h"
#include "useful.h"
#include "grain.h"

int main(int argc, char **argv) {
    srand((unsigned int) time(NULL));
    printf("finding max terms\n");
    Max_terms_list *max_terms_list;
    Cipher_info * cipher_info = grain_info();
    cipher_info->init_clocks = 5;
    if(argc >= 2 && strcmp(argv[1], "-mobius") == 0) {
        max_terms_list = mobius_find_max_terms(MAX_TERM_LIMIT, DIMENSION_LIMIT, cipher_info);
    }else{
        max_terms_list = find_max_terms(MAX_TERM_LIMIT, DIMENSION_LIMIT, cipher_info);
    }
    char *file_out_path = "C:\\Users\\User\\Documents\\GitHub\\FYP-Stream-Cipher\\max_terms.txt";
    print_max_terms(max_terms_list, file_out_path, cipher_info);
    free(cipher_info);
    for(int i=0; i < max_terms_list->max_term_count; i++){
        free_max_term(max_terms_list->max_terms[i]);
    }
    free_max_term_list(max_terms_list);
    return EXIT_SUCCESS;
};

void print_max_terms(Max_terms_list *max_terms_list, char* fileOutPath, const Cipher_info * const cipher_info){
    FILE *outFp = fopen(fileOutPath, "w+");
    printf("number of maxterms found: %d\n", max_terms_list->max_term_count);
    fputs("notes all terms form a linear equations (i.e. only maxTerms are found)\n", outFp);
    for (int i = 0; i < max_terms_list->max_term_count; i++) {
        Max_term current_max_term = *(max_terms_list->max_terms[i]);
        fputs("iv: ", outFp);
        uint64_t *iv = current_max_term.iv;
        printf("iv pre file print: ");
        printBits(4 * 2, iv);
        for (int bit_index = 0; bit_index < cipher_info->iv_size; bit_index++) {
            fputc('0' + get_bit(iv, bit_index), outFp);
        }
        fputc('\n', outFp);
        fputs("terms: ", outFp);
        fprintf(outFp, " (%d)", current_max_term.numberOfTerms);
        for (int term = 0; term < current_max_term.numberOfTerms; term++) {
            fprintf(outFp, "%d ", current_max_term.terms[term]);
        }
        fputc('\n', outFp);
        fputs("PlusOne: ", outFp);
        fputc('0' + current_max_term.plusOne, outFp);
        fputc('\n', outFp);
    }
    fclose(outFp);
}