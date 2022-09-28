cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(Utils)
include(CMakeParseArguments)

# Target to depend on all shaders, to force-build all shaders.
add_custom_target("all-shaders")

function(make_output_dir OUTPUT_FILE)
    get_filename_component(PARENT_DIR ${OUTPUT_FILE} DIRECTORY)
    if (NOT EXISTS "${PARENT_DIR}")
        file(MAKE_DIRECTORY "${PARENT_DIR}")
        message(STATUS "creating output directory: ${PARENT_DIR}")
    endif()
endfunction()

function(internal_add_compile_shader_target TARGET_NAME)
    set(oneValueArgs COMPILER_PATH SOURCES OUTPUT_FILE SHADER_STAGE OUTPUT_FORMAT)
    set(multiValueArgs COMPILER_FLAGS INCLUDES)
    cmake_parse_arguments(PARSE_ARGV 1 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    string(TOUPPER "${ARG_SHADER_STAGE}" ARG_SHADER_STAGE)

    make_output_dir("${ARG_OUTPUT_FILE}")
    add_custom_command(
        OUTPUT "${ARG_OUTPUT_FILE}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "------ Compiling ${ARG_SHADER_STAGE} Shader [${ARG_OUTPUT_FORMAT}] ------"
        MAIN_DEPENDENCY "${ARG_SOURCE}"
        DEPENDS ${ARG_INCLUDES}
        COMMAND ${CMAKE_COMMAND} -E echo "[${ARG_OUTPUT_FORMAT}] Compiling ${ARG_SHADER_STAGE} ${ARG_SOURCE} to ${ARG_OUTPUT_FILE}"
        COMMAND "${ARG_COMPILER_PATH}" ${ARG_COMPILER_FLAGS} -Fo "${ARG_OUTPUT_FILE}" "${ARG_SOURCE}"
    )
    add_custom_target("${TARGET_NAME}" DEPENDS "${ARG_OUTPUT_FILE}" SOURCES "${ARG_SOURCE}" ${ARG_INCLUDES})
endfunction()

function(internal_generate_rules_for_shader TARGET_NAME)
    set(oneValueArgs SOURCE SHADER_STAGE)
    set(multiValueArgs INCLUDES)
    cmake_parse_arguments(PARSE_ARGV 1 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    string(REPLACE ".hlsl" "" BASE_NAME "${ARG_SOURCE}")
    get_filename_component(BASE_NAME "${BASE_NAME}" NAME)
    file(RELATIVE_PATH PATH_PREFIX "${PPX_DIR}" "${ARG_SOURCE}")
    get_filename_component(PATH_PREFIX "${PATH_PREFIX}" DIRECTORY)

    # D3D11, dxbc50, sm 5_0.
    if (PPX_D3D11)
        internal_add_compile_shader_target(
            "d3d11_${TARGET_NAME}_${ARG_SHADER_STAGE}"
            COMPILER_PATH "${FXC_PATH}"
            SOURCE "${ARG_SOURCE}"
            INCLUDES ${ARG_INCLUDES}
            OUTPUT_FILE "${CMAKE_BINARY_DIR}/${PATH_PREFIX}/dxbc50/${BASE_NAME}.${ARG_SHADER_STAGE}.dxbc50"
            SHADER_STAGE "${ARG_SHADER_STAGE}"
            OUTPUT_FORMAT "DXCB_5_0"
            COMPILER_FLAGS "-T" "${ARG_SHADER_STAGE}_5_0" "-E" "${ARG_SHADER_STAGE}main" "/DPPX_D3D11=1")
        add_dependencies("d3d11_${TARGET_NAME}" "d3d11_${TARGET_NAME}_${ARG_SHADER_STAGE}")
    endif ()

    # D3D12, dxbc51, sm 5_1.
    if (PPX_D3D12)
        internal_add_compile_shader_target(
            "d3d12_${TARGET_NAME}_${ARG_SHADER_STAGE}"
            COMPILER_PATH "${FXC_PATH}"
            SOURCE "${ARG_SOURCE}"
            INCLUDES ${ARG_INCLUDES}
            OUTPUT_FILE "${CMAKE_BINARY_DIR}/${PATH_PREFIX}/dxbc51/${BASE_NAME}.${ARG_SHADER_STAGE}.dxbc51"
            SHADER_STAGE "${ARG_SHADER_STAGE}"
            OUTPUT_FORMAT "DXBC_5_1"
            COMPILER_FLAGS "-T" "${ARG_SHADER_STAGE}_5_1" "-E" "${ARG_SHADER_STAGE}main" "/DPPX_D3D12=1")
        add_dependencies("d3d12_${TARGET_NAME}" "d3d12_${TARGET_NAME}_${ARG_SHADER_STAGE}")
    endif ()

    # D3D12 / DXIL, dxil, sm 6_5.
    if (PPX_D3D12 OR PPX_DXIL_SPV)
        internal_add_compile_shader_target(
            "dxil_${TARGET_NAME}_${ARG_SHADER_STAGE}"
            COMPILER_PATH "${DXC_PATH}"
            SOURCE "${ARG_SOURCE}"
            INCLUDES ${ARG_INCLUDES}
            OUTPUT_FILE "${CMAKE_BINARY_DIR}/${PATH_PREFIX}/dxil/${BASE_NAME}.${ARG_SHADER_STAGE}.dxil"
            SHADER_STAGE "${ARG_SHADER_STAGE}"
            OUTPUT_FORMAT "DXIL_6_5"
            COMPILER_FLAGS "-T" "${ARG_SHADER_STAGE}_6_5" "-E" "${ARG_SHADER_STAGE}main" "-DPPX_DX12=1")
        add_dependencies("dxil_${TARGET_NAME}" "dxil_${TARGET_NAME}_${ARG_SHADER_STAGE}")
    endif ()

    # Vulkan w/ DXIL from SPV, spv, sm_X_Y - depends on the sm that the DXIL was compiled with
    if (PPX_DXIL_SPV)
        set(DXIL_OUTPUT_FILE "${CMAKE_BINARY_DIR}/${PATH_PREFIX}/dxil/${BASE_NAME}.${ARG_SHADER_STAGE}.dxil")
        set(SPV_OUTPUT_FILE "${CMAKE_BINARY_DIR}/${PATH_PREFIX}/dxil_spv/${BASE_NAME}.${ARG_SHADER_STAGE}.spv")
        add_custom_command(
            OUTPUT "${SPV_OUTPUT_FILE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "----- Compiling shader from DXIL to SPV -----"
            MAIN_DEPENDENCY "${DXIL_INPUT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E echo "[DXIL-SPV] Compiling ${DXIL_INPUT_FILE} to ${SPV_OUTPUT_FILE}"
            COMMAND "${DXIL_SPIRV_PATH}" "${DXIL_INPUT_FILE}" --output "${SPV_OUTPUT_FILE}")
        add_custom_target("dxilspv_${TARGET_NAME}_${ARG_SHADER_STAGE}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
        add_dependencies("dxilspv_${TARGET_NAME}" "dxilspv_${TARGET_NAME}_${ARG_SHADER_STAGE}")
    endif ()

    # Vulkan, spv, sm 6_6.
    if (PPX_VULKAN)
        internal_add_compile_shader_target(
            "vk_${TARGET_NAME}_${ARG_SHADER_STAGE}"
            COMPILER_PATH "${DXC_PATH}"
            SOURCE "${ARG_SOURCE}"
            INCLUDES ${ARG_INCLUDES}
            OUTPUT_FILE "${CMAKE_BINARY_DIR}/${PATH_PREFIX}/spv/${BASE_NAME}.${ARG_SHADER_STAGE}.spv"
            SHADER_STAGE "${ARG_SHADER_STAGE}"
            OUTPUT_FORMAT "SPV_6_6"
            COMPILER_FLAGS "-spirv" "-fspv-reflect" "-fvk-use-dx-layout" "-DPPX_VULKAN=1" "-T" "${ARG_SHADER_STAGE}_6_6" "-E" "${ARG_SHADER_STAGE}main")
        add_dependencies("vk_${TARGET_NAME}" "vk_${TARGET_NAME}_${ARG_SHADER_STAGE}")
    endif ()
endfunction()

function(generate_rules_for_shader TARGET_NAME)
    set(oneValueArgs SOURCE)
    set(multiValueArgs INCLUDES STAGES)
    cmake_parse_arguments(PARSE_ARGV 1 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    add_custom_target("${TARGET_NAME}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
    message(STATUS "creating shader target ${TARGET_NAME}.")
    add_dependencies("all-shaders" "${TARGET_NAME}")

    if (PPX_D3D11)
        add_custom_target("d3d11_${TARGET_NAME}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
        add_dependencies("${TARGET_NAME}" "d3d11_${TARGET_NAME}")
    endif ()
    if (PPX_D3D12)
        add_custom_target("d3d12_${TARGET_NAME}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
        add_dependencies("${TARGET_NAME}" "d3d12_${TARGET_NAME}")
    endif ()
    if (PPX_D3D12 OR PPX_DXIL_SPV)
        add_custom_target("dxil_${TARGET_NAME}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
        add_dependencies("${TARGET_NAME}" "dxil_${TARGET_NAME}")
    endif ()
    if (PPX_DXIL_SPV)
        add_custom_target("dxilspv_${TARGET_NAME}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
        add_dependencies("${TARGET_NAME}" "dxilspv_${TARGET_NAME}")
    endif ()
    if (PPX_VULKAN)
        add_custom_target("vk_${TARGET_NAME}" SOURCES ${ARG_SOURCE} ${ARG_INCLUDES})
        add_dependencies("${TARGET_NAME}" "vk_${TARGET_NAME}")
    endif ()

    foreach (STAGE ${ARG_STAGES})
        internal_generate_rules_for_shader("${TARGET_NAME}" SOURCE "${ARG_SOURCE}" INCLUDES ${ARG_INCLUDES} SHADER_STAGE "${STAGE}")
    endforeach ()
endfunction()

function(generate_group_rule_for_shader TARGET_NAME)
    cmake_parse_arguments(PARSE_ARGV 1 "ARG" "" NONE "CHILDREN")

    add_custom_target("${TARGET_NAME}" DEPENDS ${ARG_CHILDREN})

    if (PPX_D3D11)
        prefix_all(PREFIXED_CHILDREN LIST ${ARG_CHILDREN} PREFIX "d3d11_")
        add_custom_target("d3d11_${TARGET_NAME}" DEPENDS ${PREFIXED_CHILDREN})
    endif ()
    if (PPX_D3D12)
        prefix_all(PREFIXED_CHILDREN LIST ${ARG_CHILDREN} PREFIX "d3d12_")
        add_custom_target("d3d12_${TARGET_NAME}" DEPENDS ${PREFIXED_CHILDREN})
    endif ()
    if (PPX_D3D12 OR PPX_DXIL_SPV)
        prefix_all(PREFIXED_CHILDREN LIST ${ARG_CHILDREN} PREFIX "dxil_")
        add_custom_target("dxil_${TARGET_NAME}" DEPENDS ${PREFIXED_CHILDREN})
    endif ()
    if (PPX_DXIL_SPV)
        prefix_all(PREFIXED_CHILDREN LIST ${ARG_CHILDREN} PREFIX "dxilspv_")
        add_custom_target("dxilspv_${TARGET_NAME}" DEPENDS ${PREFIXED_CHILDREN})
    endif ()
    if (PPX_VULKAN)
        prefix_all(PREFIXED_CHILDREN LIST ${ARG_CHILDREN} PREFIX "vk_")
        add_custom_target("vk_${TARGET_NAME}" DEPENDS ${PREFIXED_CHILDREN})
    endif ()
endfunction()
