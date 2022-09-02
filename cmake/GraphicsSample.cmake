cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(CMakeParseArguments)

function(_add_shader_dependencies target_name shader_tag)
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

function(_add_sample_internal)
    set(oneValueArgs SAMPLE_NAME API_TAG SHADER_FORMAT)
    set(multiValueArgs API_DEFINES ADDITIONAL_FILES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")
    
    set (target_name "${ARG_API_TAG}_${ARG_SAMPLE_NAME}")

    foreach(filepath ${ARG_ADDITIONAL_FILES})
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
        ${ARG_ADDITIONAL_FILES}
    )

    if (NOT ARG_SHADER_FORMAT STREQUAL "")
        _add_shader_dependencies(${target_name} ${ARG_SHADER_FORMAT})
    endif()
    
    add_dependencies(${target_name} ppx_assets)

    set_target_properties(
        ${target_name}
        PROPERTIES FOLDER "ppx/samples/${ARG_API_TAG}"
    )

    target_include_directories(
        ${target_name}
        PUBLIC ${PPX_DIR}/include
    )

    target_compile_definitions(${target_name} PRIVATE ${ARG_API_DEFINES})

    target_link_libraries(
        ${target_name}
        PUBLIC ppx glfw
    )

    if (NOT TARGET all-${ARG_API_TAG})
        add_custom_target(all-${ARG_API_TAG})
    endif()
    add_dependencies(all-${ARG_API_TAG} ${target_name})
endfunction()

function(add_vk_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_VULKAN)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "vk" 
                   SHADER_FORMAT "spv"
                   API_DEFINES "USE_VK" 
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dx11_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_D3D11)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dx11" 
                   SHADER_FORMAT "dxbc50"
                   API_DEFINES "USE_DX11" 
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dx12_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_D3D12)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dx12" 
                   SHADER_FORMAT "dxbc51"
                   API_DEFINES "USE_DX12" 
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dxil_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_D3D12)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dxil" 
                   SHADER_FORMAT "dxil"
                   API_DEFINES "USE_DX12" "USE_DXIL"
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dxil_spv_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_DXIL_SPV)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dxil_spv" 
                   SHADER_FORMAT "dxil-spv"
                   API_DEFINES "USE_VK" "USE_DXIL_SPV"
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dxvk_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_DXVK)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dxvk" 
                   SHADER_FORMAT "dxbc50"
                   API_DEFINES "USE_DX11"
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dxvk_spv_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_DXVK_SPV)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dxvk_spv" 
                   SHADER_FORMAT "dxvk-spv"
                   API_DEFINES "USE_DX11" "USE_DXVK_SPV"
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_dxvk_spv_d3dcompile_sample)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    if (PPX_DXVK_SPV_D3DCOMPILE)
        _add_sample_internal(SAMPLE_NAME ${ARG_SAMPLE_NAME}
                   API_TAG "dxvk_spv_d3dcompile" 
                   SHADER_FORMAT ""
                   API_DEFINES "USE_DX11" "USE_DXVK_SPV"
                   ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
    endif()
endfunction()

function(add_samples)
    set(multiValueArgs TARGET_APIS ADDITIONAL_FILES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "${multiValueArgs}")
    
    foreach(target_api ${ARG_TARGET_APIS})
        if(target_api STREQUAL "dx12")
            add_dx12_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "dx11")
            add_dx11_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "vk")
            add_vk_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "dxil")
            add_dxil_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "dxil_spv")
            add_dxil_spv_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "dxvk")
            add_dxvk_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "dxvk_spv")
            add_dxvk_spv_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        elseif(target_api STREQUAL "dxvk_spv_d3dcompile")
            add_dxvk_spv_d3dcompile_sample(SAMPLE_NAME ${ARG_SAMPLE_NAME} ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES})
        else()
            message(FATAL_ERROR "Invalid target API \"${target_api}\"" )
        endif()
    endforeach()
endfunction()

function(add_samples_for_all_apis)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "SAMPLE_NAME" "ADDITIONAL_FILES")
    add_samples(
        SAMPLE_NAME ${ARG_SAMPLE_NAME}
        TARGET_APIS "dx12" "dx11" "vk" "dxil" "dxil_spv" "dxvk" "dxvk_spv" "dxvk_spv_d3dcompile"
        ADDITIONAL_FILES ${ARG_ADDITIONAL_FILES}
    )
endfunction()
