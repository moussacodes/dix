cmake_minimum_required(VERSION 3.0)
project(dix C)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})


set(SOURCES
    src/main.c
    src/editor.c
    src/editor.h
    src/file.h
    src/file.c
    src/highlight.h
    src/highlight.c
)

add_executable(dix ${SOURCES})
target_link_libraries(dix ${CURSES_LIBRARIES})
