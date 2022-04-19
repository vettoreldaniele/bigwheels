cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(projects)

function(add_shader_dependencies)
    set(target_name ${ARGV0})
    set(shader_tag  ${ARGV1})
    
    if (TARGET ${PROJECT_NAME}-${shader_tag}-vsps)
        add_dependencies(${target_name} ${PROJECT_NAME}-${shader_tag}-vsps)
    endif()
    if (TARGET ${PROJECT_NAME}-${shader_tag}-cs)
        add_dependencies(${target_name} ${PROJECT_NAME}-${shader_tag}-cs)
    endif()
    if (TARGET ${PROJECT_NAME}-${shader_tag}-vs)
        add_dependencies(${target_name} ${PROJECT_NAME}-${shader_tag}-vs)
    endif()
    if (TARGET ${PROJECT_NAME}-${shader_tag}-ps)
        add_dependencies(${target_name} ${PROJECT_NAME}-${shader_tag}-ps)
    endif()
endfunction()

function(add_vk_sample name is_gui)
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
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
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

function(add_dx11_sample name is_gui)
    if (PPX_D3D11)
        set (api_tag "dx11")
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
    
        add_shader_dependencies(${target_name} "dxbc50")

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common   
        )
        
        target_compile_definitions(${target_name} PRIVATE USE_DX11)        

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )
    
        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )    
        endif()    
    endif()
endfunction()

function(add_dx12_sample name is_gui)
    if (PPX_D3D12)
        set (api_tag "dx12")
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
    
        add_shader_dependencies(${target_name} "dxbc51")

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common   
        )
        
        target_compile_definitions(${target_name} PRIVATE USE_DX12)        

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )
    
        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )    
        endif()    
    endif()
endfunction()

function(add_dxil_sample name is_gui)
    if (PPX_D3D12)
        set (api_tag "dxil")
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
    
        add_shader_dependencies(${target_name} "dxil")

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common   
        )
        
        target_compile_definitions(${target_name} PRIVATE USE_DX12 USE_DXIL)        

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )
    
        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )    
        endif()    
    endif()
endfunction()

function(add_dxvk_sample name is_gui)
    if (PPX_DXVK)
        set (api_tag "dxvk")
        set (target_name "${api_tag}_${PROJECT_NAME}")
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
            
        add_shader_dependencies(${target_name} "dxbc50")

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common   
        )
        
        target_compile_definitions(${target_name} PRIVATE USE_DX11)        

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )
    
        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )    
        endif()    
    endif()
endfunction()

function(add_dxvk_spv_sample name is_gui)
    if (PPX_DXVK_SPV)
        set (api_tag "dxvk_spv")
        set (target_name "${api_tag}_${PROJECT_NAME}")
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

        add_shader_dependencies(${target_name} "dxvk-spv")

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common
        )

        target_compile_definitions(${target_name} PRIVATE USE_DX11 USE_DXVK_SPV)

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )

        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )
        endif()
    endif()
endfunction()

function(add_dxiivk_dx12_sample name is_gui)
    if (PPX_DXIIVK)
        set (api_tag "dxiivk_dx12")
        set (target_name "${api_tag}_${PROJECT_NAME}")
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
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common   
        )

        target_compile_definitions(${target_name} PRIVATE USE_DX12)        

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )

        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )    
        endif()    
    endif()
endfunction()

function(add_dxvk_spv_d3dcompile_sample name is_gui)
    if (PPX_DXVK_SPV_D3DCOMPILE)
        set (api_tag "dxvk_spv_d3dcompile")
        set (target_name "${api_tag}_${PROJECT_NAME}")
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

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
                   ${PPX_DIR}/projects/common
        )

        target_compile_definitions(${target_name} PRIVATE USE_DX11 USE_DXVK_SPV)

        target_link_libraries(
            ${target_name}
            PUBLIC ppx
        )

        target_compile_definitions(${target_name} PRIVATE PORTO_D3DCOMPILE)

        if (${is_gui})
            target_link_libraries(
                ${target_name}
                PUBLIC glfw
            )
        endif()
    endif()
endfunction()

#add_subdirectory(00_ppx_info)
add_subdirectory(01_triangle)
add_subdirectory(02_triangle_spinning)
add_subdirectory(03_square_textured)
add_subdirectory(04_cube)
add_subdirectory(05_cube_textured)
add_subdirectory(06_compute_fill)
add_subdirectory(07_draw_indexed)
add_subdirectory(08_basic_geometry)
add_subdirectory(09_obj_geometry)
add_subdirectory(10_cube_map)
add_subdirectory(11_compressed_texture)
add_subdirectory(12_camera)
add_subdirectory(13_normal_map)
add_subdirectory(14_input)
add_subdirectory(15_basic_material)
add_subdirectory(16_gbuffer)
add_subdirectory(17_primitives)
add_subdirectory(18_arcball_camera)
add_subdirectory(19_camera_fit_scene)
add_subdirectory(20_camera_motion)
add_subdirectory(21_text_draw)
add_subdirectory(22_image_filter)
add_subdirectory(27_mipmap_demo)
add_subdirectory(alloc)
add_subdirectory(fishtornado)
add_subdirectory(shadertoy)

