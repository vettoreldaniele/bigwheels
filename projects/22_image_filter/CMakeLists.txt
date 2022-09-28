cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(22_image_filter)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES
    "shader_texture"
    "shader_image_filter")
