cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0054 NEW)

project(BigWheels)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Detect Linux
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(PPX_BUILD_PROJECTS "Build sample projets" ON)

# ------------------------------------------------------------------------------
# Configure target platform
# ------------------------------------------------------------------------------

if (NOT (PPX_GPP OR PPX_LINUX OR PPX_MSW))
    message("No PPX target plaform specified - configuring based on current host")
    if (GGP)
        set(PPX_GGP TRUE)
        message("Target platform       : GGP")
    elseif (LINUX)
        set(PPX_LINUX TRUE)
        message("Target platform       : Linux")
    elseif (WIN32)
        set(PPX_MSW TRUE)
        message("Target platform       : Windows")
    endif()
endif()

# ------------------------------------------------------------------------------
# Configure graphics API
# ------------------------------------------------------------------------------

if ((NOT DEFINED PPX_D3D11) AND (NOT DEFINED PPX_D3D12) AND (NOT DEFINED PPX_DXVK) AND (NOT DEFINED PPX_DXIIVK) AND (NOT DEFINED PPX_DXVK_SPV) AND (NOT DEFINED PPX_DXVK_SPV_D3DCOMPILE) AND (NOT DEFINED PPX_VULKAN))
    message("No PPX graphics API(s) specified - configuring based on current host")
    if (GGP)
        set(PPX_VULKAN TRUE)
    elseif (LINUX)
        set(PPX_VULKAN TRUE)
    elseif (WIN32)
        set(PPX_D3D11  TRUE)
        set(PPX_D3D12  TRUE)
        set(PPX_VULKAN TRUE)
    endif()
endif()

set(PPX_D3D11_MSG    FALSE)
set(PPX_D3D12_MSG    FALSE)
set(PPX_DXVK_MSG     FALSE)
set(PPX_DXIIVK_MSG   FALSE)
set(PPX_DXVK_SPV_MSG FALSE)
set(PPX_VULKAN_MSG   FALSE)

# D3D11
if (PPX_D3D11)
    set(PPX_D3D11_MSG TRUE)
endif()
# D3D12
if (PPX_D3D12)
    set(PPX_D3D12_MSG TRUE)
endif()
# DXVK
if (PPX_DXVK)
    #if(NOT GGP)
    #    message(FATAL_ERROR "PPX_DXVK requires GGP")
    #endif()
    set(PPX_DXVK_MSG TRUE)
endif()
# DXIIVK
if (PPX_DXIIVK)
    set(PPX_DXIIVK_MSG TRUE)
endif()
# DXVK_SPV
if (PPX_DXVK_SPV OR PPX_DXVK_SPV_D3DCOMPILE)
    if(NOT GGP)
        message(FATAL_ERROR "PPX_DXVK_SPV requires GGP")
    endif()
    set(PPX_DXVK_SPV_MSG TRUE)
endif()
# Vulkan
if (PPX_VULKAN)
    set(PPX_VULKAN_MSG TRUE)
endif()

message("Graphics API D3D11    : ${PPX_D3D11_MSG}")
message("Graphics API D3D12    : ${PPX_D3D12_MSG}")
message("Graphics API DXVK     : ${PPX_DXVK_MSG}")
message("Graphics API DXIIVK   : ${PPX_DXIIVK_MSG}")
message("Graphics API DXVK SPV : ${PPX_DXVK_SPV_MSG}")
message("Graphics API Vulkan   : ${PPX_VULKAN_MSG}")

# ------------------------------------------------------------------------------
# Add shader compile functions - needs to come after platform and API config
# ------------------------------------------------------------------------------

set(GLOBAL_COUNT 0 CACHE INTERNAL "")
include(ShaderCompile)

# ------------------------------------------------------------------------------
# Configure C++ flags
# ------------------------------------------------------------------------------

if(MSVC)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /Zc:__cplusplus /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(CMAKE_CXX_STANDARD 17)

# ------------------------------------------------------------------------------
# Configure PPX directories
# ------------------------------------------------------------------------------

set(PPX_DIR             ${CMAKE_CURRENT_SOURCE_DIR})
set(PPX_THIRD_PARTY_DIR ${PPX_DIR}/third_party)
set(PPX_TESTS_DIR       ${PPX_DIR}/tests)

# ------------------------------------------------------------------------------
# Configure output directories
# ------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# ------------------------------------------------------------------------------
# Configure GLFW
# ------------------------------------------------------------------------------

# GLFW is required for OpenGL context
set(GLFW_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    FALSE CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     FALSE CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC  FALSE CACHE BOOL "" FORCE)
add_subdirectory(${PPX_THIRD_PARTY_DIR}/glfw)

# ------------------------------------------------------------------------------
# Add cpu_features
# ------------------------------------------------------------------------------

add_subdirectory(${PPX_THIRD_PARTY_DIR}/cpu_features)

# ------------------------------------------------------------------------------
# Add PPX library
# ------------------------------------------------------------------------------

if (PPX_DXVK OR PPX_DXIIVK OR PPX_DXVK_SPV OR PPX_DXVK_SPV_D3DCOMPILE)
    if (PPX_DXVK)
        set(ENABLE_DXVK   ON CACHE BOOL "" FORCE)
        set(ENABLE_DXIIVK OFF CACHE BOOL "" FORCE)
    elseif(PPX_DXIIVK)
        set(ENABLE_DXVK   OFF CACHE BOOL "" FORCE)
        set(ENABLE_DXIIVK ON CACHE BOOL "" FORCE)
    endif()
    set(LIBPORTO_DISABLE_SAMPLES ON)
    add_subdirectory(${PPX_THIRD_PARTY_DIR}/libporto)
    target_compile_definitions(
        porto
        PUBLIC  LIBPORTO_EXPERIMENTAL_ENABLE_SPIRV_SHADERS
    )
endif()

add_subdirectory(src)

# ------------------------------------------------------------------------------
# Add projects
# ------------------------------------------------------------------------------

if (PPX_BUILD_PROJECTS)
    message("Building sample projects")
    add_subdirectory(projects)
endif()


# ------------------------------------------------------------------------------
# Add benchmarks
# ------------------------------------------------------------------------------

if (PPX_BUILD_BENCHMARKS)
    message("Building benchmarks projects")
    add_subdirectory(benchmarks)
endif()

# ------------------------------------------------------------------------------
# Enable testing support for CMake
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    enable_testing()
endif()
