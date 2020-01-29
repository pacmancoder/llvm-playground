project(HelloPassSample)

add_custom_target(
    ${PROJECT_NAME}
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/run.sh" "${CMAKE_CURRENT_BINARY_DIR}" "${PASS_PLUGIN_HELLO_PASS}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    SOURCES main.cpp
)

add_dependencies(${PROJECT_NAME} 00_hello_world)

# More convinient plugin invocation

add_executable(${PROJECT_NAME}-clang main.cpp)
add_dependencies(${PROJECT_NAME}-clang 01_clang_plugin)

target_compile_options(
    ${PROJECT_NAME}-clang
    PRIVATE
    "-O1"
    "-fexperimental-new-pass-manager"
    "-fpass-plugin=${PASS_PLUGIN_HELLO_CLANG_PASS}"
)