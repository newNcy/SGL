project(test)

add_executable(mat_test mat_test.cpp)
target_link_libraries(mat_test sgl_debug sgl_math)
