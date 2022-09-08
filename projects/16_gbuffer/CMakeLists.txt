cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(16_gbuffer)

set(SHADER_DIR "${PPX_DIR}/assets/gbuffer/shaders")

list(APPEND VSPS_HLSL_FILES
    ${SHADER_DIR}/DrawGBufferAttribute.hlsl
    ${SHADER_DIR}/DeferredLight.hlsl
)

list(APPEND VS_HLSL_FILES
    ${SHADER_DIR}/VertexShader.hlsl
)

list(APPEND PS_HLSL_FILES
    ${SHADER_DIR}/DeferredRender.hlsl
)

CompileShaders(
    VS_HLSL_FILES ${VSPS_HLSL_FILES} ${VS_HLSL_FILES}
    PS_HLSL_FILES ${VSPS_HLSL_FILES} ${PS_HLSL_FILES}
    OUTPUT_DIR ${SHADER_DIR}
    WORKING_DIR ${PPX_DIR}
)

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

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${ADDITIONAL_FILES}
)
