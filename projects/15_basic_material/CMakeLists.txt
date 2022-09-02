cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(15_basic_material)

set(SHADER_DIR "${PPX_DIR}/assets/materials/shaders")

# --------------------------------------------------------------------------------------------------
# Vertex/Pixel Shader(s)
# --------------------------------------------------------------------------------------------------
list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

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
    ${SHADER_DIR}/Gouraud.hlsl
    ${SHADER_DIR}/Phong.hlsl
    ${SHADER_DIR}/BlinnPhong.hlsl
    ${SHADER_DIR}/PBR.hlsl
)

CompileShadersPS("${PS_HLSL_FILES}" "${SHADER_DIR}" "${PPX_DIR}")

list(APPEND ADDITIONAL_FILES
    ${COMMON_FILES}
    ${HLSL_FILES}
    ${VS_HLSL_FILES}
    ${PS_HLSL_FILES}
    ${SHADER_DIR}/Config.hlsli
)

add_samples_for_all_apis(SAMPLE_NAME ${PROJECT_NAME}
						 ADDITIONAL_FILES ${ADDITIONAL_FILES})
