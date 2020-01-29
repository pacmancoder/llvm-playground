cmake_minimum_required(VERSION 3.10)
project(benchmark)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(CMAKE_CXX_STANDARD 17)

add_library(profiler profiler.cpp)

set(
    PROJ_COMPILE_OPTIONS
    "-O1"
    "-fexperimental-new-pass-manager"
    "-fpass-plugin=${PASS_PLUGIN_PROFILER}"
)



add_library(sqlite3 sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC sqlite)
target_link_libraries(sqlite3 dl pthread)
target_compile_options(sqlite3 PRIVATE ${PROJ_COMPILE_OPTIONS})

add_executable(${PROJECT_NAME} benchmark.c)
target_link_libraries(${PROJECT_NAME} profiler sqlite3)
add_dependencies(${PROJECT_NAME} 03_code_generation)
target_compile_options(${PROJECT_NAME} PRIVATE ${PROJ_COMPILE_OPTIONS})