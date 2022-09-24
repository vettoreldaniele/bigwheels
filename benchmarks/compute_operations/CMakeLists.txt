cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(compute_operations)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES
    "shader_fullscreen_triangle"
    "shader_benchmarks_compute_filter_3"
    "shader_benchmarks_compute_filter_5"
    "shader_benchmarks_compute_filter_7")
