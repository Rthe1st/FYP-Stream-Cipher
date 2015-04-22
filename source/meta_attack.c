#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>

#include "./ciphers/dummy_cipher.h"
#include "./cube_attack/cube_attack.h"
#include "./cube_attack/mobius_cube_attack.h"
#include "./cipher_io/useful.h"
#include "./ciphers/grain.h"
#include "./ciphers/cipher_helpers.h"

void find_max_terms_for_inits(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info);
void find_max_terms_for_inits_random(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info, int guesses);

int main(int argc, char **argv) {
    printf("finding max terms\n");
    //standard
    char dummy_fp[37] = ".\\dummy_exhaustive_max_terms_000.txt";
    Cipher_info* cipher_info = dummy_info();
    printf("dummy exhaustive\n");
    find_max_terms_for_inits(dummy_fp, 37-8, 5, 5, cipher_info);
    free(cipher_info);
    char grain_full_fp[37] = ".\\grain_exhaustive_max_terms_000.txt";
    cipher_info = grain_info();
    printf("grain exhaustive\n");
    find_max_terms_for_inits(grain_full_fp, 37-8, 256, 5, cipher_info);
    free(cipher_info);
    char dummy_random_fp[32] = ".\\dummy_guess_max_terms_000.txt";
    cipher_info = dummy_info();
    printf("dummy guess\n");
    find_max_terms_for_inits_random(dummy_random_fp, 32-8, 5, 5, cipher_info, 10);
    free(cipher_info);
    char grain_random_fp[32] = ".\\grain_guess_max_terms_000.txt";
    cipher_info = grain_info();
    printf("grain guess\n");
    find_max_terms_for_inits_random(grain_random_fp, 32-8, 256, 1, cipher_info, 10);
    free(cipher_info);
    //begin the lame
    cipher_info = grain_info();
    //no h
    NO_H = 1; NO_LFSR_F = 0; NO_NLFSR_F = 0;
    MAKE_NLFSR_LINEAR = 0; NO_INIT_LFSR_F = 0; NO_INIT_NLFSR_F = 0;
    char grain_no_h_fp[31] = ".\\grain_no_h_max_terms_000.txt";
    printf("grain no_h\n");
    find_max_terms_for_inits(grain_no_h_fp, 31-8, 256, 5, cipher_info);
    char grain_no_h_random_fp[37] = ".\\grain_no_h_guess_max_terms_000.txt";
    printf("grain no_h rand\n");
    find_max_terms_for_inits_random(grain_no_h_random_fp, 37-8, 256, 5, cipher_info, 10);
    //no_lfsr_feedback, no_init_lfsr
    NO_H = 0; NO_LFSR_F = 1; NO_NLFSR_F = 0;
    MAKE_NLFSR_LINEAR = 0; NO_INIT_LFSR_F = 1; NO_INIT_NLFSR_F = 0;
    char grain_no_l_fp[31] = ".\\grain_no_l_max_terms_000.txt";
    printf("grain no_l\n");
    find_max_terms_for_inits(grain_no_l_fp, 32-8, 256, 5, cipher_info);
    char grain_no_l_random_fp[37] = ".\\grain_no_l_guess_max_terms_000.txt";
    printf("grain no_l rand\n");
    find_max_terms_for_inits_random(grain_no_l_random_fp, 37-8, 256, 5, cipher_info, 10);
    //no_nlfsr_feedback, no_init_nlfsr
    NO_H = 0; NO_LFSR_F = 0; NO_NLFSR_F = 1;
    MAKE_NLFSR_LINEAR = 0; NO_INIT_LFSR_F = 0; NO_INIT_NLFSR_F = 1;
    char grain_no_nl_fp[32] = ".\\grain_no_nl_max_terms_000.txt";
    printf("grain no_nl\n");
    find_max_terms_for_inits(grain_no_nl_fp, 33-8, 256, 5, cipher_info);
    char grain_no_nl_random_fp[38] = ".\\grain_no_nl_guess_max_terms_000.txt";
    printf("grain no_nl rand\n");
    find_max_terms_for_inits_random(grain_no_nl_random_fp, 38-8, 256, 5, cipher_info, 10);
    //make nlfsr linear
    NO_H = 0; NO_LFSR_F = 0; NO_NLFSR_F = 0;
    MAKE_NLFSR_LINEAR = 1; NO_INIT_LFSR_F = 0; NO_INIT_NLFSR_F = 0;
    char grain_no_mnl_fp[33] = ".\\grain_no_mnl_max_terms_000.txt";
    printf("grain no_mnl\n");
    find_max_terms_for_inits(grain_no_mnl_fp, 33-8, 256, 5, cipher_info);
    char grain_no_mnl_random_fp[39] = ".\\grain_no_mnl_guess_max_terms_000.txt";
    printf("grain no_mnl rand\n");
    find_max_terms_for_inits_random(grain_no_mnl_random_fp, 38-8, 256, 5, cipher_info, 10);
    //no init
    NO_H = 0; NO_LFSR_F = 0; NO_NLFSR_F = 0;
    MAKE_NLFSR_LINEAR = 0; NO_INIT_LFSR_F = 1; NO_INIT_NLFSR_F = 1;
    char grain_no_ni_fp[32] = ".\\grain_no_ni_max_terms_000.txt";
    printf("grain no_ni\n");
    find_max_terms_for_inits(grain_no_ni_fp, 33-8, 256, 5, cipher_info);
    char grain_no_ni_random_fp[38] = ".\\grain_no_ni_guess_max_terms_000.txt";
    printf("grain no_ni rand\n");
    find_max_terms_for_inits_random(grain_no_ni_random_fp, 38-8, 256, 5, cipher_info, 10);
    //no_h, make nlfsr linear
    NO_H = 0; NO_LFSR_F = 0; NO_NLFSR_F = 0;
    MAKE_NLFSR_LINEAR = 0; NO_INIT_LFSR_F = 1; NO_INIT_NLFSR_F = 1;
    char grain_no_ml_fp[32] = ".\\grain_no_ml_max_terms_000.txt";
    printf("grain no_ml\n");
    find_max_terms_for_inits(grain_no_ml_fp, 32-8, 256, 5, cipher_info);
    char grain_no_ml_random_fp[38] = ".\\grain_no_ml_guess_max_terms_000.txt";
    printf("grain no_ml rand\n");
    find_max_terms_for_inits_random(grain_no_ml_random_fp, 38-8, 256, 5, cipher_info, 10);
    free(cipher_info);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );
    return EXIT_SUCCESS;
};

void find_max_terms_for_inits(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info){
    int max_digit_pos = 2;
    Max_term *max_terms;
    int max_term_limit = 20;
    clock_t time_taken = clock();
    for(int init_clocks=0; init_clocks<max_init_amt;init_clocks++) {
        printf("init clock %d\n", init_clocks);
        cipher_info->init_clocks=init_clocks;
        max_terms = mobius_find_max_terms(max_term_limit, iv_size, cipher_info);
        for(int digit_pos =0; digit_pos <= max_digit_pos; digit_pos++){
            int digit = (init_clocks/(int)(pow(10, digit_pos)))%10;
            char in_ascii = (char)(48+digit);
            int offset_to_last_digit = first_digit_pos + max_digit_pos;
            fp[offset_to_last_digit - digit_pos] = in_ascii;
        }
        print_max_terms(max_terms, fp, cipher_info);
        delete_hash_and_free(&max_terms);
    }
    printf("attack took: %d\n", clock()-time_taken);
}

void find_max_terms_for_inits_random(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info, int guesses){
    int max_digit_pos = 2;
    int max_term_limit = 20;
    Max_term *max_terms;
    clock_t time_taken = clock();
    for(int init_clocks=0; init_clocks<max_init_amt;init_clocks++) {
        printf("init clock %d\n", init_clocks);
        cipher_info->init_clocks=init_clocks;
        max_terms = mobius_find_max_terms_guessing(max_term_limit, iv_size, guesses, cipher_info);
        for(int digit_pos =0; digit_pos <= max_digit_pos; digit_pos++){
            int digit = (init_clocks/(int)(pow(10, digit_pos)))%10;
            char in_ascii = (char)(48+digit);
            int offset_to_last_digit = first_digit_pos + max_digit_pos;
            fp[offset_to_last_digit - digit_pos] = in_ascii;
        }
        print_max_terms(max_terms, fp, cipher_info);
        delete_hash_and_free(&max_terms);
    }
    printf("attack took: %d\n", clock()-time_taken);
}