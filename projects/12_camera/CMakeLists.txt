cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(12_camera)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/DiffuseShadow.hlsl"
)

foreach(HLSL_PATH ${VSPS_HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

list(APPEND VS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Depth.hlsl"
)

foreach(HLSL_PATH ${VS_HLSL_FILES})
    CompileShaderVS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
)

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")