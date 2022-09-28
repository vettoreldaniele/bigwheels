cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(12_shadows)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    SHADER_DEPENDENCIES "shader_diffuse_shadow" "shader_depth")
