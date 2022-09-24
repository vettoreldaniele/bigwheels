cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(CMakeParseArguments)

function(_add_sample_internal)
    set(oneValueArgs NAME API_TAG SHADER_FORMAT)
    set(multiValueArgs API_DEFINES SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    set (TARGET_NAME "${ARG_API_TAG}_${ARG_NAME}")
    add_executable("${TARGET_NAME}" ${ARG_SOURCES})
    set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "ppx/samples/${ARG_API_TAG}")

    target_include_directories("${TARGET_NAME}" PUBLIC ${PPX_DIR}/include)
    target_compile_definitions("${TARGET_NAME}" PRIVATE ${ARG_API_DEFINES})

    target_link_libraries("${TARGET_NAME}" PUBLIC ppx glfw)

    add_dependencies("${TARGET_NAME}" ppx_assets)
    if (DEFINED ARG_DEPENDENCIES)
        add_dependencies("${TARGET_NAME}" ${ARG_DEPENDENCIES})
    endif ()

    if (NOT TARGET all-${ARG_API_TAG})
        add_custom_target(all-${ARG_API_TAG})
    endif()
    add_dependencies(all-${ARG_API_TAG} "${TARGET_NAME}")
endfunction()

function(add_vk_sample)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")
    if (PPX_VULKAN)
        _add_sample_internal(NAME ${ARG_NAME}
                   API_TAG "vk"
                   SHADER_FORMAT "spv"
                   API_DEFINES "USE_VK"
                   SOURCES ${ARG_SOURCES}
                   DEPENDENCIES ${ARG_DEPENDENCIES})
    endif()
endfunction()

function(add_dx11_sample)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")
    if (PPX_D3D11)
        _add_sample_internal(NAME ${ARG_NAME}
                   API_TAG "dx11"
                   SHADER_FORMAT "dxbc50"
                   API_DEFINES "USE_DX11"
                   SOURCES ${ARG_SOURCES}
                   DEPENDENCIES ${ARG_DEPENDENCIES})
    endif()
endfunction()

function(add_dx12_sample)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")
    if (PPX_D3D12)
        _add_sample_internal(NAME ${ARG_NAME}
                   API_TAG "dx12"
                   SHADER_FORMAT "dxbc51"
                   API_DEFINES "USE_DX12"
                   SOURCES ${ARG_SOURCES}
                   DEPENDENCIES ${ARG_DEPENDENCIES})
    endif()
endfunction()

function(add_dxil_sample)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")
    if (PPX_D3D12)
        _add_sample_internal(NAME ${ARG_NAME}
                   API_TAG "dxil"
                   SHADER_FORMAT "dxil"
                   API_DEFINES "USE_DX12" "USE_DXIL"
                   SOURCES ${ARG_SOURCES}
                   DEPENDENCIES ${ARG_DEPENDENCIES})
    endif()
endfunction()

function(add_dxil_spv_sample)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")
    if (PPX_DXIL_SPV)
        _add_sample_internal(NAME ${ARG_NAME}
                   API_TAG "dxil_spv"
                   SHADER_FORMAT "dxil-spv"
                   API_DEFINES "USE_VK" "USE_DXIL_SPV"
                   SOURCES ${ARG_SOURCES}
                   DEPENDENCIES ${ARG_DEPENDENCIES})
    endif()
endfunction()

function(add_samples)
    set(multiValueArgs TARGET_APIS SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")

    foreach(target_api ${ARG_TARGET_APIS})
        if(target_api STREQUAL "dx12")
            add_dx12_sample(NAME ${ARG_NAME} SOURCES ${ARG_SOURCES} DEPENDENCIES ${ARG_DEPENDENCIES})
        elseif(target_api STREQUAL "dx11")
            add_dx11_sample(NAME ${ARG_NAME} SOURCES ${ARG_SOURCES} DEPENDENCIES ${ARG_DEPENDENCIES})
        elseif(target_api STREQUAL "vk")
            add_vk_sample(NAME ${ARG_NAME} SOURCES ${ARG_SOURCES} DEPENDENCIES ${ARG_DEPENDENCIES})
        elseif(target_api STREQUAL "dxil")
            add_dxil_sample(NAME ${ARG_NAME} SOURCES ${ARG_SOURCES} DEPENDENCIES ${ARG_DEPENDENCIES})
        elseif(target_api STREQUAL "dxil_spv")
            add_dxil_spv_sample(NAME ${ARG_NAME} SOURCES ${ARG_SOURCES} DEPENDENCIES ${ARG_DEPENDENCIES})
        else()
            message(FATAL_ERROR "Invalid target API \"${target_api}\"" )
        endif()
    endforeach()
endfunction()

function(add_samples_for_all_apis)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME" "${multiValueArgs}")
    add_samples(
        NAME ${ARG_NAME}
        TARGET_APIS "dx12" "dx11" "vk" "dxil" "dxil_spv"
        SOURCES ${ARG_SOURCES}
        DEPENDENCIES ${ARG_DEPENDENCIES}
    )
endfunction()
