cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(21_text_draw)

add_samples_for_all_apis(
    NAME ${PROJECT_NAME}
    SOURCES "main.cpp"
    DEPENDENCIES
    "shader_text_draw")
