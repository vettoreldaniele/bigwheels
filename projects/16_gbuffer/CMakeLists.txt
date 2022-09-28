cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(16_gbuffer)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES
    "main.cpp"
    "Entity.h"
    "Entity.cpp"
    "Material.h"
    "Material.cpp"
    "Render.h"
    SHADER_DEPENDENCIES
    "shader_gbuffer"
    "shader_fullscreen_triangle")
