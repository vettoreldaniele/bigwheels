cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(15_basic_material)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES
    "shader_texture"
    "shader_mtl_vertex_shader"
    "shader_mtl_gouraud"
    "shader_mtl_phong"
    "shader_mtl_blinn_phong"
    "shader_mtl_pbr")
