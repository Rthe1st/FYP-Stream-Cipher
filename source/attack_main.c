#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ciphers/dummy_cipher.h>

#include "attack_main.h"
#include "./cube_attack/cube_attack.h"
#include "./cube_attack/mobius_cube_attack.h"
#include "./cipher_io/useful.h"
#include "./ciphers/grain.h"
#include "./ciphers/cipher_helpers.h"

int main(int argc, char **argv) {
    srand((unsigned int) time(NULL));
    printf("finding max terms\n");
    Max_term *max_terms;
    Cipher_info * cipher_info = grain_info();//dummy_info();//
    //cipher_info->init_clocks=0;
    cipher_info->init_clocks = 5;
    clock_t time_taken = clock();
    if(argc >= 2 && strcmp(argv[1], "-mobius") == 0) {
        max_terms = mobius_find_max_terms(50, 2, cipher_info);
    }else{
        if(argc >= 2 && strcmp(argv[1], "-guess") == 0) {
            max_terms = find_max_terms_guessing(50, 10, 1, cipher_info);
        }else{
            max_terms = find_max_terms(50, 2, cipher_info);
        }
    }
    char *file_out_path = ".\\max_terms.txt";
    print_max_terms(max_terms, file_out_path, cipher_info);
    printf("Attack took: %d\n", clock()-time_taken);
    free(cipher_info);
    delete_hash_and_free(&max_terms);
    return EXIT_SUCCESS;
};