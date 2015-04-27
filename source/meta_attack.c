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

void find_max_terms_for_inits(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info, int skip_clocks);
void find_max_terms_for_inits_random(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info, int guesses, int skip_clocks);
void plain_v_mobius_time(Cipher_info* cipher_info, int min_dimension, int max_dimension, int clocks, char* fp);

int main(int argc, char **argv) {
    printf("finding max terms\n");
    Cipher_info* cipher_info;
    char dummy_fp[37] = ".\\dummy_exhaustive_max_terms_000.txt";
    cipher_info = dummy_info();
    printf("dummy exhaustive\n");
    find_max_terms_for_inits(dummy_fp, 37-8, 5, 5, cipher_info, 1);
    free(cipher_info);
    char grain_full_fp[37] = ".\\grain_exhaustive_max_terms_000.txt";
    cipher_info = grain_info();
    printf("grain exhaustive\n");
    find_max_terms_for_inits(grain_full_fp, 37-8, 256, 3, cipher_info, 10);
    free(cipher_info);
    char dummy_random_fp[32] = ".\\dummy_guess_max_terms_000.txt";
    cipher_info = dummy_info();
    printf("dummy guess\n");
    find_max_terms_for_inits_random(dummy_random_fp, 32-8, 5, 5, cipher_info, 10, 1);
    free(cipher_info);
    char grain_random_fp[32] = ".\\grain_guess_max_terms_000.txt";
    cipher_info = grain_info();
    printf("grain guess\n");
    find_max_terms_for_inits_random(grain_random_fp, 32-8, 256, 5, cipher_info, 10, 10);
    free(cipher_info);
    cipher_info = grain_info();
     //make nlfsr linear
    MAKE_NLFSR_LINEAR = 1;
    char grain_no_mnl_fp[33] = ".\\grain_no_mnl_max_terms_000.txt";
    printf("grain no_mnl\n");
    find_max_terms_for_inits(grain_no_mnl_fp, 33-8, 256, 3, cipher_info, 10);
    char grain_no_mnl_random_fp[39] = ".\\grain_no_mnl_guess_max_terms_000.txt";
    printf("grain no_mnl rand\n");
    find_max_terms_for_inits_random(grain_no_mnl_random_fp, 38-8, 256, 5, cipher_info, 10, 10);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );
    char dummy_timing[35] = ".\\dummy_results\\plain_v_mobius.txt";
    plain_v_mobius_time(dummy_info(), 1, 5, 5, dummy_timing);
    char grain_timing[35] = ".\\grain_results\\plain_v_mobius.txt";
    plain_v_mobius_time(grain_info(), 1, 2, 10, grain_timing);
    return EXIT_SUCCESS;
};

void plain_v_mobius_time(Cipher_info* cipher_info, int min_dimension, int max_dimension, int clocks, char* fileOutPath){
    Max_term *max_terms;
    int max_term_limit = 20;
    FILE *fp = fopen(fileOutPath, "w+");
    for(int dimension =min_dimension; dimension <=max_dimension; dimension++) {
        printf("dimesnion %d\n", dimension);
        cipher_info->init_clocks= clocks;
        clock_t start = clock();
        max_terms = mobius_find_max_terms(max_term_limit, dimension, cipher_info, 1);
        clock_t mobius_time = clock()-start;
        start = clock();
        max_terms = find_max_terms(max_term_limit, dimension, cipher_info);
        clock_t plain_time = clock()-start;
        fprintf(fp, "init dimensions %d clocks %d in cpu clocks: plain time: %d mobius time: %d\n", dimension, clocks, plain_time, mobius_time);
        delete_hash_and_free(&max_terms);
    }
}

void find_max_terms_for_inits(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info, int skip_clocks){
    int max_digit_pos = 2;
    Max_term *max_terms;
    int max_term_limit = 20;
    clock_t time_taken = clock();
    for(int init_clocks=0; init_clocks<=max_init_amt;init_clocks+=skip_clocks) {
        printf("init clock %d\n", init_clocks);
        cipher_info->init_clocks=init_clocks;
        max_terms = mobius_find_max_terms(max_term_limit, iv_size, cipher_info, iv_size);
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

void find_max_terms_for_inits_random(char* fp, int first_digit_pos, int max_init_amt, int iv_size, Cipher_info* cipher_info, int guesses, int skip_clocks){
    int max_digit_pos = 2;
    int max_term_limit = 20;
    Max_term *max_terms;
    clock_t time_taken = clock();
    for(int init_clocks=0; init_clocks<=max_init_amt;init_clocks+=skip_clocks) {
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