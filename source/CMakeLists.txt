cmake_minimum_required(VERSION 2.8.4)
project(FYP_Stream_Cipher)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11 -m32 -ggdb")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/Users/User/Documents/GitHub/FYP-Stream-Cipher/bin")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cube_attack)
add_subdirectory(ciphers)
add_subdirectory(cipher_io)
add_subdirectory(tests)

add_executable(FYP_Stream_Cipher main.c)
target_link_libraries(FYP_Stream_Cipher ciphers)
target_link_libraries(FYP_Stream_Cipher cipher_io)

add_executable(FYP_Stream_Cipher_Cube_Attack attack_main.c)
target_link_libraries(FYP_Stream_Cipher_Cube_Attack cube_attack)
target_link_libraries(FYP_Stream_Cipher_Cube_Attack ciphers)
target_link_libraries(FYP_Stream_Cipher_Cube_Attack cipher_io)
