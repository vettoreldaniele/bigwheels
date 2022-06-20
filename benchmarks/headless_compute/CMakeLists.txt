cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(headless_compute)

list(APPEND CS_HLSL_FILES
    "${PPX_DIR}/assets/benchmarks/shaders/ComputeBufferIncrement.hlsl"
)

CompileShadersCS("${CS_HLSL_FILES}" "${PPX_DIR}/assets/benchmarks/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${CS_HLSL_FILES}
)

add_vk_benchmark(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
