cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(fishtornado)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES
    "Buffer.h"
    "Buffer.cpp"
    "Config.h"
    "FishTornado.h"
    "FishTornado.cpp"
    "Flocking.h"
    "Flocking.cpp"
    "Material.h"
    "Ocean.h"
    "Ocean.cpp"
    "ShaderConfig.h"
    "Shark.h"
    "Shark.cpp"
    "main.cpp"
    SHADER_DEPENDENCIES
    "shader_static_vertex_colors"
    "shader_fishtornado"
)
