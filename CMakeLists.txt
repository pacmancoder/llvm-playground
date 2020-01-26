cmake_minimum_required(VERSION 3.14)
project(llvm-playground)

set(CMAKE_CXX_STANDARD 14)

# Common LLVM definitions
find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

# Passes
add_subdirectory(00_hello_world)
add_subdirectory(01_clang_plugin)
add_subdirectory(02_code_modification)
add_subdirectory(03_code_generation)