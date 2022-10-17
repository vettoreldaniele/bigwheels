cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(11_compressed_texture)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES "shader_texture")
