cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(16_gbuffer)

set(SHADER_DIR "${PPX_DIR}/assets/gbuffer/shaders")

# --------------------------------------------------------------------------------------------------
# Vertex/Pixel Shader(s)
# --------------------------------------------------------------------------------------------------

list(APPEND VSPS_HLSL_FILES
    ${SHADER_DIR}/DrawGBufferAttribute.hlsl
    ${SHADER_DIR}/DeferredLight.hlsl
)

CompileShadersVSPS("${VSPS_HLSL_FILES}" "${SHADER_DIR}" "${PPX_DIR}")

# --------------------------------------------------------------------------------------------------
# Vertex Shader(s)
# --------------------------------------------------------------------------------------------------

list(APPEND VS_HLSL_FILES
    ${SHADER_DIR}/VertexShader.hlsl
)

CompileShadersVS("${VS_HLSL_FILES}" "${SHADER_DIR}" "${PPX_DIR}")

# --------------------------------------------------------------------------------------------------
# Pixel Shader(s)
# --------------------------------------------------------------------------------------------------

list(APPEND PS_HLSL_FILES
    ${SHADER_DIR}/DeferredRender.hlsl
)

CompileShadersPS("${PS_HLSL_FILES}" "${SHADER_DIR}" "${PPX_DIR}")


list(APPEND ADDITIONAL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render.h
    ${BASIC_HLSL_FILES}
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
    ${PS_HLSL_FILES}
    ${SHADER_DIR}/Config.hlsli
    ${SHADER_DIR}/GBuffer.hlsli
)

add_vk_sample(${PROJECT_NAME}   TRUE "${ADDITIONAL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxvk_spv_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxvk_spv_d3dcompile_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxil_spv_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
