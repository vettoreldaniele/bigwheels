cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(benchmarks)

function(add_vk_benchmark name is_gui)
    if (PPX_VULKAN)
        set (api_tag "vk")
        set (target_name "${api_tag}_${name}")
        set (additional_files "${ARGV2}")

        foreach(filepath ${additional_files})
            get_filename_component(ext ${filepath} EXT)
            if (("${ext}" STREQUAL ".hlsl") OR ("${ext}" STREQUAL ".hlsli"))
                list(APPEND shader_files ${filepath})
                set_source_files_properties(${filepath} PROPERTIES VS_TOOL_OVERRIDE "None")
            endif()
        endforeach()
        source_group("Shader Files" FILES ${shader_files})

        add_executable(
            ${target_name}
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
            ${additional_files}
        )

        add_shader_dependencies(${target_name} "spv")

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/benchmarks/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common
        )

        target_compile_definitions(${target_name} PRIVATE USE_VK)

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )

        if (is_gui)
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )
        endif()
    endif()
endfunction()

add_subdirectory(primitive_assembly)
add_subdirectory(texture_load)
add_subdirectory(texture_sample)
add_subdirectory(draw_call)
add_subdirectory(texture_transfer_cpu_to_gpu)
add_subdirectory(overdraw)
