cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(23_async_compute)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
	"${PPX_DIR}/assets/basic/shaders/StaticTexture.hlsl"
)

CompileShadersVSPS("${VSPS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND CS_HLSL_FILES
	"${PPX_DIR}/assets/basic/shaders/ImageFilter.hlsl"
)

CompileShadersCS("${CS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${CS_HLSL_FILES}
)

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_spv_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxiivk_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_spv_d3dcompile_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_spv_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
