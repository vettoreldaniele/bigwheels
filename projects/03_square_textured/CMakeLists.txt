cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(03_square_textured)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
)

foreach(HLSL_PATH ${HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")