cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(render_target)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES
    "shader_benchmarks_passthrough_pos"
    "shader_benchmarks_multiple_rt"
    "shader_fullscreen_triangle")
