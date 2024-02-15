cmake_minimum_required(VERSION 3.13) 
project(aufgabe1)
set(CMAKE_C_STANDARD 23)

set(SOURCE_FILES
    src/main.c
    src/arukone.h
    src/arukone.c
)

add_executable(aufgabe1 ${SOURCE_FILES})