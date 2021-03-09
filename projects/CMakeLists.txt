cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(projects)

function(add_vk_sample name is_gui)
    set (api_tag "vk")
    set (target_name "${api_tag}_${name}")
    set (additional_files "${ARGV2}")

    foreach(filepath ${additional_files})
        get_filename_component(ext ${filepath} EXT)
        if (("${ext}" STREQUAL ".hlsl") OR ("${ext}" STREQUAL ".hlsli"))
            list(APPEND shader_files ${filepath})
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
        
        target_compile_definitions(${target_name} PRIVATE USE_DX)        

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
        
        target_compile_definitions(${target_name} PRIVATE USE_DX USE_DXIL)        

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
add_subdirectory(alloc)
add_subdirectory(fishtornado)
add_subdirectory(shadertoy)

