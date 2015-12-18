
add_executable(p1 p1.c)
add_executable(p2 p2.c)
add_executable(p3 p3.c)
add_executable(p4 p4.c)

target_link_libraries(p1 pgm)
target_link_libraries(p2 pgm)
target_link_libraries(p3 pgm)
target_link_libraries(p4 pgm)