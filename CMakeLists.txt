project(gdi3praktikum2)

add_executable(gdi3praktikum2 src/pgm.c src/main.c)
add_executable(tests src/pgm.c tests/pgmtest.cpp tests/testsuite/tests.cpp)

include_directories(src)