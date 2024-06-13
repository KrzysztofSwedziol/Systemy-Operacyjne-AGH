cmake_minimum_required(VERSION 3.27)
project(lab11_sockets1 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab11_sockets1 server.c
        client.c)
