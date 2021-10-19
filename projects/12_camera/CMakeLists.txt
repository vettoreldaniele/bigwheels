cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(12_camera)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/DiffuseShadow.hlsl"
)

CompileShadersVSPS("${VSPS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND VS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Depth.hlsl"
)

CompileShadersVS("${VS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
)

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_spv_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
