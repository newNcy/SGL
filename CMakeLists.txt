project(sgl)

set(SDL third_part/SDL2-2.0.12)

cmake_minimum_required(VERSION 3.17)
add_subdirectory(${SDL})

include_directories(src ${SDL}/include)
aux_source_directory(src SRC)


if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)


add_definitions("-g")
add_executable(sgl ${SRC})
target_link_libraries(sgl mingw32 SDL2main SDL2-static)

#tests
add_subdirectory(test)
