cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0054 NEW)

project(BigWheels)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ------------------------------------------------------------------------------
# Options.
# ------------------------------------------------------------------------------
option(PPX_BUILD_PROJECTS "Build sample projets" ON)

# ------------------------------------------------------------------------------
# Detect DXC presence. This is REQUIRED to compile DXIL and SPIR-V shaders.
# ------------------------------------------------------------------------------

message(CHECK_START "Looking for DXC")
if (NOT DXC_PATH)
    message(STATUS "DXC_PATH not set. Looking for Vulkan SDK path in ENV")
    if (DEFINED ENV{VULKAN_SDK})
        if (WIN32)
            set(VK_SDK_DXC "$ENV{VULKAN_SDK}\\Bin\\dxc.exe")
        else ()
            set(VK_SDK_DXC "$ENV{VULKAN_SDK}/bin/dxc")
        endif ()

        if (EXISTS "${VK_SDK_DXC}")
            message(CHECK_PASS "found")
            set(DXC_PATH "${VK_SDK_DXC}")
        else ()
            message(CHECK_FAIL "not found")
        endif ()
    else ()
        message(CHECK_FAIL "not found")
        message(FATAL_ERROR "DXC is required.")
    endif ()
elseif (NOT EXISTS "${DXC_PATH}")
    message(CHECK_FAIL "Cannot open DXC at ${DXC_PATH}")
    message(FATAL_ERROR "DXC is required.")
else ()
    message(CHECK_PASS "found")
endif ()

if (PPX_DXIL_SPV AND NOT DXIL_SPIRV_PATH)
    message(FATAL_ERROR "Could not locate dxil-spirv executable - dxil-spirv is required")
endif()

message(STATUS "DXC_PATH: ${DXC_PATH}")

# ------------------------------------------------------------------------------
# Detect FXC presence. This is required to compile DXCB shaders.
# ------------------------------------------------------------------------------

if (WIN32)
    message(CHECK_START "Looking for FXC")
    # To support compiling shaders to DXBC we need to know the FXC_PATH.
    # In order to find that, we need CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION.
    # However, when a custom TOOLCHAIN is used, that variable is not set. This
    # logic tries to infer CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION in that case.
    if (NOT FXC_PATH)
        if (NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
            set(TARGET_DIR "C:\\Program Files (x86)\\Windows Kits\\10\\bin")
            file(GLOB children RELATIVE ${TARGET_DIR} ${TARGET_DIR}/*)
            foreach(child ${children})
                if (IS_DIRECTORY ${TARGET_DIR}\\${child} AND (${child} MATCHES 10.*))
                    set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION ${child})
                endif()
            endforeach()
        endif()

        if (NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
            message(FATAL_ERROR "Windows SDK not found on this system. Make sure that CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is set.")
        endif()
        message(STATUS "CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is set to: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}.")

        set(FXC_PATH "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}\\x64\\fxc.exe")
    endif()

    if (NOT EXISTS "${FXC_PATH}")
        message(CHECK_FAIL "Cannot open FXC at ${FXC_PATH}")
        message(FATAL_ERROR "FXC is required if PPX_D3D12 or PPX_D3D11 is set.")
    else ()
        message(CHECK_PASS "FXC_PATH: ${FXC_PATH}")
    endif ()
endif ()


# ------------------------------------------------------------------------------
# Configure target platform.
# ------------------------------------------------------------------------------

# Detect Linux.
if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

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
# Configure graphics API(s).
# ------------------------------------------------------------------------------

if (NOT DEFINED PPX_VULKAN)
    set(PPX_VULKAN TRUE)
endif ()

if (NOT DEFINED PPX_D3D11)
    if (WIN32 AND DEFINED FXC_PATH)
        set(PPX_D3D11 TRUE)
    else ()
        set(PPX_D3D11 FALSE)
    endif ()
endif ()

if (NOT DEFINED PPX_D3D12)
    if (WIN32)
        set(PPX_D3D12 TRUE)
    else ()
        set(PPX_D3D12 FALSE)
    endif ()
endif ()

# PPX_DXIL_SPV is never enabled unless specified.
if (NOT DEFINED PPX_DXIL_SPV)
    set(PPX_DXIL_SPV FALSE)
endif ()

message("Graphics API D3D11    : ${PPX_D3D11}")
message("Graphics API D3D12    : ${PPX_D3D12}")
message("Graphics API DXIL SPV : ${PPX_DXIL_SPV}")
message("Graphics API Vulkan   : ${PPX_VULKAN}")

# ------------------------------------------------------------------------------
# Add CMake modules for shader compilation and sample creation.
# Needs to come after platform and API config.
# ------------------------------------------------------------------------------

include(ShaderCompile)
include(GraphicsSample)

# ------------------------------------------------------------------------------
# Configure C++ flags.
# ------------------------------------------------------------------------------

if(MSVC)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /Zc:__cplusplus /std:c++20")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
endif()

set(CMAKE_CXX_STANDARD 20)

# ------------------------------------------------------------------------------
# Configure PPX directories.
# ------------------------------------------------------------------------------

set(PPX_DIR             "${CMAKE_CURRENT_SOURCE_DIR}")
set(PPX_THIRD_PARTY_DIR "${PPX_DIR}/third_party")
set(PPX_TESTS_DIR       "${PPX_DIR}/tests")

# ------------------------------------------------------------------------------
# Configure output directories.
# ------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(PPX_ASSET_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_custom_target(ppx_assets)

# When using a custom output directory, make sure to copy over the assets.
if(NOT (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR))
    add_custom_target(ppx_copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PPX_ASSET_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/assets"
    )
    add_dependencies(ppx_assets ppx_copy_assets)
endif()

# ------------------------------------------------------------------------------
# Configure GLFW.
# ------------------------------------------------------------------------------

# GLFW is required for OpenGL context.
set(GLFW_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    FALSE CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     FALSE CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC  FALSE CACHE BOOL "" FORCE)
add_subdirectory(${PPX_THIRD_PARTY_DIR}/glfw)

# ------------------------------------------------------------------------------
# Add cpu_features.
# ------------------------------------------------------------------------------
function(add_cpu_features)
    set(BUILD_TESTING OFF)
    add_subdirectory(${PPX_THIRD_PARTY_DIR}/cpu_features)
endfunction()

add_cpu_features()

# ------------------------------------------------------------------------------
# Add PPX library.
# ------------------------------------------------------------------------------

add_subdirectory(src)
add_subdirectory(assets)

# ------------------------------------------------------------------------------
# Add projects.
# ------------------------------------------------------------------------------

if (PPX_BUILD_PROJECTS)
    message("Building sample projects")
    add_subdirectory(projects)
endif()


# ------------------------------------------------------------------------------
# Add benchmarks.
# ------------------------------------------------------------------------------

if (PPX_BUILD_BENCHMARKS)
    message("Building benchmarks projects")
    add_subdirectory(benchmarks)
endif()

# ------------------------------------------------------------------------------
# Enable testing support for CMake.
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    enable_testing()
endif()
