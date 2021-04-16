cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(fishtornado)

# ------------------------------------------------------------------------------
# Basic shaders
# ------------------------------------------------------------------------------
list(APPEND BASIC_VSPS_HLSL_FILES
    "${PPX_DIR}/assets/basic/shaders/VertexColors.hlsl"
)

foreach(HLSL_PATH ${BASIC_VSPS_HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${PPX_DIR}/assets/basic/shaders" "${PPX_DIR}")
endforeach()

# ------------------------------------------------------------------------------
# Fish Tornado shaders
# ------------------------------------------------------------------------------
list(APPEND HLSL_INCLUDE_FILES
    "${PPX_DIR}/assets/fishtornado/shaders/Config.hlsli"
    "${PPX_DIR}/assets/fishtornado/shaders/Lighting.hlsli"
)

list(APPEND VSPS_HLSL_FILES
    "${PPX_DIR}/assets/fishtornado/shaders/DebugDraw.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/FlockingRender.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/OceanBeam.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/OceanFloor.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/OceanSurface.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/Shark.hlsl"
)

list(APPEND VS_HLSL_FILES
    "${PPX_DIR}/assets/fishtornado/shaders/FlockingShadow.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/SharkShadow.hlsl"
)
 
list(APPEND CS_HLSL_FILES
    "${PPX_DIR}/assets/fishtornado/shaders/FlockingPosition.hlsl"
    "${PPX_DIR}/assets/fishtornado/shaders/FlockingVelocity.hlsl"
)

foreach(HLSL_PATH ${VSPS_HLSL_FILES})
    CompileShaderVSPS("${HLSL_PATH}" "${PPX_DIR}/assets/fishtornado/shaders" "${PPX_DIR}")
endforeach()

foreach(HLSL_PATH ${VS_HLSL_FILES})
    CompileShaderVS("${HLSL_PATH}" "${PPX_DIR}/assets/fishtornado/shaders" "${PPX_DIR}")
endforeach()

foreach(HLSL_PATH ${CS_HLSL_FILES})
    CompileShaderCS("${HLSL_PATH}" "${PPX_DIR}/assets/fishtornado/shaders" "${PPX_DIR}")
endforeach()

# ------------------------------------------------------------------------------
# C/C++
# ------------------------------------------------------------------------------
list(APPEND CPP_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FishTornado.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FishTornado.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Flocking.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Flocking.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ocean.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Ocean.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ShaderConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Shark.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Shark.cpp
)

list(APPEND ADDITIONAL_FILES
    ${BASIC_VSPS_HLSL_FILES}
    ${HLSL_INCLUDE_FILES}
    ${VSPS_HLSL_FILES}
    ${VS_HLSL_FILES}
    ${CS_HLSL_FILES}
    ${CPP_FILES}
)

add_vk_sample(${PROJECT_NAME}   TRUE "${ADDITIONAL_FILES}")
add_dx11_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dx12_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")
add_dxil_sample(${PROJECT_NAME} TRUE "${ADDITIONAL_FILES}")