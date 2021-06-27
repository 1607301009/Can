# cmake_minimum_required(VERSION <specify CMake version here>)
project(Can C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(I2C_EEPPROM)
include_directories(Num_Show)

add_executable(Can
        I2C_EEPPROM/eeprom.c
        I2C_EEPPROM/I2C.c
        "I2C_EEPPROM/main - 副本 - 副本.c"
        I2C_EEPPROM/main.c
        Num_Show/main.c
        Num_Show/NumShow.c)
