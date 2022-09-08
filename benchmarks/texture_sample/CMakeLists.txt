cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(texture_sample)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/benchmarks/shaders/TextureSample.hlsl"
    "${PPX_DIR}/assets/benchmarks/shaders/TextureSample4Textures.hlsl"
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
    TARGET_APIS "vk" "dx12"
)
