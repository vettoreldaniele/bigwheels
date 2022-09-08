cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(12_shadows)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/DiffuseShadow.hlsl"
)

list(APPEND VS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Depth.hlsl"
)

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
)

CompileShaders(
    VS_HLSL_FILES ${VSPS_HLSL_FILES} ${VS_HLSL_FILES}
    PS_HLSL_FILES ${VSPS_HLSL_FILES}
    CS_HLSL_FILES ${CS_HLSL_FILES}
    OUTPUT_DIR "${PPX_DIR}/assets/basic/shaders"
    WORKING_DIR ${PPX_DIR}
)

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${HLSL_FILES}
)
