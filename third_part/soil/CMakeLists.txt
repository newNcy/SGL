

cmake_minimum_required(VERSION 3.8)
include_directories(src)
add_library(SOIL
        src/SOIL.c
        src/image_DXT.c
        src/stb_image_aug.c
        src/image_helper.c
        )
