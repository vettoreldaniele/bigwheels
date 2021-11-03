cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(14_input)

add_vk_sample(${PROJECT_NAME}   TRUE "${HLSL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxvk_spv_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
add_dxiivk_dx12_sample(${PROJECT_NAME} TRUE "${HLSL_FILES}")
