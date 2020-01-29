cmake_minimum_required(VERSION 3.10)
project(annotations)

add_executable(${PROJECT_NAME} annotations.c)

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    "-O1"
    "-fexperimental-new-pass-manager"
    "-fpass-plugin=${PASS_PLUGIN_BINARY_EMBEDDER}"
)

add_dependencies(${PROJECT_NAME} 02_code_modification)