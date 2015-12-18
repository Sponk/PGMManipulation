project(gdi3praktikum2)

add_executable(gdi3praktikum2 src/pgmfilter.c src/pgm.c src/main.c)
add_executable(tests src/pgm.c src/pgmfilter.c  tests/pgmtest.cpp tests/testsuite/tests.cpp)
add_library(pgm STATIC src/pgmfilter.c src/pgm.c)

include_directories(src)

add_subdirectory(exercises)

enable_testing()
add_test(tests tests)