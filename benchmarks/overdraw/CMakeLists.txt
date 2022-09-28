cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(overdraw)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES
    "shader_benchmarks_texture_sample"
    "shader_benchmarks_texture_sample_explicit_early_z")
