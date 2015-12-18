cmake_minimum_required(VERSION 2.4)
project(Praktikum2)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

if(NOT MSVC)
   set(CMAKE_C_FLAGS "-std=c11")
   set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lm")
endif()

add_executable(gdi3praktikum2 src/pgmfilter.c src/pgm.c src/main.c)
add_executable(tests src/pgm.c src/pgmfilter.c  tests/pgmtest.cpp tests/testsuite/tests.cpp)
add_library(pgm STATIC src/pgmfilter.c src/pgm.c)

include_directories(src)

add_subdirectory(exercises)

enable_testing()
add_test(tests tests)