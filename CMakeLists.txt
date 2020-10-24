project(sgl)

set(SDL third_part/SDL2-2.0.12)

add_subdirectory(${SDL})

include_directories(${SDL}/include)
aux_source_directory(src SRC)


add_Executable(sgl ${SRC})
target_link_libraries(sgl mingw32 SDL2main SDL2-static)

