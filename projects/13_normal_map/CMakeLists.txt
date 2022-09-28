cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(13_normal_map)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES "shader_normal_map")
