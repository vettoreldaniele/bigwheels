cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(fluid_simulation)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES
    "shader_passthrough_pos"
    "shader_fullscreen_triangle"
    "shader_fluid_simulation")
