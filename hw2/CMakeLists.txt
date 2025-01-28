# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the main program executable and specify source files
add_executable(my_program
    test.c
)