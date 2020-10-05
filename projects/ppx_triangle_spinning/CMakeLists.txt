cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(ppx_triangle_spinning)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES FOLDER "ppx/projects"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${PPX_DIR}/include
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC ppx glfw        
)