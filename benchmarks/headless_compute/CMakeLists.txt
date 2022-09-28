cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(headless_compute)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES
    "shader_benchmarks_compute_buffer_increment")
