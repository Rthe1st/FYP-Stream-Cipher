#include <inttypes.h>

#include "../cube_attack/cube_attack.h"

#ifndef CUBE_ATTACK_TESTS_H
#define CUBE_ATTACK_TESTS_H

int check_max_terms(int clocks, Max_term * correct_max_terms, Max_term * actual_max_terms, int expected_amount, int number_being_tested, int iv_size);
int run_cube_attack_unit_tests();

#endif