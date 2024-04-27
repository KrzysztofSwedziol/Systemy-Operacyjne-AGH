cmake_minimum_required(VERSION 3.27)
project(lab7_IPC_communication C)

set(CMAKE_C_STANDARD 11)

add_executable(lab7_IPC_communication server.c
        client.c
        client2.c
        client3.c)
