cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(compute_operations)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders/shaders" "${PPX_DIR}")

list(APPEND CS_HLSL_FILES
    "${PPX_DIR}/assets/benchmarks/shaders/ComputeFilter3.hlsl"
    "${PPX_DIR}/assets/benchmarks/shaders/ComputeFilter5.hlsl"
    "${PPX_DIR}/assets/benchmarks/shaders/ComputeFilter7.hlsl"
)

CompileShadersCS("${CS_HLSL_FILES}" "${PPX_DIR}/assets/benchmarks/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${CS_HLSL_FILES}
)

add_vk_benchmark(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
