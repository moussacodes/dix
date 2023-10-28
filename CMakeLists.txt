cmake_minimum_required(VERSION 3.0)
project(dix C)

set(SOURCES
    src/main.c
    src/editor.c
    src/editor.h
)

add_executable(dix ${SOURCES})
