cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(BigWheels)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ShaderCompile)

# Detect GGP
if (GGP_TOOLCHAIN_PATH)
    set(GGP TRUE)
endif()

# Detect Linux
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ------------------------------------------------------------------------------
# Configure target platform
# ------------------------------------------------------------------------------

if (NOT (PPX_GPP OR PPX_LINUX OR PPX_MSW))
    message("No PPX target plaform specified - configuring based on current host")
    if (GGP)
        set(PPX_GGP TRUE)
        message("Target platform     : GGP")
    elseif (LINUX)
        set(PPX_LINUX TRUE)
        message("Target platform     : Linux")
    elseif (WIN32)
        set(PPX_MSW TRUE)
        message("Target platform     : Windows")
    endif()
endif()

# ------------------------------------------------------------------------------
# Configure graphics API
# ------------------------------------------------------------------------------

if (NOT (PPX_D3D12 OR PPX_OPENGL OR PPX_VULKAN))
    message("No PPX graphics API(s) specified - configuring based on current host")
    if (GGP)
        set(PPX_VULKAN TRUE)
    elseif (LINUX)
        set(PPX_VULKAN TRUE)
    elseif (WIN32)
        set(PPX_D3D12  TRUE)
        set(PPX_VULKAN TRUE)
    endif()
endif()

set(PPX_D3D12_MSG  FALSE)
set(PPX_VULKAN_MSG FALSE)

# D3D12
if (PPX_D3D12)
    set(PPX_D3D12_MSG  TRUE)
endif()
# Vulkan
if (PPX_VULKAN)
    set(PPX_VULKAN_MSG  TRUE)
endif()

message("Graphics API D3D12  : ${PPX_D3D12_MSG}")
message("Graphics API Vulkan : ${PPX_VULKAN_MSG}")

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
# Add PPX library
# ------------------------------------------------------------------------------

add_subdirectory(src)

# ------------------------------------------------------------------------------
# Add projects
# ------------------------------------------------------------------------------

add_subdirectory(projects)
