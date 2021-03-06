project(sgl)

set(SDL third_part/SDL2-2.0.12)
set(STB third_part/stb)


#set(CMAKE_BUILD_TYPE Profile)
#set(CMAKE_CXX_FLAGS_PROFILE "-pg -g")

cmake_minimum_required(VERSION 3.17)

add_subdirectory(${SDL})
add_subdirectory(third_part/assimp-3.3.1)

include_directories(
	src 
	${SDL}/include
	${STB}
    third_part/assimp-3.3.1/include
	)


add_library(sgl_debug STATIC src/debug.cpp)
add_library(sgl_model STATIC src/ModelLoader.cpp src/Model.cpp)
add_library(sgl_math STATIC src/maths.cpp)

add_library(sgl STATIC 
	src/Shader.cpp
	src/FrameBuffer.cpp
	src/Pipeline.cpp
	src/SGL.cpp
	)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)


add_definitions("-g -O3 -flto -ffast-math")

add_executable(app src/main.cpp)
target_link_libraries(app sgl sgl_debug sgl_math mingw32 SDL2main SDL2-static sgl_model assimp)


#tests
add_subdirectory(test)
