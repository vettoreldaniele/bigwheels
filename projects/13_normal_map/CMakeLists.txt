cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(13_normal_map)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/NormalMap.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")