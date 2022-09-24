cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(10_cube_map)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES "shader_cubemap" "shader_skybox")
