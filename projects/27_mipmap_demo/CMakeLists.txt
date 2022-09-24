cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(27_mipmap_demo)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES
    "shader_texture_mip")
