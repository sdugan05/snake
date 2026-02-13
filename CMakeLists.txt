cmake_minimum_required(VERSION 3.16)
project(snake C)

find_package(raylib 3.0 REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(snake
    src/main.c
    src/snake.c
    src/util.c
)

target_include_directories(snake PRIVATE src)

target_link_libraries(snake PRIVATE raylib)
target_link_libraries(snake PRIVATE "-framework IOKit")
target_link_libraries(snake PRIVATE "-framework Cocoa")
target_link_libraries(snake PRIVATE "-framework OpenGL")

target_compile_options(snake PRIVATE -O0 -g3)
target_compile_options(snake PRIVATE -fsanitize=address)
target_link_options(snake PRIVATE -fsanitize=address)
