cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(15_basic_material)

set(SHADER_DIR "${PPX_DIR}/assets/materials/shaders")

# --------------------------------------------------------------------------------------------------
# Common header and source files
# --------------------------------------------------------------------------------------------------
list(APPEND COMMON_FILES
)

# --------------------------------------------------------------------------------------------------
# Vertex Shader(s)
# --------------------------------------------------------------------------------------------------

list(APPEND VS_HLSL_FILES
    ${SHADER_DIR}/VertexShader.hlsl
)

foreach(HLSL_PATH ${VS_HLSL_FILES})
    CompileShaderVS("${HLSL_PATH}" "${SHADER_DIR}" "${PPX_DIR}")
endforeach()

# --------------------------------------------------------------------------------------------------
# Pixel Shader(s)
# --------------------------------------------------------------------------------------------------

list(APPEND PS_HLSL_FILES
    ${SHADER_DIR}/Gouraud.hlsl
    ${SHADER_DIR}/Phong.hlsl
    ${SHADER_DIR}/BlinnPhong.hlsl
    ${SHADER_DIR}/PBR.hlsl
)

foreach(HLSL_PATH ${PS_HLSL_FILES})
    CompileShaderPS("${HLSL_PATH}" "${SHADER_DIR}" "${PPX_DIR}")
endforeach()

list(APPEND ADDITIONAL_FILES
    ${COMMON_FILES}
    ${VS_HLSL_FILES}
    ${PS_HLSL_FILES}
    ${SHADER_DIR}/Config.hlsli
)

add_vk_sample(${PROJECT_NAME}   TRUE "${ADDITIONAL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")