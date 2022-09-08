cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(06_compute_fill)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Texture.hlsl"
)

list(APPEND CS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/ComputeFill.hlsl"
)

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${CS_HLSL_FILES}
)

CompileShaders(
    VS_HLSL_FILES ${VSPS_HLSL_FILES}
    PS_HLSL_FILES ${VSPS_HLSL_FILES}
    CS_HLSL_FILES ${CS_HLSL_FILES}
    OUTPUT_DIR "${PPX_DIR}/assets/basic/shaders"
    WORKING_DIR ${PPX_DIR}
)

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${HLSL_FILES}
)
