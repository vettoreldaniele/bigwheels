cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(libporto)

# Detect Linux
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# ------------------------------------------------------------------------------
# Vulkan header and libraries
# ------------------------------------------------------------------------------

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

message("Found Vulkan for DXVK: ${VULKAN_DIR}")
message("  Vulkan include directory: ${VULKAN_INCLUDE_DIR}")
message("  Vulkan library directory: ${VULKAN_LIBRARY_DIR}")

# ------------------------------------------------------------------------------
# DXVK
# ------------------------------------------------------------------------------

if (PPX_DXVK AND NOT LIBPORTO_DIR)
    message(FATAL_ERROR "PPX_DXVK requires LIBPORTO_DIR to find libporto")
endif()

set(LIBPORTO_INC_DIR "${LIBPORTO_DIR}/include")
set(LIBPORTO_SRC_DIR "${LIBPORTO_DIR}/src")
set(DXVK_DIR         "${LIBPORTO_DIR}/third_party/dxvk")
set(DXVK_INC_DIR     "${DXVK_DIR}/include")
set(DXVK_SRC_DIR     "${DXVK_DIR}/src")

message("LIBPORTO_DIR: ${LIBPORTO_DIR}")
message("DXVK_DIR    : ${DXVK_DIR}")

# ------------------------------------------------------------------------------
# dxvk_core
# ------------------------------------------------------------------------------

list(
    APPEND DXVK_CORE_HDR_FILES
    ${DXVK_SRC_DIR}/dxbc/dxbc_analysis.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_chunk_isgn.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_chunk_shex.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_common.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_compiler.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_decoder.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_defs.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_enums.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_header.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_include.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_modinfo.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_module.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_names.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_options.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_reader.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_tag.h
    ${DXVK_SRC_DIR}/dxbc/dxbc_util.h
    ${DXVK_SRC_DIR}/dxso/dxso_analysis.h
    ${DXVK_SRC_DIR}/dxso/dxso_code.h
    ${DXVK_SRC_DIR}/dxso/dxso_common.h
    ${DXVK_SRC_DIR}/dxso/dxso_compiler.h
    ${DXVK_SRC_DIR}/dxso/dxso_ctab.h
    ${DXVK_SRC_DIR}/dxso/dxso_decoder.h
    ${DXVK_SRC_DIR}/dxso/dxso_enums.h
    ${DXVK_SRC_DIR}/dxso/dxso_header.h
    ${DXVK_SRC_DIR}/dxso/dxso_helpers.h
    ${DXVK_SRC_DIR}/dxso/dxso_include.h
    ${DXVK_SRC_DIR}/dxso/dxso_isgn.h
    ${DXVK_SRC_DIR}/dxso/dxso_modinfo.h
    ${DXVK_SRC_DIR}/dxso/dxso_module.h
    ${DXVK_SRC_DIR}/dxso/dxso_options.h
    ${DXVK_SRC_DIR}/dxso/dxso_reader.h
    ${DXVK_SRC_DIR}/dxso/dxso_tables.h
    ${DXVK_SRC_DIR}/dxso/dxso_util.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_adapter.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_barrier.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_bind_mask.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_buffer.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_cmdlist.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_compute.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_constant_state.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_context.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_context_state.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_cs.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_data.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_descriptor.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_device.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_device_filter.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_device_info.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_extensions.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_extension_provider.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_format.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_framebuffer.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_gpu_event.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_gpu_query.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_graphics.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_graphics_state.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_hash.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_image.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_include.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_instance.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_lifetime.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_limits.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_main.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_memory.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_blit.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_clear.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_copy.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_mipgen.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_pack.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_resolve.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_objects.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_openvr.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_options.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_pipecache.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_pipelayout.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_pipemanager.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_platform_exts.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_queue.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_recycler.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_renderpass.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_resource.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_sampler.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_shader.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_shader_key.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_signal.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_spec_const.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_staging.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_state_cache.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_state_cache_types.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_stats.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_unbound.h
    ${DXVK_SRC_DIR}/dxvk/dxvk_util.h
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud.h
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud_font.h
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud_item.h
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud_renderer.h
    ${DXVK_SRC_DIR}/spirv/spirv_code_buffer.h
    ${DXVK_SRC_DIR}/spirv/spirv_compression.h
    ${DXVK_SRC_DIR}/spirv/spirv_include.h
    ${DXVK_SRC_DIR}/spirv/spirv_instruction.h
    ${DXVK_SRC_DIR}/spirv/spirv_module.h
    ${DXVK_SRC_DIR}/util/com/com_guid.h
    ${DXVK_SRC_DIR}/util/com/com_include.h
    ${DXVK_SRC_DIR}/util/com/com_object.h
    ${DXVK_SRC_DIR}/util/com/com_pointer.h
    ${DXVK_SRC_DIR}/util/com/com_private_data.h
    ${DXVK_SRC_DIR}/util/config/config.h
    ${DXVK_SRC_DIR}/util/log/log.h
    ${DXVK_SRC_DIR}/util/log/log_debug.h
    ${DXVK_SRC_DIR}/util/sha1/sha1.h
    ${DXVK_SRC_DIR}/util/sha1/sha1_util.h
    ${DXVK_SRC_DIR}/util/sync/sync_recursive.h
    ${DXVK_SRC_DIR}/util/sync/sync_signal.h
    ${DXVK_SRC_DIR}/util/sync/sync_signal_win32.h
    ${DXVK_SRC_DIR}/util/sync/sync_spinlock.h
    ${DXVK_SRC_DIR}/util/sync/sync_ticketlock.h
    ${DXVK_SRC_DIR}/util/thread.h
    ${DXVK_SRC_DIR}/util/util_bit.h
    ${DXVK_SRC_DIR}/util/util_enum.h
    ${DXVK_SRC_DIR}/util/util_env.h
    ${DXVK_SRC_DIR}/util/util_error.h
    ${DXVK_SRC_DIR}/util/util_flags.h
    ${DXVK_SRC_DIR}/util/util_gdi.h
    ${DXVK_SRC_DIR}/util/util_lazy.h
    ${DXVK_SRC_DIR}/util/util_likely.h
    ${DXVK_SRC_DIR}/util/util_luid.h
    ${DXVK_SRC_DIR}/util/util_math.h
    ${DXVK_SRC_DIR}/util/util_matrix.h
    ${DXVK_SRC_DIR}/util/util_misc.h
    ${DXVK_SRC_DIR}/util/util_monitor.h
    ${DXVK_SRC_DIR}/util/util_ratio.h
    ${DXVK_SRC_DIR}/util/util_small_vector.h
    ${DXVK_SRC_DIR}/util/util_string.h
    ${DXVK_SRC_DIR}/util/util_time.h
    ${DXVK_SRC_DIR}/util/util_vector.h
    ${DXVK_SRC_DIR}/vulkan/vulkan_loader.h
    ${DXVK_SRC_DIR}/vulkan/vulkan_names.h
    ${DXVK_SRC_DIR}/vulkan/vulkan_presenter.h
    ${DXVK_SRC_DIR}/vulkan/vulkan_util.h    
)

list(
    APPEND DXVK_CORE_SRC_FILES
    ${DXVK_SRC_DIR}/dxbc/dxbc_analysis.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_chunk_isgn.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_chunk_shex.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_common.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_compiler.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_decoder.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_defs.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_header.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_module.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_names.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_options.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_reader.cpp
    ${DXVK_SRC_DIR}/dxbc/dxbc_util.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_analysis.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_code.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_common.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_compiler.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_ctab.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_decoder.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_enums.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_header.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_module.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_options.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_reader.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_tables.cpp
    ${DXVK_SRC_DIR}/dxso/dxso_util.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_adapter.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_barrier.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_buffer.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_cmdlist.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_compute.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_context.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_cs.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_data.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_descriptor.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_device.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_device_filter.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_extensions.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_format.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_framebuffer.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_gpu_event.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_gpu_query.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_graphics.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_image.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_instance.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_lifetime.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_main.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_memory.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_blit.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_clear.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_copy.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_mipgen.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_pack.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_meta_resolve.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_openvr.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_options.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_pipecache.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_pipelayout.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_pipemanager.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_queue.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_renderpass.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_resource.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_sampler.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_shader.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_shader_key.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_signal.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_spec_const.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_staging.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_state_cache.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_stats.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_unbound.cpp
    ${DXVK_SRC_DIR}/dxvk/dxvk_util.cpp
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud.cpp
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud_font.cpp
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud_item.cpp
    ${DXVK_SRC_DIR}/dxvk/hud/dxvk_hud_renderer.cpp
    ${DXVK_SRC_DIR}/dxvk/platform/dxvk_win32_exts.cpp
    ${DXVK_SRC_DIR}/spirv/spirv_code_buffer.cpp
    ${DXVK_SRC_DIR}/spirv/spirv_compression.cpp
    ${DXVK_SRC_DIR}/spirv/spirv_module.cpp
    ${DXVK_SRC_DIR}/util/com/com_guid.cpp
    ${DXVK_SRC_DIR}/util/com/com_private_data.cpp
    ${DXVK_SRC_DIR}/util/config/config.cpp
    ${DXVK_SRC_DIR}/util/log/log.cpp
    ${DXVK_SRC_DIR}/util/log/log_debug.cpp
    ${DXVK_SRC_DIR}/util/sha1/sha1.c
    ${DXVK_SRC_DIR}/util/sha1/sha1_util.cpp
    ${DXVK_SRC_DIR}/util/sync/sync_recursive.cpp
    ${DXVK_SRC_DIR}/util/util_env.cpp
    ${DXVK_SRC_DIR}/util/util_gdi.cpp
    ${DXVK_SRC_DIR}/util/util_luid.cpp
    ${DXVK_SRC_DIR}/util/util_matrix.cpp
    ${DXVK_SRC_DIR}/util/util_monitor.cpp
    ${DXVK_SRC_DIR}/util/util_string.cpp
    ${DXVK_SRC_DIR}/vulkan/vulkan_loader.cpp
    ${DXVK_SRC_DIR}/vulkan/vulkan_names.cpp
    ${DXVK_SRC_DIR}/vulkan/vulkan_presenter.cpp    
)

add_library(
    dxvk_core STATIC
    ${DXVK_CORE_HDR_FILES}
    ${DXVK_CORE_SRC_FILES}
)

set_source_files_properties(${DXVK_SRC_DIR}/util/sha1/sha1.c PROPERTIES LANGUAGE CXX)

set_target_properties(
    dxvk_core
    PROPERTIES FOLDER "dxvk"
)

target_include_directories(
    dxvk_core
    PUBLIC  ${LIBPORTO_INC_DIR}
    PRIVATE ${DXVK_INC_DIR}
            ${DXVK_SRC_DIR}
            ${DXVK_DIR}/porto_generated_headers
            ${LIBPORTO_SRC_DIR}
            ${VULKAN_INCLUDE_DIR}
)
target_include_directories(
    dxvk_core
    INTERFACE ${LIBPORTO_INC_DIR}
)

target_compile_definitions(dxvk_core PUBLIC PORTO_DEVELOPMENT=1)

if (GGP)
    target_compile_definitions(dxvk_core PUBLIC _GNU_SOURCE)
elseif (WIN32)
    target_compile_definitions(dxvk_core PUBLIC NOMINMAX)
endif()

if (GGP)
    target_compile_options(dxvk_core PUBLIC -fshort-wchar -Wno-ignored-attributes -Wno-unused-private-field -Wno-missing-braces -Wno-unused-const-variable)
elseif (WIN32)
    target_compile_options(dxvk_core PUBLIC /wd4267 /wd4244 /wd4146)
endif()

if (GGP)
    target_link_directories(dxvk_core PUBLIC ${VULKAN_LIBRARY_DIR} )
    target_link_libraries(dxvk_core PUBLIC vulkan) 
elseif (WIN32)
    target_link_libraries(dxvk_core PUBLIC "${VULKAN_LIBRARY_DIR}/vulkan-1.lib")
endif()

# ------------------------------------------------------------------------------
# dxvk_d3d11
# ------------------------------------------------------------------------------

list(
    APPEND DXVK_D3D11_HDR_FILES
    ${DXVK_SRC_DIR}/d3d10/d3d10_blend.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_buffer.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_depth_stencil.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_device.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_input_layout.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_multithread.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_query.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_rasterizer.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_sampler.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_texture.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_util.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_view_dsv.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_view_rtv.h
    ${DXVK_SRC_DIR}/d3d10/d3d10_view_srv.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_annotation.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_blend.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_buffer.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_class_linkage.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_cmd.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_cmdlist.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_context.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_def.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_ext.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_imm.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_state.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_depth_stencil.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_device.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_device_child.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_enums.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_gdi.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_include.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_initializer.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_input_layout.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_interfaces.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_interop.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_options.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_query.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_rasterizer.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_resource.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_sampler.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_shader.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_state.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_state_object.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_swapchain.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_texture.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_util.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_view.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_dsv.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_rtv.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_srv.h
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_uav.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_format.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_monitor.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_swapchain.h
)

list(
    APPEND DXVK_D3D11_SRC_FILES
    ${DXVK_SRC_DIR}/d3d10/d3d10_blend.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_buffer.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_depth_stencil.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_device.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_input_layout.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_multithread.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_query.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_rasterizer.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_sampler.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_texture.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_util.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_view_dsv.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_view_rtv.cpp
    ${DXVK_SRC_DIR}/d3d10/d3d10_view_srv.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_annotation.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_blend.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_buffer.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_class_linkage.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_cmdlist.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_context.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_def.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_ext.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_context_imm.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_depth_stencil.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_device.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_enums.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_gdi.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_initializer.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_input_layout.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_interop.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_main.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_options.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_query.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_rasterizer.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_resource.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_sampler.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_shader.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_state.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_state_object.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_swapchain.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_texture.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_util.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_dsv.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_rtv.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_srv.cpp
    ${DXVK_SRC_DIR}/d3d11/d3d11_view_uav.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_format.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_monitor.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_swapchain.cpp
)

add_library(
    dxvk_d3d11 STATIC
    ${DXVK_D3D11_HDR_FILES}
    ${DXVK_D3D11_SRC_FILES}
)

set_target_properties(
    dxvk_d3d11
    PROPERTIES FOLDER "dxvk"
)

target_include_directories(
    dxvk_d3d11
    PUBLIC  ${LIBPORTO_INC_DIR}
    PRIVATE ${DXVK_INC_DIR}
            ${DXVK_SRC_DIR}
            ${DXVK_DIR}/porto_generated_headers
            ${VULKAN_INCLUDE_DIR}
            ${LIBPORTO_SRC_DIR}
)

target_compile_definitions(dxvk_d3d11 PUBLIC PORTO_DEVELOPMENT=1)

if (GGP)
    target_compile_definitions(dxvk_d3d11 PUBLIC _GNU_SOURCE)
elseif (WIN32)
    target_compile_definitions(dxvk_d3d11 PUBLIC NOMINMAX)
endif()

if (GGP)
    target_compile_options(dxvk_d3d11 PUBLIC -fshort-wchar -Wno-ignored-attributes -Wno-unused-private-field -Wno-missing-braces -Wno-unused-const-variable)
elseif (WIN32)
    target_compile_options(dxvk_d3d11 PUBLIC /wd4267 /wd4244 /wd4146)
endif()

if (GGP)
    target_link_directories(dxvk_d3d11 PUBLIC ${VULKAN_LIBRARY_DIR} )
    target_link_libraries(dxvk_d3d11 PUBLIC vulkan) 
elseif (WIN32)
    target_link_libraries(dxvk_d3d11 PUBLIC "${VULKAN_LIBRARY_DIR}/vulkan-1.lib")
endif()

# ------------------------------------------------------------------------------
# dxvk_dxgi
# ------------------------------------------------------------------------------

list(
    APPEND DXVK_DXGI_HDR_FILES
    ${DXVK_SRC_DIR}/dxgi/dxgi_adapter.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_enums.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_factory.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_format.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_include.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_interfaces.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_monitor.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_object.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_options.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_output.h
    ${DXVK_SRC_DIR}/dxgi/dxgi_swapchain.h
)

list(
    APPEND DXVK_DXGI_SRC_FILES
    ${DXVK_SRC_DIR}/dxgi/dxgi_adapter.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_enums.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_factory.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_format.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_main.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_monitor.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_options.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_output.cpp
    ${DXVK_SRC_DIR}/dxgi/dxgi_swapchain.cpp
)

add_library(
    dxvk_dxgi STATIC
    ${DXVK_DXGI_HDR_FILES}
    ${DXVK_DXGI_SRC_FILES}
)

set_target_properties(
    dxvk_dxgi
    PROPERTIES FOLDER "dxvk"
)

target_include_directories(
    dxvk_dxgi
    PUBLIC  ${LIBPORTO_INC_DIR}
    PRIVATE ${DXVK_INC_DIR}
            ${DXVK_SRC_DIR}
            ${DXVK_DIR}/porto_generated_headers
            ${LIBPORTO_SRC_DIR}
            ${VULKAN_INCLUDE_DIR}
)

target_compile_definitions(dxvk_dxgi PUBLIC PORTO_DEVELOPMENT=1)

if (GGP)
    target_compile_definitions(dxvk_dxgi PUBLIC _GNU_SOURCE)
elseif (WIN32)
    target_compile_definitions(dxvk_dxgi PUBLIC NOMINMAX)
endif()

if (GGP)
    target_compile_options(dxvk_dxgi PUBLIC -fshort-wchar -Wno-ignored-attributes -Wno-unused-private-field -Wno-missing-braces -Wno-unused-const-variable)
elseif (WIN32)
    target_compile_options(dxvk_dxgi PUBLIC /wd4267 /wd4244 /wd4146)
endif()

if (GGP)
    target_link_directories(dxvk_dxgi PUBLIC ${VULKAN_LIBRARY_DIR} )
    target_link_libraries(dxvk_dxgi PUBLIC vulkan) 
elseif (WIN32)
    target_link_libraries(dxvk_dxgi PUBLIC "${VULKAN_LIBRARY_DIR}/vulkan-1.lib")
endif()

# ------------------------------------------------------------------------------
# libporto
# ------------------------------------------------------------------------------

list(
    APPEND LIBPORTO_HDR_FILES
    ${LIBPORTO_INC_DIR}/combaseapi.h
    ${LIBPORTO_INC_DIR}/constants.h
    ${LIBPORTO_INC_DIR}/c_cpp_types.h
    ${LIBPORTO_INC_DIR}/d3d10.h
    ${LIBPORTO_INC_DIR}/d3d10effect.h
    ${LIBPORTO_INC_DIR}/d3d10misc.h
    ${LIBPORTO_INC_DIR}/d3d10shader.h
    ${LIBPORTO_INC_DIR}/d3d10_1.h
    ${LIBPORTO_INC_DIR}/d3d11.h
    ${LIBPORTO_INC_DIR}/d3d11sdklayers.h
    ${LIBPORTO_INC_DIR}/d3d11shader.h
    ${LIBPORTO_INC_DIR}/d3d11_1.h
    ${LIBPORTO_INC_DIR}/d3d11_2.h
    ${LIBPORTO_INC_DIR}/d3d11_3.h
    ${LIBPORTO_INC_DIR}/d3d11_4.h
    ${LIBPORTO_INC_DIR}/d3d11_structs.h
    ${LIBPORTO_INC_DIR}/d3d9.h
    ${LIBPORTO_INC_DIR}/d3d9caps.h
    ${LIBPORTO_INC_DIR}/d3d9types.h
    ${LIBPORTO_INC_DIR}/d3dcommon.h
    ${LIBPORTO_INC_DIR}/d3d_enums.h
    ${LIBPORTO_INC_DIR}/dxgi.h
    ${LIBPORTO_INC_DIR}/dxgi1_2.h
    ${LIBPORTO_INC_DIR}/dxgi1_3.h
    ${LIBPORTO_INC_DIR}/dxgi1_3_functions.h
    ${LIBPORTO_INC_DIR}/dxgi1_4.h
    ${LIBPORTO_INC_DIR}/dxgi1_5.h
    ${LIBPORTO_INC_DIR}/dxgi1_6.h
    ${LIBPORTO_INC_DIR}/functions/d3d10_1_functions.h
    ${LIBPORTO_INC_DIR}/functions/d3d10_functions.h
    ${LIBPORTO_INC_DIR}/functions/d3d11_functions.h
    ${LIBPORTO_INC_DIR}/functions/d3d9_functions.h
    ${LIBPORTO_INC_DIR}/functions/dxgi1_3_functions.h
    ${LIBPORTO_INC_DIR}/functions/dxgi_functions.h
    ${LIBPORTO_INC_DIR}/functions/porto_functions.h
    ${LIBPORTO_INC_DIR}/function_pointer_types.h
    ${LIBPORTO_INC_DIR}/macros.h
    ${LIBPORTO_INC_DIR}/porto.h
    ${LIBPORTO_INC_DIR}/sal.h
    ${LIBPORTO_INC_DIR}/types.h
    ${LIBPORTO_INC_DIR}/unknwn.h
    ${LIBPORTO_INC_DIR}/uuid.h
    ${LIBPORTO_INC_DIR}/windows.h
    ${LIBPORTO_INC_DIR}/wingdi.h
    ${LIBPORTO_INC_DIR}/winuser.h
    ${LIBPORTO_SRC_DIR}/config/logging_config.h
    ${LIBPORTO_SRC_DIR}/config/porto_config.h
    ${LIBPORTO_SRC_DIR}/porto/porto_global.h
    ${LIBPORTO_SRC_DIR}/porto/porto_impl.h
    ${LIBPORTO_SRC_DIR}/porto/porto_impl_builder.h
    ${LIBPORTO_SRC_DIR}/porto/strings.h
    ${LIBPORTO_SRC_DIR}/porto/string_conversion.h
    ${LIBPORTO_SRC_DIR}/porto/string_copy.h
    ${LIBPORTO_SRC_DIR}/porto/u16string_util.h
    ${LIBPORTO_SRC_DIR}/porto_core/logger.h
    ${LIBPORTO_SRC_DIR}/porto_core/logger_global.h
    ${LIBPORTO_SRC_DIR}/porto_core/logging.h
    ${LIBPORTO_SRC_DIR}/porto_core/logging_parameter_packer.h
    ${LIBPORTO_SRC_DIR}/porto_core/macros.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_core.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_core_global.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_c/logging.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_c/log_format.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_c/porto_core.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_c/porto_types.h
    ${LIBPORTO_SRC_DIR}/porto_core/porto_hwnd.h
)

list(
    APPEND LIBPORTO_SRC_FILES
    ${LIBPORTO_SRC_DIR}/forwards/porto_forwards.cpp
    ${LIBPORTO_SRC_DIR}/porto/porto_global.cpp
    ${LIBPORTO_SRC_DIR}/porto/porto_impl.cpp
    ${LIBPORTO_SRC_DIR}/porto/string_conversion.cpp
    ${LIBPORTO_SRC_DIR}/porto_core/logging.cpp
    ${LIBPORTO_SRC_DIR}/porto_core/porto_core.cpp
    ${LIBPORTO_SRC_DIR}/porto_core/porto_hwnd.cpp
)

if (GGP)
    add_library(
        libporto STATIC
        ${LIBPORTO_HDR_FILES}
        ${LIBPORTO_SRC_FILES}
    )

    set_target_properties(
        libporto
        PROPERTIES FOLDER "libporto"
    )

    target_include_directories(
        libporto
        PUBLIC  ${LIBPORTO_INC_DIR}
    )

    target_compile_definitions(libporto PUBLIC PORTO_DEVELOPMENT=1)
    target_compile_definitions(libporto PUBLIC _GNU_SOURCE)
    target_compile_options(libporto PUBLIC -fshort-wchar -Wno-ignored-attributes -Wno-unused-private-field -Wno-missing-braces -Wno-unused-const-variable)
endif()