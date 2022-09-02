cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(12_shadows)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/DiffuseShadow.hlsl"
)

CompileShadersVSPS("${VSPS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND VS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/Depth.hlsl"
)

CompileShadersVS("${VS_HLSL_FILES}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")

list(APPEND HLSL_FILES
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
)

add_samples_for_all_apis(SAMPLE_NAME ${PROJECT_NAME}
						 ADDITIONAL_FILES ${HLSL_FILES})
