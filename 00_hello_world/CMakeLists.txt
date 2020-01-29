project(00_hello_world)

add_library(${PROJECT_NAME} MODULE HelloPass.cpp)
set(PASS_PLUGIN_HELLO_PASS $<TARGET_FILE:${PROJECT_NAME}> PARENT_SCOPE)