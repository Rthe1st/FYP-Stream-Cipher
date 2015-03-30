add_executable(tests cipher_unit_tests.c cube_attack_unit_tests.c mobius_unit_tests.c minunit.c run_all_tests.c)
target_link_libraries(tests cube_attack)
target_link_libraries(tests ciphers)
target_link_libraries(tests cipher_io)
