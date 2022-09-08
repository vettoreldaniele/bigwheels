cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(CMakeParseArguments)

message("Adding shader compile functions for HLSL shaders")

set(PPX_FXC_DX11_FLAGS   "/DPPX_D3D11=1")
set(PPX_FXC_DX12_FLAGS   "/DPPX_D3D12=1")
set(PPX_DXC_DXIL_FLAGS   "-DPPX_DX12=1")
set(PPX_DXC_VULKAN_FLAGS "-DPPX_VULKAN=1")
list(APPEND PPX_DXC_DXVK_FLAGS
    "-fvk-use-dx-layout" 
    "-fvk-use-dx-position-w"
    "-fvk-auto-shift-bindings" 
    "-fvk-b-shift" "160" "0" 
    "-fvk-s-shift" "176" "0" 
    "-fvk-t-shift" "192" "0"
    "-fvk-u-shift" "960" "0"
    "-DPPX_D3D11=1")

# If DXC_PATH isn't passed in, look for DXC included in the Vulkan SDK.
if (DEFINED ENV{VULKAN_SDK} AND NOT DXC_PATH)
    if (WIN32)
        set(VK_SDK_DXC "$ENV{VULKAN_SDK}\\Bin\\dxc.exe")
    else()
        set(VK_SDK_DXC "$ENV{VULKAN_SDK}/bin/dxc")
    endif()

    message("Looking for DXC at ${VK_SDK_DXC}")
    if (EXISTS "${VK_SDK_DXC}")
        message("   DXC found at ${VK_SDK_DXC}")
        set (DXC_PATH "${VK_SDK_DXC}")
    else()
        message("   DXC not found at ${VK_SDK_DXC}")
    endif()
endif()

if (NOT DXC_PATH)
    message(FATAL_ERROR "Could not locate DXC executable - DXC is required")
endif()

if (PPX_DXIL_SPV AND NOT DXIL_SPIRV_PATH)
    message(FATAL_ERROR "Could not locate dxil-spirv executable - dxil-spirv is required")
endif()

if(ENABLE_HLSL_BINDING_SEMANTICS)
    message("Compiling SPIR-V shaders with HLSL binding semantics")
    set(DXC_HLSL_BINDING_SEMANTICS_FLAG "-fvk-hlsl-binding-semantics")
else()
    set(DXIL_SPIRV_HLSL_BINDING_SEMANTICS_FLAG "--disable-hlsl-binding-semantics")
endif()

# To support compiling shaders to DXBC we need to know the FXC_PATH.
# In order to find that, we need CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION.
# However, when a custom TOOLCHAIN is used, that variable is not set. This
# logic tries to infer CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION in that case.
if (WIN32 AND NOT FXC_PATH)
    if (NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        set(TARGET_DIR "C:\\Program Files (x86)\\Windows Kits\\10\\bin")
        file(GLOB children RELATIVE ${TARGET_DIR} ${TARGET_DIR}/*)
        foreach(child ${children})
            if (IS_DIRECTORY ${TARGET_DIR}\\${child} AND (${child} MATCHES 10.*))
                set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION ${child})
            endif()
        endforeach()
    endif()

    if (NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        message(FATAL_ERROR "Windows SDK not found on this system. Make sure that CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is set.")
    endif()
    message("CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is set to: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}.")
   
    set(FXC_PATH "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}\\x64\\fxc.exe")
    if (NOT EXISTS "${FXC_PATH}")
        message(FATAL_ERROR "Could not locate FXC executable at ${FXC_PATH} - FXC is required")
    endif()
endif()

message("DXC_PATH: ${DXC_PATH}")
message("FXC_PATH: ${FXC_PATH}")

function(_CompileShaderMakeOutputDir OUTPUT_FILE)
    get_filename_component(PARENT_DIR ${OUTPUT_FILE} DIRECTORY)
    if (NOT EXISTS "${PARENT_DIR}")
        file(MAKE_DIRECTORY "${PARENT_DIR}")
        message("Created shader binary output directory: ${PARENT_DIR}")
    endif()
endfunction()

function(_AddCompileShaderCustomCommand)
    set(oneValueArgs COMPILER_PATH HLSL_FILE OUTPUT_FILE WORKING_DIR SHADER_STAGE OUTPUT_FORMAT)
    set(multiValueArgs COMPILER_FLAGS)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    string(TOUPPER ${ARG_SHADER_STAGE} ARG_SHADER_STAGE)

    _CompileShaderMakeOutputDir(${ARG_OUTPUT_FILE})
    add_custom_command(
        OUTPUT ${ARG_OUTPUT_FILE}
        WORKING_DIRECTORY ${ARG_WORKING_DIR}
        COMMENT "------ Compiling ${ARG_SHADER_STAGE} Shader [${ARG_OUTPUT_FORMAT}] ------"
        MAIN_DEPENDENCY ${ARG_HLSL_FILE}
        COMMAND ${CMAKE_COMMAND} -E echo "[${ARG_OUTPUT_FORMAT}] Compiling ${ARG_SHADER_STAGE} ${ARG_HLSL_FILE} to ${ARG_OUTPUT_FILE}"
        COMMAND ${ARG_COMPILER_PATH} ${ARG_COMPILER_FLAGS} -Fo ${ARG_OUTPUT_FILE} ${ARG_HLSL_FILE}
    )
endfunction()

function(_CompileDXILToSPV)
    set(oneValueArgs DXIL_FILE OUTPUT_FILE WORKING_DIR)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "")
    
    _CompileShaderMakeOutputDir(${ARG_OUTPUT_FILE})
    add_custom_command(
        OUTPUT ${ARG_OUTPUT_FILE}
        WORKING_DIRECTORY ${ARG_WORKING_DIR}
        COMMENT "------ Compiling Shader [DXIL-SPV] ------"
        MAIN_DEPENDENCY ${ARG_DXIL_FILE}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXIL-SPV] Compiling ${ARG_DXIL_FILE} to ${ARG_OUTPUT_FILE}"
        COMMAND ${DXIL_SPIRV_PATH} ${ARG_DXIL_FILE} ${DXIL_SPIRV_HLSL_BINDING_SEMANTICS_FLAG} --output ${ARG_OUTPUT_FILE}
    )
endfunction()

function(_CompileToDXBC)
    set(oneValueArgs HLSL_FILE OUTPUT_FILE WORKING_DIR SHADER_STAGE SHADER_MODEL)
    set(multiValueArgs COMPILER_FLAGS)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    list(APPEND all_flags "-T" "${ARG_SHADER_STAGE}_${ARG_SHADER_MODEL}" "-E" "${ARG_SHADER_STAGE}main" ${ARG_COMPILER_FLAGS})
    _AddCompileShaderCustomCommand(
        COMPILER_PATH ${FXC_PATH}
        HLSL_FILE ${ARG_HLSL_FILE}
        OUTPUT_FILE ${ARG_OUTPUT_FILE}
        WORKING_DIR ${ARG_WORKING_DIR}
        SHADER_STAGE ${ARG_SHADER_STAGE}
        OUTPUT_FORMAT "DXBC_${ARG_SHADER_MODEL}"
        COMPILER_FLAGS ${all_flags}
    )
endfunction()

function(_CompileToDXIL)
    set(oneValueArgs HLSL_FILE OUTPUT_FILE WORKING_DIR SHADER_STAGE SHADER_MODEL)
    set(multiValueArgs COMPILER_FLAGS)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    list(APPEND all_flags "-T" "${ARG_SHADER_STAGE}_${ARG_SHADER_MODEL}" "-E" "${ARG_SHADER_STAGE}main" ${ARG_COMPILER_FLAGS})
    _AddCompileShaderCustomCommand(
        COMPILER_PATH ${DXC_PATH}
        HLSL_FILE ${ARG_HLSL_FILE}
        OUTPUT_FILE ${ARG_OUTPUT_FILE}
        WORKING_DIR ${ARG_WORKING_DIR}
        SHADER_STAGE ${ARG_SHADER_STAGE}
        OUTPUT_FORMAT "DXIL_${ARG_SHADER_MODEL}"
        COMPILER_FLAGS ${all_flags}
    )
endfunction()

function(_CompileToSPV)
    set(oneValueArgs HLSL_FILE OUTPUT_FILE WORKING_DIR SHADER_STAGE SHADER_MODEL)
    set(multiValueArgs COMPILER_FLAGS)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    list(APPEND all_flags "-spirv" "-fspv-reflect" ${DXC_HLSL_BINDING_SEMANTICS_FLAG} "-T" "${ARG_SHADER_STAGE}_${ARG_SHADER_MODEL}" "-E" "${ARG_SHADER_STAGE}main" ${ARG_COMPILER_FLAGS})
    _AddCompileShaderCustomCommand(
        COMPILER_PATH ${DXC_PATH}
        HLSL_FILE ${ARG_HLSL_FILE}
        OUTPUT_FILE ${ARG_OUTPUT_FILE}
        WORKING_DIR ${ARG_WORKING_DIR}
        SHADER_STAGE ${ARG_SHADER_STAGE}
        OUTPUT_FORMAT "SPV_${ARG_SHADER_MODEL}"
        COMPILER_FLAGS ${all_flags}
    )
endfunction()

function(_CompileShaders)
    set(oneValueArgs OUTPUT_DIR WORKING_DIR)
    set(multiValueArgs HLSL_FILES SHADER_STAGES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")

    list(APPEND compile_targets)   
    if (PPX_VULKAN)
        list(APPEND compile_targets "spv")
    endif()
    if (PPX_D3D11 OR PPX_DXVK)
        list(APPEND compile_targets "dxbc50")
    endif()
    if (PPX_D3D12)
        list(APPEND compile_targets "dxbc51")
    endif()
    if (PPX_D3D12 OR PPX_DXIL_SPV)
        list(APPEND compile_targets "dxil")
    endif()
    if (PPX_DXVK_SPV)
        list(APPEND compile_targets "dxvk_spv")
    endif()
    if (PPX_DXIL_SPV)
        list(APPEND compile_targets "dxil_spv")
    endif()
    
    foreach(shader_stage ${ARG_SHADER_STAGES})
        foreach(compile_target ${compile_targets})
            set(shader_outputs "")
            foreach(HLSL_PATH ${ARG_HLSL_FILES})   
                string(REPLACE ".hlsl" "" base_name ${HLSL_PATH})
                get_filename_component(base_name ${base_name} NAME)

                if (compile_target STREQUAL "dxbc50")
                    set(out_file ${ARG_OUTPUT_DIR}/dxbc50/${base_name}.${shader_stage}.dxbc50)
                    _CompileToDXBC(
                        HLSL_FILE ${HLSL_PATH}
                        OUTPUT_FILE ${out_file}
                        WORKING_DIR ${ARG_WORKING_DIR}
                        SHADER_STAGE ${shader_stage}
                        SHADER_MODEL "5_0"
                        COMPILER_FLAGS ${PPX_FXC_DX11_FLAGS}
                    )         
               elseif (compile_target STREQUAL "dxbc51")
                    set(out_file ${ARG_OUTPUT_DIR}/dxbc51/${base_name}.${shader_stage}.dxbc51)
                    _CompileToDXBC(
                        HLSL_FILE ${HLSL_PATH}
                        OUTPUT_FILE ${out_file}
                        WORKING_DIR ${ARG_WORKING_DIR}
                        SHADER_STAGE ${shader_stage}
                        SHADER_MODEL "5_1"
                        COMPILER_FLAGS ${PPX_FXC_DX12_FLAGS}
                    )     
               elseif (compile_target STREQUAL "dxil")
                    set(out_file ${ARG_OUTPUT_DIR}/dxil/${base_name}.${shader_stage}.dxil)
                    _CompileToDXIL(
                        HLSL_FILE ${HLSL_PATH}
                        OUTPUT_FILE ${out_file}
                        WORKING_DIR ${ARG_WORKING_DIR}
                        SHADER_STAGE ${shader_stage}
                        SHADER_MODEL "6_0"
                        COMPILER_FLAGS ${PPX_DXC_DXIL_FLAGS}
                    )
               elseif (compile_target STREQUAL "dxil_spv")
                    set(dxil_file ${ARG_OUTPUT_DIR}/dxil/${base_name}.${shader_stage}.dxil)
                    set(out_file ${ARG_OUTPUT_DIR}/dxil_spv/${base_name}.${shader_stage}.spv)
                    _CompileDXILToSPV(
                        DXIL_FILE ${dxil_file}
                        OUTPUT_FILE ${out_file}
                        WORKING_DIR ${ARG_WORKING_DIR}
                    )
               elseif (compile_target STREQUAL "spv")
                    set(out_file ${ARG_OUTPUT_DIR}/spv/${base_name}.${shader_stage}.spv)
                    _CompileToSPV(
                        HLSL_FILE ${HLSL_PATH}
                        OUTPUT_FILE ${out_file}
                        WORKING_DIR ${ARG_WORKING_DIR}
                        SHADER_STAGE ${shader_stage}
                        SHADER_MODEL "6_0"
                        COMPILER_FLAGS ${PPX_DXC_VULKAN_FLAGS} 
                    )          
               elseif (compile_target STREQUAL "dxvk_spv")
                    set(out_file ${ARG_OUTPUT_DIR}/dxvk_spv/${base_name}.${shader_stage}.spv)
                    _CompileToSPV(
                        HLSL_FILE ${HLSL_PATH}
                        OUTPUT_FILE ${out_file}
                        WORKING_DIR ${ARG_WORKING_DIR}
                        SHADER_STAGE ${shader_stage}
                        SHADER_MODEL "6_0"
                        COMPILER_FLAGS ${PPX_DXC_DXVK_FLAGS}
                    )
               endif()
                
               list(APPEND shader_outputs ${out_file})
            endforeach() # end foreach(hlsl_file)

            # Create a target for all the shader outputs for the 
            # given (project, shader format, shader stage) combination.
            set(target_name ${PROJECT_NAME}-${compile_target}-${shader_stage})
            add_custom_target(${target_name} DEPENDS  ${shader_outputs})
            set_target_properties(${target_name} PROPERTIES FOLDER "ppx/samples/shaders-targets/${compile_target}")

            # Create a target for all shader outputs.
            if (NOT TARGET all-shaders)
                add_custom_target(all-shaders)
            endif()
            add_dependencies(all-shaders ${target_name})
        endforeach() # end foreach(compile_target)
    endforeach() # end foreach(shader_stage)
endfunction()

# ------------------------------------------------------------------------------
# Public interface.
# ------------------------------------------------------------------------------
function(CompileShaders)
    set(oneValueArgs OUTPUT_DIR WORKING_DIR)
    set(multiValueArgs VS_HLSL_FILES PS_HLSL_FILES CS_HLSL_FILES)
    cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "${oneValueArgs}" "${multiValueArgs}")
    if (ARG_VS_HLSL_FILES)
        _CompileShaders(
            OUTPUT_DIR ${ARG_OUTPUT_DIR}
            WORKING_DIR ${ARG_WORKING_DIR}
            HLSL_FILES ${ARG_VS_HLSL_FILES}
            SHADER_STAGES "vs"
        )
    endif()

    if (ARG_PS_HLSL_FILES)
        _CompileShaders(
            OUTPUT_DIR ${ARG_OUTPUT_DIR}
            WORKING_DIR ${ARG_WORKING_DIR}
            HLSL_FILES ${ARG_PS_HLSL_FILES}
            SHADER_STAGES "ps"
        )
    endif()

    if (ARG_CS_HLSL_FILES)
        _CompileShaders(
            OUTPUT_DIR ${ARG_OUTPUT_DIR}
            WORKING_DIR ${ARG_WORKING_DIR}
            HLSL_FILES ${ARG_CS_HLSL_FILES}
            SHADER_STAGES "cs"
        )
    endif()
endfunction()
