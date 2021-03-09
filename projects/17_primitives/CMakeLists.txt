cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(17_primitives)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/VertexColors.hlsl"
)

foreach(HLSL_PATH ${HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")