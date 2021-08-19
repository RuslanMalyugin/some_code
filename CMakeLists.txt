cmake_minimum_required(VERSION 2.8)
project(assembler_testing)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")
find_package(PythonLibs 3.6 REQUIRED)
python_add_module(matrix matrix.c)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(matrix ${PYTHON_LIBRARIES})
