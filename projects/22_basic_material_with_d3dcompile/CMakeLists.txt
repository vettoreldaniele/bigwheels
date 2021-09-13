cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(22_basic_material_with_d3dcompile)

if (PPX_DXVK OR PPX_DXVK_SPV)
    set(SHADER_DIR "${PPX_DIR}/assets/materials/shaders")

    # --------------------------------------------------------------------------------------------------
    # Vertex/Pixel Shader(s)
    # --------------------------------------------------------------------------------------------------
    list(APPEND HLSL_FILES
        "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
    )

    # --------------------------------------------------------------------------------------------------
    # Vertex Shader(s)
    # --------------------------------------------------------------------------------------------------

    list(APPEND VS_HLSL_FILES
        ${SHADER_DIR}/VertexShader.hlsl
    )

    # --------------------------------------------------------------------------------------------------
    # Pixel Shader(s)
    # --------------------------------------------------------------------------------------------------

    list(APPEND PS_HLSL_FILES
        ${SHADER_DIR}/Gouraud.hlsl
        ${SHADER_DIR}/Phong.hlsl
        ${SHADER_DIR}/BlinnPhong.hlsl
        ${SHADER_DIR}/PBR.hlsl
    )

    list(APPEND ADDITIONAL_FILES
        ${COMMON_FILES}
        ${HLSL_FILES}
        ${VS_HLSL_FILES}
        ${PS_HLSL_FILES}
        ${SHADER_DIR}/Config.hlsli
    )

    set(ENABLE_PORTO_D3DCOMPILER TRUE)
    add_dxvk_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
    add_dxvk_spv_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
endif (PPX_DXVK OR PPX_DXVK_SPV)
