cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(texture_load)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES
    "shader_benchmarks_texture_load"
    "shader_benchmarks_texture_load_4_textures")
