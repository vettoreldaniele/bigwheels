cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(fluid_simulation)

set(SHADER_DIR "${PPX_DIR}/assets/fluid_simulation/shaders")

list(APPEND VS_HLSL_FILES
	"${PPX_DIR}/assets/basic/shaders/PassThroughPos.hlsl"
	"${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
	"${PPX_DIR}/assets/fluid_simulation/shaders/base.hlsl"
	"${PPX_DIR}/assets/fluid_simulation/shaders/blur_vert.hlsl"
)

list(APPEND PS_HLSL_FILES
	"${PPX_DIR}/assets/basic/shaders/PassThroughPos.hlsl"
	"${PPX_DIR}/assets/basic/shaders/FullScreenTriangle.hlsl"
)

list(APPEND CS_HLSL_FILES
	"${SHADER_DIR}/advection.hlsl"
	"${SHADER_DIR}/bloom_blur.hlsl"
	"${SHADER_DIR}/bloom_final.hlsl"
	"${SHADER_DIR}/bloom_prefilter.hlsl"
	"${SHADER_DIR}/blur.hlsl"
	"${SHADER_DIR}/checkerboard.hlsl"
	"${SHADER_DIR}/clear.hlsl"
	"${SHADER_DIR}/color.hlsl"
	"${SHADER_DIR}/copy.hlsl"
	"${SHADER_DIR}/curl.hlsl"
	"${SHADER_DIR}/display.hlsl"
	"${SHADER_DIR}/divergence.hlsl"
	"${SHADER_DIR}/gradient_subtract.hlsl"
	"${SHADER_DIR}/pressure.hlsl"
	"${SHADER_DIR}/splat.hlsl"
	"${SHADER_DIR}/sunrays.hlsl"
	"${SHADER_DIR}/sunrays_mask.hlsl"
	"${SHADER_DIR}/vorticity.hlsl"
)

list(APPEND ADDITIONAL_FILES
	${VS_HLSL_FILES}
	${PS_HLSL_FILES}
	${CS_HLSL_FILES}
	${SHADER_DIR}/config.hlsli
)

CompileShadersVS("${VS_HLSL_FILES}" "${PPX_DIR}/assets/fluid_simulation/shaders" "${PPX_DIR}")
CompileShadersPS("${PS_HLSL_FILES}" "${PPX_DIR}/assets/fluid_simulation/shaders" "${PPX_DIR}")
CompileShadersCS("${CS_HLSL_FILES}" "${PPX_DIR}/assets/fluid_simulation/shaders" "${PPX_DIR}")

add_vk_sample(${PROJECT_NAME}   TRUE "${ADDITIONAL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxvk_spv_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxiivk_dx12_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxvk_spv_d3dcompile_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
