project(sgl)

set(SDL third_part/SDL2-2.0.12)
set(GLM third_part/glm)

cmake_minimum_required(VERSION 3.17)

add_subdirectory(${SDL})
add_subdirectory(${GLM})

include_directories(
	src 
	${SDL}/include
	${GLM}
	)


add_library(sgl_debug STATIC src/debug.cpp)
add_library(sgl_math STATIC src/maths.cpp)

add_library(sgl STATIC 
	src/FrameBuffer.cpp
	src/SGL.cpp
	)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)


add_definitions("-g")

add_executable(app src/main.cpp)
target_link_libraries(app sgl sgl_debug sgl_math mingw32 SDL2main SDL2-static)


#tests
add_subdirectory(test)
