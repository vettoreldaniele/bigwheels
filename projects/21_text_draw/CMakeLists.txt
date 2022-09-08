cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(21_text_draw)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/TextDraw.hlsl"
)

CompileShaders(
    VS_HLSL_FILES ${HLSL_FILES}
    PS_HLSL_FILES ${HLSL_FILES}
    OUTPUT_DIR "${PPX_DIR}/assets/basic/shaders"
    WORKING_DIR ${PPX_DIR}
)

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${HLSL_FILES}
)
