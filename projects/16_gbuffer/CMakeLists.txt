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

foreach(HLSL_PATH ${VSPS_HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${SHADER_DIR}" "${PPX_DIR}")
endforeach()

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
    ${SHADER_DIR}/DeferredRender.hlsl
)

foreach(HLSL_PATH ${PS_HLSL_FILES})
    CompileShaderPS("${HLSL_PATH}" "${SHADER_DIR}" "${PPX_DIR}")
endforeach()


list(APPEND ADDITIONAL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render.h
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