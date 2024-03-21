cmake_minimum_required(VERSION 3.27)
project(SysOpyLab2 C)

set(CMAKE_C_STANDARD 11)

add_executable(SysOpyLab2 client.c
        collatz.c
        client_dynamic.c)
