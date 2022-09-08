cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(23_async_compute)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
	"${PPX_DIR}/assets/basic/shaders/StaticTexture.hlsl"
)

list(APPEND CS_HLSL_FILES
	"${PPX_DIR}/assets/basic/shaders/ImageFilter.hlsl"
)

CompileShaders(
    VS_HLSL_FILES ${VSPS_HLSL_FILES}
    PS_HLSL_FILES ${VSPS_HLSL_FILES}
    CS_HLSL_FILES ${CS_HLSL_FILES}
    OUTPUT_DIR "${PPX_DIR}/assets/basic/shaders"
    WORKING_DIR ${PPX_DIR}
)

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${CS_HLSL_FILES}
)

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${HLSL_FILES}
)
