project(03_code_generation)

add_library(${PROJECT_NAME} MODULE PassPlugin.cpp Profiler.cpp)
set(PASS_PLUGIN_PROFILER $<TARGET_FILE:${PROJECT_NAME}> PARENT_SCOPE)