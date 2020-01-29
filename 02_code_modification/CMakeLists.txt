project(02_code_modification)


add_library(${PROJECT_NAME} MODULE BinaryEmbedder.cpp PassPlugin.cpp)
set(PASS_PLUGIN_BINARY_EMBEDDER $<TARGET_FILE:${PROJECT_NAME}> PARENT_SCOPE)