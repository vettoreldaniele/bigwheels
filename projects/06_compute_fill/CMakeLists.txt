cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(06_compute_fill)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
)

foreach(HLSL_PATH ${VSPS_HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

list(APPEND CS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/ComputeFill.hlsl"
)

foreach(HLSL_PATH ${CS_HLSL_FILES})
    CompileShaderCS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${CS_HLSL_FILES}
)

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")