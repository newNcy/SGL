project(test)

add_executable(mat_test mat_test.cpp)
target_link_libraries(mat_test sgl_debug sgl_math)

add_definitions("-g ")
add_executable(geo_test geometry_test.cpp)
add_executable(model_test test_modelloader.cpp)
target_link_libraries(model_test sgl_debug sgl_model)
