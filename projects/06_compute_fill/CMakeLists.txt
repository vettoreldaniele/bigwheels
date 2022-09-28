cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(06_compute_fill)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES "shader_compute_fill" "shader_texture")
