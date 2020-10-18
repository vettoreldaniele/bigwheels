cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(projects)

function(add_vk_sample name is_gui)
    set (api_tag "vk")
    set (target_name "${api_tag}_${name}")

    add_executable(
        ${target_name}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

    set_target_properties(
        ${target_name}
        PROPERTIES FOLDER "ppx/projects/${api_tag}"
    )

    target_include_directories(
        ${target_name}
        PUBLIC ${PPX_DIR}/include
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

function(add_dx_sample name is_gui)
    if (PPX_D3D12)
        set (api_tag "dx")
        set (target_name "${api_tag}_${name}")

        add_executable(
            ${target_name}
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
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

        add_executable(
            ${target_name}
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

        set_target_properties(
            ${target_name}
            PROPERTIES FOLDER "ppx/projects/${api_tag}"
        )

        target_include_directories(
            ${target_name}
            PUBLIC ${PPX_DIR}/include
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

add_subdirectory(00_ppx_info)
add_subdirectory(01_triangle)
add_subdirectory(02_triangle_spinning)
add_subdirectory(03_square_textured)
add_subdirectory(04_cube)
add_subdirectory(05_cube_textured)
add_subdirectory(06_compute_fill)
add_subdirectory(07_draw_indexed)
add_subdirectory(shadertoy)

