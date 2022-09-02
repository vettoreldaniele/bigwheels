cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(render_target)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/benchmarks/shaders/PassThroughPos.hlsl"
    "${PPX_DIR}/assets/benchmarks/shaders/MultipleRT.hlsl"
    "${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
)

CompileShadersVSPS("${VSPS_HLSL_FILES}" "${PPX_DIR}/assets/benchmarks/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
)

add_samples(SAMPLE_NAME ${PROJECT_NAME}
			ADDITIONAL_FILES ${HLSL_FILES}
            TARGET_APIS "vk")
