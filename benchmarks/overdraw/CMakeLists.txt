cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(overdraw)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/benchmarks/shaders/TextureSample.hlsl"
    "${PPX_DIR}/assets/benchmarks/shaders/TextureSample_ExplicitEarlyZ.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/benchmarks/shaders" "${PPX_DIR}")

add_vk_benchmark(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
