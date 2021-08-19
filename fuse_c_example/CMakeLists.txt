cmake_minimum_required(VERSION 3.15)

project(fuse-example)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -g")

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)

include_directories(${FUSE_INCLUDE_DIRS})
add_executable(fuse-example main.c)
target_link_libraries(fuse-example ${FUSE_LIBRARIES})