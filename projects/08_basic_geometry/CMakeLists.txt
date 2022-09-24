cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(08_basic_geometry)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES "shader_vertex_colors")
