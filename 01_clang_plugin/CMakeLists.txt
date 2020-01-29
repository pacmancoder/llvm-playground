project(01_clang_plugin)

add_library(${PROJECT_NAME} MODULE ClangPlugin.cpp)
set(PASS_PLUGIN_HELLO_CLANG_PASS $<TARGET_FILE:${PROJECT_NAME}> PARENT_SCOPE)