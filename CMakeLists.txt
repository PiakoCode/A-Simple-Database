cmake_minimum_required(VERSION 3.26)
project(build_my_own_sqlite C)

set(CMAKE_C_STANDARD 11)

add_executable(build_my_own_sqlite main.c
        inputBuffer.h
        inputBuffer.c row.h)
