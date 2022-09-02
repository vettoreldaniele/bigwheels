cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(draw_call)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/benchmarks/shaders/PassThroughPos.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/benchmarks/shaders" "${PPX_DIR}")

add_samples(SAMPLE_NAME ${PROJECT_NAME}
			ADDITIONAL_FILES ${HLSL_FILES}
            TARGET_APIS "vk")
