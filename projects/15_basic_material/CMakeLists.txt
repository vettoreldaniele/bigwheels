cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(15_basic_material)

set(SHADER_DIR "${PPX_DIR}/assets/materials/shaders")

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
)

list(APPEND VS_HLSL_FILES
    ${SHADER_DIR}/VertexShader.hlsl
)

list(APPEND PS_HLSL_FILES
    ${SHADER_DIR}/Gouraud.hlsl
    ${SHADER_DIR}/Phong.hlsl
    ${SHADER_DIR}/BlinnPhong.hlsl
    ${SHADER_DIR}/PBR.hlsl
)

list(APPEND ADDITIONAL_FILES
    ${COMMON_FILES}
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
    ${PS_HLSL_FILES}
    ${SHADER_DIR}/Config.hlsli
)

CompileShaders(
    VS_HLSL_FILES ${VSPS_HLSL_FILES} ${VS_HLSL_FILES}
    PS_HLSL_FILES ${VSPS_HLSL_FILES} ${PS_HLSL_FILES}
    OUTPUT_DIR ${SHADER_DIR}
    WORKING_DIR ${PPX_DIR}
)

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${ADDITIONAL_FILES}
)
