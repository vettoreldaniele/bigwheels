cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(texture_transfer_cpu_to_gpu)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
)

CompileShaders(
    VS_HLSL_FILES ${HLSL_FILES}
    PS_HLSL_FILES ${HLSL_FILES}
    OUTPUT_DIR "${PPX_DIR}/assets/benchmarks/shaders"
    WORKING_DIR ${PPX_DIR}
)

add_samples(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${HLSL_FILES}
    TARGET_APIS "vk"
)
