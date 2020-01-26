cmake_minimum_required(VERSION 3.10)
project(benchmark)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

add_library(profiler profiler.c)

add_executable(benchmark benchmark.c)

target_compile_options(
    benchmark
    PRIVATE
    "-O1"
    "-fexperimental-new-pass-manager"
    "-fpass-plugin=${PROFILER_PASS_PLUGIN}"
)

target_link_libraries(benchmark profiler)
