cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(shadertoy)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
)

CompileShadersVSPS("${VSPS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND CS_HLSL_FILES
    "${PPX_DIR}/assets/shadertoy/shaders/Fractal_Land.hlsl"
    "${PPX_DIR}/assets/shadertoy/shaders/Geodesic_Tiling.hlsl"
    "${PPX_DIR}/assets/shadertoy/shaders/Happy_Jumping.hlsl"
    "${PPX_DIR}/assets/shadertoy/shaders/Protean_Clouds.hlsl"
    "${PPX_DIR}/assets/shadertoy/shaders/Seascape.hlsl"
    "${PPX_DIR}/assets/shadertoy/shaders/Xyptonjtroz.hlsl"
)

CompileShadersCS("${CS_HLSL_FILES}" "${PPX_DIR}/assets/shadertoy/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${CS_HLSL_FILES}
)

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")