cmake_minimum_required(VERSION 3.10)
project(annotations)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

add_executable(annotations annotations.c)

target_compile_options(
    annotations
    PRIVATE
    "-O1"
    "-fexperimental-new-pass-manager"
    "-fpass-plugin=${BINARY_EMBEDDER_PASS_PLUGIN}"
)

