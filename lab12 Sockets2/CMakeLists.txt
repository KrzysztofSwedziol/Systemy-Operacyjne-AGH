cmake_minimum_required(VERSION 3.27)
project(lab12_Sockets2 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab12_Sockets2 main.c
        client.c
        Server.c)
