cmake_minimum_required(VERSION 3.10)
project(benchmark)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(CMAKE_CXX_STANDARD 17)

add_library(profiler profiler.cpp)

add_compile_options(
    "-O1"
    "-fexperimental-new-pass-manager"
    "-fpass-plugin=${PROFILER_PASS_PLUGIN}"
)

add_library(sqlite3 sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC sqlite)
target_link_libraries(sqlite3 dl pthread)

add_executable(benchmark benchmark.c)

target_link_libraries(benchmark profiler sqlite3)
