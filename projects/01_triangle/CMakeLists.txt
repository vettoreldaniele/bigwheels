cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(01_triangle)

list(APPEND HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/StaticVertexColors.hlsl"
)

CompileShadersVSPS("${HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

add_samples_for_all_apis(SAMPLE_NAME ${PROJECT_NAME}
						 ADDITIONAL_FILES ${HLSL_FILES})
