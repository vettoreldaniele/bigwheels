cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(ppx)

# ------------------------------------------------------------------------------
# Vulkan header and libraries
# ------------------------------------------------------------------------------

# Detect Vulkan directories
if (PPX_VULKAN)
    # Determine Vulkan's include directory
    if (NOT VULKAN_INCLUDE_DIR)
        if (LINUX)
            if (GGP)
                set(VULKAN_INCLUDE_DIR "${GGP_SYSROOT_PATH}/usr/local/include")
            else()
                if (DEFINED ENV{VULKAN_SDK})
                    set(VULKAN_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
                endif()
            endif()
        elseif (APPLE OR WIN32)
            if (DEFINED ENV{VULKAN_SDK})
                set(VULKAN_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")
            endif()
        endif()
    endif()

    # Determine Vulkan's library directory
    if (NOT VULKAN_LIBRARY_DIR)
        if (LINUX)
            if (GGP)
                set(VULKAN_LIBRARY_DIR "${GGP_SYSROOT_PATH}/usr/local/lib")
            else()
                if (DEFINED ENV{VULKAN_SDK})
                    set(VULKAN_LIBRARY_DIR "$ENV{VULKAN_SDK}/lib")
                endif()
            endif()
        elseif (APPLE OR WIN32)
            if (DEFINED ENV{VULKAN_SDK})
                set(VULKAN_LIBRARY_DIR "$ENV{VULKAN_SDK}/Lib")
            endif()
        endif()
    endif()

    # Bail if Vulkan's include directory is not set
    if (NOT VULKAN_INCLUDE_DIR)
        message(FATAL_ERROR "VULKAN_INCLUDE_DIR not specified and could not be determined using environment variable VULKAN_SDK")
    endif()

    # Bail if Vulkan's library directory is not set
    if (NOT VULKAN_LIBRARY_DIR)
        message(FATAL_ERROR "VULKAN_LIBRARY_DIR not specified and could not be determined using environment variable VULKAN_SDK")
    endif()

    message("Found Vulkan: ${VULKAN_DIR}")
    message("  Vulkan include directory: ${VULKAN_INCLUDE_DIR}")
    message("  Vulkan library directory: ${VULKAN_LIBRARY_DIR}")
endif()

# ------------------------------------------------------------------------------
# Vulkan Linux surface flags
# ------------------------------------------------------------------------------
#
# Defaults to XCB if nothing is specified
#
if (PPX_LINUX_XCB)
    set(PPX_LINUX_VULKAN_SURFACE "PPX_LINUX_XCB")
elseif(PPX_LINUX_XLIB)
    set(PPX_LINUX_VULKAN_SURFACE "PPX_LINUX_XLIB")
elseif(PPX_LINUX_WAYLAND)
    set(PPX_LINUX_VULKAN_SURFACE "PPX_LINUX_WAYLAND")
else()
    set(PPX_LINUX_VULKAN_SURFACE "PPX_LINUX_XCB")
endif()

# ------------------------------------------------------------------------------
# Add header and source files
# ------------------------------------------------------------------------------

set(INC_DIR    ${PPX_DIR}/include)
set(SRC_DIR    ${PPX_DIR}/src)
set(IMGUI_DIR  ${PPX_DIR}/third_party/imgui)

list(APPEND IMGUI_HEADER_FILES
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/examples/imgui_impl_glfw.h
)

list(APPEND IMGUI_SOURCE_FILES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/examples/imgui_impl_vulkan.cpp
)

list(APPEND XXHASH_FILES
    ${PPX_DIR}/third_party/xxHash/xxhash.h
    ${PPX_DIR}/third_party/xxHash/xxhash.c
)

if (PPX_D3D12)
    list(APPEND IMGUI_SOURCE_FILES
        ${IMGUI_DIR}/examples/imgui_impl_dx12.cpp
    )
endif()

list(
    APPEND PPX_HEADER_FILES
    ${INC_DIR}/ppx/000_config.h
    ${INC_DIR}/ppx/000_math_config.h
    ${INC_DIR}/ppx/application.h
    ${INC_DIR}/ppx/base_application.h
    ${INC_DIR}/ppx/bitmap.h
    ${INC_DIR}/ppx/camera.h
    ${INC_DIR}/ppx/fs.h
    ${INC_DIR}/ppx/geometry.h
    ${INC_DIR}/ppx/graphics_util.h
    ${INC_DIR}/ppx/headless_application.h
    ${INC_DIR}/ppx/imgui_impl.h
    ${INC_DIR}/ppx/log.h
    ${INC_DIR}/ppx/math_util.h
    ${INC_DIR}/ppx/mesh.h
    ${INC_DIR}/ppx/mipmap.h
    ${INC_DIR}/ppx/obj_ptr.h
    ${INC_DIR}/ppx/platform.h
    ${INC_DIR}/ppx/ppx.h
    ${INC_DIR}/ppx/profiler.h
    ${INC_DIR}/ppx/string_util.h
    ${INC_DIR}/ppx/timer.h
    ${INC_DIR}/ppx/transform.h
    ${INC_DIR}/ppx/util.h
)

list(
    APPEND PPX_SOURCE_FILES
    ${SRC_DIR}/ppx/application.cpp
    ${SRC_DIR}/ppx/base_application.cpp
    ${SRC_DIR}/ppx/bitmap.cpp
    ${SRC_DIR}/ppx/camera.cpp
    ${SRC_DIR}/ppx/geometry.cpp
    ${SRC_DIR}/ppx/graphics_util.cpp
    ${SRC_DIR}/ppx/headless_application.cpp
    ${SRC_DIR}/ppx/imgui_impl.cpp
    ${SRC_DIR}/ppx/log.cpp
    ${SRC_DIR}/ppx/math_util.cpp
    ${SRC_DIR}/ppx/mesh.cpp
    ${SRC_DIR}/ppx/mesh.cpp
    ${SRC_DIR}/ppx/mipmap.cpp
    ${SRC_DIR}/ppx/platform.cpp
    ${SRC_DIR}/ppx/profiler.cpp
    ${SRC_DIR}/ppx/string_util.cpp
    ${SRC_DIR}/ppx/timer.cpp
    ${SRC_DIR}/ppx/transform.cpp
    ${SRC_DIR}/ppx/imgui/font_inconsolata.h
    ${SRC_DIR}/ppx/imgui/font_inconsolata.cpp
)

list(
    APPEND PPX_GRFX_HEADER_FILES
    ${INC_DIR}/ppx/grfx/000_grfx_config.h
    ${INC_DIR}/ppx/grfx/grfx_buffer.h
    ${INC_DIR}/ppx/grfx/grfx_command.h
    ${INC_DIR}/ppx/grfx/grfx_constants.h
    ${INC_DIR}/ppx/grfx/grfx_descriptor.h
    ${INC_DIR}/ppx/grfx/grfx_device.h
    ${INC_DIR}/ppx/grfx/grfx_draw_pass.h
    ${INC_DIR}/ppx/grfx/grfx_enums.h
    ${INC_DIR}/ppx/grfx/grfx_format.h
    ${INC_DIR}/ppx/grfx/grfx_fullscreen_quad.h
    ${INC_DIR}/ppx/grfx/grfx_gpu.h
    ${INC_DIR}/ppx/grfx/grfx_helper.h
    ${INC_DIR}/ppx/grfx/grfx_image.h
    ${INC_DIR}/ppx/grfx/grfx_instance.h
    ${INC_DIR}/ppx/grfx/grfx_model.h
    ${INC_DIR}/ppx/grfx/grfx_pipeline.h
    ${INC_DIR}/ppx/grfx/grfx_queue.h
    ${INC_DIR}/ppx/grfx/grfx_render_pass.h
    ${INC_DIR}/ppx/grfx/grfx_scope.h
    ${INC_DIR}/ppx/grfx/grfx_shader.h
    ${INC_DIR}/ppx/grfx/grfx_swapchain.h
    ${INC_DIR}/ppx/grfx/grfx_sync.h
    ${INC_DIR}/ppx/grfx/grfx_texture.h
    ${INC_DIR}/ppx/grfx/grfx_util.h
)

list(
    APPEND PPX_GRFX_SOURCE_FILES
    ${SRC_DIR}/ppx/grfx/grfx_buffer.cpp
    ${SRC_DIR}/ppx/grfx/grfx_command.cpp
    ${SRC_DIR}/ppx/grfx/grfx_descriptor.cpp
    ${SRC_DIR}/ppx/grfx/grfx_device.cpp
    ${SRC_DIR}/ppx/grfx/grfx_draw_pass.cpp
    ${SRC_DIR}/ppx/grfx/grfx_format.cpp
    ${SRC_DIR}/ppx/grfx/grfx_fullscreen_quad.cpp
    ${SRC_DIR}/ppx/grfx/grfx_gpu.cpp
    ${SRC_DIR}/ppx/grfx/grfx_helper.cpp
    ${SRC_DIR}/ppx/grfx/grfx_image.cpp
    ${SRC_DIR}/ppx/grfx/grfx_instance.cpp
    ${SRC_DIR}/ppx/grfx/grfx_model.cpp
    ${SRC_DIR}/ppx/grfx/grfx_pipeline.cpp
    ${SRC_DIR}/ppx/grfx/grfx_queue.cpp
    ${SRC_DIR}/ppx/grfx/grfx_render_pass.cpp
    ${SRC_DIR}/ppx/grfx/grfx_scope.cpp
    ${SRC_DIR}/ppx/grfx/grfx_shader.cpp
    ${SRC_DIR}/ppx/grfx/grfx_swapchain.cpp
    ${SRC_DIR}/ppx/grfx/grfx_sync.cpp
    ${SRC_DIR}/ppx/grfx/grfx_texture.cpp
    ${SRC_DIR}/ppx/grfx/grfx_util.cpp
)

list(
    APPEND PPX_IO_HEADER_FILES
    ${INC_DIR}/ppx/io/000_io_config.h
    ${INC_DIR}/ppx/io/file.h
    ${INC_DIR}/ppx/io/image_loader.h
    ${INC_DIR}/ppx/io/obj_loader.h
)

list(
    APPEND PPX_IO_SOURCE_FILES
    ${SRC_DIR}/ppx/io/file.cpp
    ${SRC_DIR}/ppx/io/image_loader.cpp
    ${SRC_DIR}/ppx/io/obj_loader.cpp
)

if (PPX_D3D12)
    list(
        APPEND PPX_GRFX_DX_HEADER_FILES
        ${INC_DIR}/ppx/grfx/dx/000_dx_config.h
        ${INC_DIR}/ppx/grfx/dx/dx_buffer.h
        ${INC_DIR}/ppx/grfx/dx/dx_command.h
        ${INC_DIR}/ppx/grfx/dx/dx_descriptor.h
        ${INC_DIR}/ppx/grfx/dx/dx_descriptor_helper.h
        ${INC_DIR}/ppx/grfx/dx/dx_device.h
        ${INC_DIR}/ppx/grfx/dx/dx_gpu.h
        ${INC_DIR}/ppx/grfx/dx/dx_image.h
        ${INC_DIR}/ppx/grfx/dx/dx_instance.h
        ${INC_DIR}/ppx/grfx/dx/dx_pipeline.h
        ${INC_DIR}/ppx/grfx/dx/dx_queue.h
        ${INC_DIR}/ppx/grfx/dx/dx_render_pass.h
        ${INC_DIR}/ppx/grfx/dx/dx_shader.h
        ${INC_DIR}/ppx/grfx/dx/dx_swapchain.h
        ${INC_DIR}/ppx/grfx/dx/dx_sync.h
        ${INC_DIR}/ppx/grfx/dx/dx_util.h
    )

    list(
        APPEND PPX_GRFX_DX_SOURCE_FILES
        ${SRC_DIR}/ppx/grfx/dx/dx_buffer.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_command.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_descriptor.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_descriptor_helper.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_device.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_gpu.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_image.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_instance.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_pipeline.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_queue.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_render_pass.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_shader.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_swapchain.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_sync.cpp
        ${SRC_DIR}/ppx/grfx/dx/dx_util.cpp
        ${PPX_THIRD_PARTY_DIR}/D3D12MemoryAllocator/src/D3D12MemAlloc.h
        ${PPX_THIRD_PARTY_DIR}/D3D12MemoryAllocator/src/D3D12MemAlloc.cpp
    )
endif()

list(
    APPEND PPX_GRFX_VK_HEADER_FILES
    ${INC_DIR}/ppx/grfx/vk/000_vk_config.h
    ${INC_DIR}/ppx/grfx/vk/000_vk_config_platform.h
    ${INC_DIR}/ppx/grfx/vk/vk_buffer.h
    ${INC_DIR}/ppx/grfx/vk/vk_command.h
    ${INC_DIR}/ppx/grfx/vk/vk_descriptor.h
    ${INC_DIR}/ppx/grfx/vk/vk_device.h
    ${INC_DIR}/ppx/grfx/vk/vk_gpu.h
    ${INC_DIR}/ppx/grfx/vk/vk_image.h
    ${INC_DIR}/ppx/grfx/vk/vk_instance.h
    ${INC_DIR}/ppx/grfx/vk/vk_pipeline.h
    ${INC_DIR}/ppx/grfx/vk/vk_queue.h
    ${INC_DIR}/ppx/grfx/vk/vk_render_pass.h
    ${INC_DIR}/ppx/grfx/vk/vk_shader.h
    ${INC_DIR}/ppx/grfx/vk/vk_swapchain.h
    ${INC_DIR}/ppx/grfx/vk/vk_sync.h
    ${INC_DIR}/ppx/grfx/vk/vk_util.h
)

list(
    APPEND PPX_GRFX_VK_SOURCE_FILES
    ${SRC_DIR}/ppx/grfx/vk/vk_buffer.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_command.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_descriptor.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_device.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_gpu.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_image.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_instance.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_pipeline.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_profiler_fn_wrapper.h
    ${SRC_DIR}/ppx/grfx/vk/vk_profiler_fn_wrapper.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_queue.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_render_pass.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_shader.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_swapchain.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_sync.cpp
    ${SRC_DIR}/ppx/grfx/vk/vk_util.cpp
    ${PPX_THIRD_PARTY_DIR}/VulkanMemoryAllocator/src/vk_mem_alloc.h
)

list(
    APPEND CONTRIB_FILES
    ${PPX_THIRD_PARTY_DIR}/contrib/MIPFile/MIPFile.h
    ${PPX_THIRD_PARTY_DIR}/contrib/MIPFile/MIPFile.cpp
)

# ------------------------------------------------------------------------------
# Source group
# ------------------------------------------------------------------------------

source_group("ppx\\header"          FILES ${PPX_HEADER_FILES})
source_group("ppx\\source"          FILES ${PPX_SOURCE_FILES})
source_group("ppx-grfx\\header"     FILES ${PPX_GRFX_HEADER_FILES})
source_group("ppx-grfx\\source"     FILES ${PPX_GRFX_SOURCE_FILES})
source_group("ppx-grfx-dx\\header"  FILES ${PPX_GRFX_DX_HEADER_FILES})
source_group("ppx-grfx-dx\\source"  FILES ${PPX_GRFX_DX_SOURCE_FILES})
source_group("ppx-grfx-vk\\header"  FILES ${PPX_GRFX_VK_HEADER_FILES})
source_group("ppx-grfx-vk\\source"  FILES ${PPX_GRFX_VK_SOURCE_FILES})
source_group("ppx-io\\header"       FILES ${PPX_IO_HEADER_FILES})
source_group("ppx-io\\source"       FILES ${PPX_IO_SOURCE_FILES})
source_group("third_party\\contrib" FILES ${CONTRIB_FILES})
source_group("third_party\\imgui"   FILES ${IMGUI_HEADER_FILES} ${IMGUI_SOURCE_FILES})
source_group("third_party\\xxHash"  FILES ${XXHASH_FILES})

# ------------------------------------------------------------------------------
# Add library
# ------------------------------------------------------------------------------

add_library(
    ${PROJECT_NAME} STATIC
    ${PPX_HEADER_FILES}
    ${PPX_SOURCE_FILES}
    ${PPX_GRFX_HEADER_FILES}
    ${PPX_GRFX_SOURCE_FILES}
    ${PPX_GRFX_DX_HEADER_FILES}
    ${PPX_GRFX_DX_SOURCE_FILES}
    ${PPX_GRFX_VK_HEADER_FILES}
    ${PPX_GRFX_VK_SOURCE_FILES}
    ${PPX_IO_HEADER_FILES}
    ${PPX_IO_SOURCE_FILES}
    ${CONTRIB_FILES}
    ${IMGUI_HEADER_FILES}
    ${IMGUI_SOURCE_FILES}
    ${XXHASH_FILES}
)

if (PPX_LINUX OR PPX_GGP)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES PREFIX "lib"
    )
endif()
    
# ------------------------------------------------------------------------------
# Include directories
# ------------------------------------------------------------------------------

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${SRC_DIR}
    PUBLIC  ${INC_DIR}
            ${PPX_THIRD_PARTY_DIR}/glfw/include
            ${PPX_THIRD_PARTY_DIR}/glm
            ${PPX_THIRD_PARTY_DIR}/stb
            ${PPX_THIRD_PARTY_DIR}/imgui
            ${PPX_THIRD_PARTY_DIR}/tinyobjloader
            ${PPX_THIRD_PARTY_DIR}/pcg32
            ${PPX_THIRD_PARTY_DIR}/xxHash
)

# Include D3D12 directories if D3D12 is enabled
if (PPX_D3D12)
    target_include_directories(
        ${PROJECT_NAME}
        PRIVATE ${PPX_THIRD_PARTY_DIR}/D3D12MemoryAllocator/src
    )
endif()

# Include Vulkan directories if Vulkan is enabled
if (PPX_VULKAN)
    target_include_directories(
        ${PROJECT_NAME}
        PRIVATE ${PPX_THIRD_PARTY_DIR}/VulkanMemoryAllocator/src
        PUBLIC  ${VULKAN_INCLUDE_DIR}
    )
endif()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES FOLDER "ppx"
)

# ------------------------------------------------------------------------------
# Compile definitions
# ------------------------------------------------------------------------------
target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC  GLFW_INCLUDE_NONE
)

if (PPX_GGP)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC  PPX_GGP
    )
elseif (PPX_LINUX)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC  PPX_LINUX
    )
elseif (PPX_MSW)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC  PPX_MSW
                _CRT_SECURE_NO_WARNINGS
    )
endif()

# ------------------------------------------------------------------------------
# Link libraries
# ------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
    PUBLIC cpu_features
)

# ------------------------------------------------------------------------------
# Graphics API compile definitions
# ------------------------------------------------------------------------------

if (PPX_D3D12)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC  PPX_D3D12
    )
endif()

if (PPX_VULKAN)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC  PPX_VULKAN
    )
    # Platform specific Vulkan defintions
    if (PPX_GGP)
        target_compile_definitions(
            ${PROJECT_NAME}
            PRIVATE
        )
        #
        # Disaable Wno-missing-braces because it's meaningless in the cases
        # that PPX uses. 
        #
        target_compile_options(
            ${PROJECT_NAME}
            PUBLIC -Wno-missing-braces        
		)
    elseif (PPX_LINUX)

        target_compile_definitions(
            ${PROJECT_NAME}
            PUBLIC ${PPX_LINUX_VULKAN_SURFACE}
        )
    elseif (PPX_MSW)
        target_compile_definitions(
            ${PROJECT_NAME}
            PRIVATE
        )
    endif()
endif()

# ------------------------------------------------------------------------------
# Graphics API link libraries
# ------------------------------------------------------------------------------

if (PPX_D3D12)
    # Link in d3d12.lib and dxgi.lib
    if (PPX_D3D12)
        target_link_libraries(${PROJECT_NAME} 
            PUBLIC d3d12.lib dxgi.lib dxguid.lib shcore.lib
        )
    endif()
endif()

if (PPX_VULKAN)
    # GGP
    if (PPX_GGP)
        # Link directories
        target_link_directories(${PROJECT_NAME} 
            PUBLIC ${VULKAN_LIBRARY_DIR}
        )
        # Link libraries
        target_link_libraries(${PROJECT_NAME} 
            PUBLIC vulkan
        ) 
    #Linux
    elseif (PPX_LINUX)
        # Link directories
        target_link_directories(${PROJECT_NAME} 
            PUBLIC ${VULKAN_LIBRARY_DIR}
        )
        # Link libraries
        target_link_libraries(${PROJECT_NAME} 
            PUBLIC vulkan
        )  
    
        # Link properties
        set_target_properties(${PROJECT_NAME}
            PROPERTIES OUTPUT_NAME PPX
                       LINKER_LANGUAGE CXX
        )
        # Link libraries
        target_link_libraries(${PROJECT_NAME} 
            PUBLIC xcb X11-xcb
        )
    # Windows
    elseif (PPX_MSW)
        target_link_libraries(${PROJECT_NAME} 
            PUBLIC "${VULKAN_LIBRARY_DIR}/vulkan-1.lib"
        )
    endif()
endif()

# ------------------------------------------------------------------------------
# Profiling
# ------------------------------------------------------------------------------

if (PPX_ENABLE_PROFILE_GRFX_API_FUNCTIONS)
    target_compile_definitions(
        ${PROJECT_NAME}
        PRIVATE PPX_ENABLE_PROFILE_GRFX_API_FUNCTIONS
    )
endif()