cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(texture_transfer_cpu_to_gpu)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders/shaders" "${PPX_DIR}")

add_samples(SAMPLE_NAME ${PROJECT_NAME}
			ADDITIONAL_FILES ${HLSL_FILES}
            TARGET_APIS "vk")
