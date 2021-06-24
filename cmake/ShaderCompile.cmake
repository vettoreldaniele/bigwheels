
message("Adding shader compile functions for HLSL shaders")

set(PPX_FXC_DX11_FLAGS   "/DPPX_D3D11=1")
set(PPX_FXC_DX12_FLAGS   "/DPPX_D3D12=1")
set(PPX_DXC_DXIL_FLAGS   "-DPPX_DX12=1")
set(PPX_DXC_VULKAN_FLAGS "-DPPX_VULKAN=1")

# If DXC_PATH isn't passed in look for it
if (NOT DXC_PATH)
    # Look for it in Vulkan SDK
    if (PPX_LINUX)
        set(VK_SDK_DXC "$ENV{VULKAN_SDK}/bin/dxc")
        message("Looking for dxc at ${VK_SDK_DXC}")
        if (EXISTS "${VK_SDK_DXC}")
            message("   dxc found at ${VK_SDK_DXC}")
            set (DXC_PATH "${VK_SDK_DXC}")
        else()
            message("   dxc not found at ${VK_SDK_DXC}")
        endif()
    elseif(PPX_GGP OR PPX_MSW)
        set(VK_SDK_DXC "$ENV{VULKAN_SDK}\\Bin\\dxc.exe")
        message("Looking for dxc.exe at ${VK_SDK_DXC}")
        if (EXISTS "${VK_SDK_DXC}")
            message("   dxc.exe found at ${VK_SDK_DXC}")
            set (DXC_PATH "${VK_SDK_DXC}")
        else()
            message("   dxc.exe not found at ${VK_SDK_DXC}")
        endif()
    endif()
endif()

if (NOT DXC_PATH)
    message(FATAL_ERROR "Could not locate DXC executable - DXC is required")
endif()

# Hacky
if (PPX_DXVK)
    set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.18362.0")
endif()

if ((PPX_MSW OR PPX_DXVK) AND NOT FXC_PATH)
    message("CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is set to: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}.")
    if (NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        message(FATAL_ERROR "Windows SDK not found on this system. Make sure that CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is set.")
    endif()

    set(FXC_PATH "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}\\x64\\fxc.exe")

    if (NOT EXISTS "${FXC_PATH}")
        message(FATAL_ERROR "Could not locate FXC executable at ${FXC_PATH} - FXC is required")
    endif()
endif()

message("DXC_PATH: ${DXC_PATH}")
message("FXC_PATH: ${FXC_PATH}")

function(CompileShaderMakeOutputDir OUTPUT_FILE)
    get_filename_component(PARENT_DIR ${OUTPUT_FILE} DIRECTORY)
    if (NOT EXISTS "${PARENT_DIR}")
        file(MAKE_DIRECTORY "${PARENT_DIR}")
        message("Created shader binary output directory: ${PARENT_DIR}")
    endif()
endfunction()

function(CompileToDXBC50_VS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
      
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling VS Shader [DXBC50] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXBC 5.0
        COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC50] Compiling VS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${FXC_PATH} -T vs_5_0 -E vsmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX11_FLAGS}
    )    
endfunction()

function(CompileToDXBC51_VS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling VS Shader [DXBC51] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXBC 5.1
        COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC51] Compiling VS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${FXC_PATH} -T vs_5_1 -E vsmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX12_FLAGS}
    )    
endfunction()

function(CompileToDXIL_VS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling VS Shader [DXC-DXIL] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXIL
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling VS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -T vs_6_0 -E vsmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_DXC_DXIL_FLAGS}
    )    
endfunction()

function(CompileToSPV_VS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling VS Shader [DXC-SPV] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV] Compiling VS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -spirv -T vs_6_0 -E vsmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_DXC_VULKAN_FLAGS}
    )    
endfunction()

function(CompileToDxvkSPV_VS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling VS Shader [DXC-DXVK-SPV] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXVK-SPV] Compiling VS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -spirv -T vs_6_0 -E vsmain -fvk-use-dx-layout -fvk-use-dx-position-w -fvk-auto-shift-bindings -fvk-b-shift 160 0 -fvk-s-shift 176 0 -fvk-t-shift 192 0 -fvk-u-shift 960 0 -fspv-reflect -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX11_FLAGS}
    )
endfunction()

function(CompileToDXBC50_PS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling PS Shader ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXBC 5.0
        COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling PS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${FXC_PATH} -T ps_5_0 -E psmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX11_FLAGS}
    )    
endfunction()

function(CompileToDXBC51_PS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling PS Shader ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXBC 5.1
        COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling PS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${FXC_PATH} -T ps_5_1 -E psmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX12_FLAGS}
    )    
endfunction()

function(CompileToDXIL_PS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling PS Shader [DXC-DXIL] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXIL
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling PS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -T ps_6_0 -E psmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_DXC_DXIL_FLAGS}
    )    
endfunction()

function(CompileToSPV_PS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling PS Shader [DXC-SPV] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV] Compiling PS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -spirv -T ps_6_0 -E psmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_DXC_VULKAN_FLAGS}
    )    
endfunction()

function(CompileToDxvkSPV_PS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling PS Shader [DXC-DXVK-SPV] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXVK-SPV] Compiling PS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -spirv -T ps_6_0 -E psmain -fvk-use-dx-layout -fvk-use-dx-position-w -fvk-auto-shift-bindings -fvk-b-shift 0 0 -fvk-s-shift 16 0 -fvk-t-shift 32 0 -fvk-u-shift 960 0 -fspv-reflect -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX11_FLAGS}
    )
endfunction()

function(CompileToDXBC50_CS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling CS Shader ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXBC 5.0
        COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling CS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${FXC_PATH} -T cs_5_0 -E csmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX11_FLAGS}
    )    
endfunction()

function(CompileToDXBC51_CS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling CS Shader ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXBC 5.1
        COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling CS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${FXC_PATH} -T cs_5_1 -E csmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX12_FLAGS}
    )    
endfunction()

function(CompileToDXIL_CS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling CS Shader [DXC-DXIL] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to DXIL
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling CS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -T cs_6_0 -E csmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_DXC_DXIL_FLAGS}
    )    
endfunction()

function(CompileToSPV_CS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})
   
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling CS Shader [DXC-SPV] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV] Compiling CS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -spirv -T cs_6_0 -E csmain -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_DXC_VULKAN_FLAGS}
    )    
endfunction()

function(CompileToDxvkSPV_CS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_FILE ${ARGV1})
    set(WORKING_DIR ${ARGV2})
    CompileShaderMakeOutputDir(${OUTPUT_FILE})

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        WORKING_DIRECTORY ${WORKING_DIR}
        COMMENT "------ Compiling CS Shader [DXC-DXVK-SPV] ------"
        MAIN_DEPENDENCY ${HLSL_PATH}
        # Compile to SPIR-V
        COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXVK-SPV] Compiling CS ${HLSL_PATH} to ${OUTPUT_FILE}"
        COMMAND ${DXC_PATH} -spirv -T cs_6_0 -E csmain -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -fvk-auto-shift-bindings -fvk-b-shift 800 0 -fvk-s-shift 816 0 -fvk-t-shift 832 0 -fvk-u-shift 1088 0 -fspv-reflect -Fo ${OUTPUT_FILE} ${HLSL_PATH} ${PPX_FXC_DX11_FLAGS}
    )
endfunction()

# @fn compile_vs_ps - Compiles a list of HLSL files with VS and PS to DXBC, DXIL, and SPV
#   
# @param ARGV0 HLSL_PATH
# @param ARGV1 OUTPUT_DIR
# @parma ARGV2 WORKING_DIR
#
function(CompileShadersVSPS)
    set(HLSL_FILES  ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})
          
    set(BUILD_DXBC50     FALSE)
    set(BUILD_DXBC51     FALSE)
    set(BUILD_DXIL       FALSE)
    set(BUILD_SPIRV      FALSE)
    set(BUILD_DXVK_SPIRV FALSE)
    
    if (PPX_D3D11 OR PPX_DXVK)
        set(BUILD_DXBC50 TRUE)
    endif()
    if (PPX_D3D12)
        set(BUILD_DXBC51 TRUE)
        set(BUILD_DXIL   TRUE)
    endif()
    if (PPX_VULKAN)
        set(BUILD_SPIRV TRUE)
    endif()
    if (PPX_DXVK_SPV)
        set(BUILD_DXVK_SPIRV TRUE)
    endif()

    list(APPEND outputs_dxbc50)
    list(APPEND outputs_dxbc51)
    list(APPEND outputs_dxil)
    list(APPEND outputs_spv)
    list(APPEND outputs_dxvk_spv)
    foreach(HLSL_PATH ${HLSL_FILES})   
        # SPV targets
        string(REPLACE "hlsl" "vs.spv" vs_file ${HLSL_PATH})
        string(REPLACE "hlsl" "ps.spv" ps_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        get_filename_component(ps_file ${ps_file} NAME)
        set(spv_vs_file ${OUTPUT_DIR}/spv/${vs_file})
        set(spv_ps_file ${OUTPUT_DIR}/spv/${ps_file})

        # DXVK SPV targets
        set(dxvk_spv_vs_file ${OUTPUT_DIR}/dxvk_spv/${vs_file})
        set(dxvk_spv_ps_file ${OUTPUT_DIR}/dxvk_spv/${ps_file})

        # DXIL targets
        string(REPLACE "hlsl" "vs.dxil" vs_file ${HLSL_PATH})
        string(REPLACE "hlsl" "ps.dxil" ps_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        get_filename_component(ps_file ${ps_file} NAME)
        set(dxil_vs_file ${OUTPUT_DIR}/dxil/${vs_file})
        set(dxil_ps_file ${OUTPUT_DIR}/dxil/${ps_file})    
        
        # DXBC 5.0 targets
        string(REPLACE "hlsl" "vs.dxbc50" vs_file ${HLSL_PATH})
        string(REPLACE "hlsl" "ps.dxbc50" ps_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        get_filename_component(ps_file ${ps_file} NAME)
        set(dxbc50_vs_file ${OUTPUT_DIR}/dxbc50/${vs_file})
        set(dxbc50_ps_file ${OUTPUT_DIR}/dxbc50/${ps_file})    
        
        # DXBC 5.1 targets
        string(REPLACE "hlsl" "vs.dxbc51" vs_file ${HLSL_PATH})
        string(REPLACE "hlsl" "ps.dxbc51" ps_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        get_filename_component(ps_file ${ps_file} NAME)
        set(dxbc51_vs_file ${OUTPUT_DIR}/dxbc51/${vs_file})
        set(dxbc51_ps_file ${OUTPUT_DIR}/dxbc51/${ps_file})
        
        if (BUILD_DXBC50)
            CompileToDXBC50_VS(${HLSL_PATH} "${dxbc50_vs_file}" ${WORKING_DIR})
            CompileToDXBC50_PS(${HLSL_PATH} "${dxbc50_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc50 ${dxbc50_vs_file} ${dxbc50_ps_file})            
        endif()
        
        if (BUILD_DXBC51)
            CompileToDXBC51_VS(${HLSL_PATH} "${dxbc51_vs_file}" ${WORKING_DIR})
            CompileToDXBC51_PS(${HLSL_PATH} "${dxbc51_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc51 ${dxbc51_vs_file} ${dxbc51_ps_file})          
        endif()
        
        if (BUILD_DXIL)
            CompileToDXIL_VS(${HLSL_PATH} "${dxil_vs_file}" ${WORKING_DIR})
            CompileToDXIL_PS(${HLSL_PATH} "${dxil_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxil ${dxil_vs_file} ${dxil_ps_file})          
        endif()
        
        if (BUILD_SPIRV)
            CompileToSPV_VS(${HLSL_PATH} "${spv_vs_file}" ${WORKING_DIR})
            CompileToSPV_PS(${HLSL_PATH} "${spv_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_spv ${spv_vs_file} ${spv_ps_file})          
        endif()

        if (BUILD_DXVK_SPIRV)
            CompileToDxvkSPV_VS(${HLSL_PATH} "${dxvk_spv_vs_file}" ${WORKING_DIR})
            CompileToDxvkSPV_PS(${HLSL_PATH} "${dxvk_spv_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxvk_spv ${dxvk_spv_vs_file} ${dxvk_spv_ps_file})
        endif()
    endforeach()   
    
    if (BUILD_DXBC50)
        set(target_name ${PROJECT_NAME}-dxbc50-vsps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc50})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc50")
    endif()
    
    if (BUILD_DXBC51)
        set(target_name ${PROJECT_NAME}-dxbc51-vsps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc51})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc51")
    endif()
    
    if (BUILD_DXIL)
        set(target_name ${PROJECT_NAME}-dxil-vsps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxil})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxil")
    endif()
    
    if (BUILD_SPIRV)
        set(target_name ${PROJECT_NAME}-spv-vsps)
        add_custom_target(${target_name} DEPENDS ${outputs_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/spv")
    endif()

    if (BUILD_DXVK_SPIRV)
        set(target_name ${PROJECT_NAME}-dxvk-spv-vsps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxvk_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxvk_spv")
    endif()
endfunction()

function(CompileShadersVS)
    set(HLSL_FILES  ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})
          
    set(BUILD_DXBC50     FALSE)
    set(BUILD_DXBC51     FALSE)
    set(BUILD_DXIL       FALSE)
    set(BUILD_SPIRV      FALSE)
    set(BUILD_DXVK_SPIRV FALSE)
    
    if (PPX_D3D11 OR PPX_DXVK)
        set(BUILD_DXBC50 TRUE)
    endif()
    if (PPX_D3D12)
        set(BUILD_DXBC51 TRUE)
        set(BUILD_DXIL   TRUE)
    endif()
    if (PPX_VULKAN)
        set(BUILD_SPIRV TRUE)
    endif()
    if (PPX_DXVK_SPV)
        set(BUILD_DXVK_SPIRV TRUE)
    endif()

    list(APPEND outputs_dxbc50)
    list(APPEND outputs_dxbc51)
    list(APPEND outputs_dxil)
    list(APPEND outputs_spv)
    list(APPEND outputs_dxvk_spv)
    foreach(HLSL_PATH ${HLSL_FILES})   
        # SPV targets
        string(REPLACE "hlsl" "vs.spv" vs_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        set(spv_vs_file ${OUTPUT_DIR}/spv/${vs_file})

        # DXVK SPV targets
        set(dxvk_spv_vs_file ${OUTPUT_DIR}/dxvk_spv/${vs_file})

        # DXIL targets
        string(REPLACE "hlsl" "vs.dxil" vs_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        set(dxil_vs_file ${OUTPUT_DIR}/dxil/${vs_file})
        
        # DXBC 5.0 targets
        string(REPLACE "hlsl" "vs.dxbc50" vs_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        set(dxbc50_vs_file ${OUTPUT_DIR}/dxbc50/${vs_file})
        
        # DXBC 5.1 targets
        string(REPLACE "hlsl" "vs.dxbc51" vs_file ${HLSL_PATH})
        get_filename_component(vs_file ${vs_file} NAME)
        set(dxbc51_vs_file ${OUTPUT_DIR}/dxbc51/${vs_file})
        
        if (BUILD_DXBC50)
            CompileToDXBC50_VS(${HLSL_PATH} "${dxbc50_vs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc50 ${dxbc50_vs_file} ${dxbc50_ps_file})            
        endif()
        
        if (BUILD_DXBC51)
            CompileToDXBC51_VS(${HLSL_PATH} "${dxbc51_vs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc51 ${dxbc51_vs_file} ${dxbc51_ps_file})          
        endif()
        
        if (BUILD_DXIL)
            CompileToDXIL_VS(${HLSL_PATH} "${dxil_vs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxil ${dxil_vs_file} ${dxil_ps_file})          
        endif()
        
        if (BUILD_SPIRV)
            CompileToSPV_VS(${HLSL_PATH} "${spv_vs_file}" ${WORKING_DIR})
            list(APPEND outputs_spv ${spv_vs_file} ${spv_ps_file})          
        endif()

        if (BUILD_DXVK_SPIRV)
            CompileToDxvkSPV_VS(${HLSL_PATH} "${dxvk_spv_vs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxvk_spv ${dxvk_spv_vs_file} ${dxvk_spv_ps_file})
        endif()
    endforeach()   
    
    if (BUILD_DXBC50)
        set(target_name ${PROJECT_NAME}-dxbc50-vs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc50})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc50")
    endif()
    
    if (BUILD_DXBC51)
        set(target_name ${PROJECT_NAME}-dxbc51-vs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc51})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc51")
    endif()
    
    if (BUILD_DXIL)
        set(target_name ${PROJECT_NAME}-dxil-vs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxil})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxil")
    endif()
    
    if (BUILD_SPIRV)
        set(target_name ${PROJECT_NAME}-spv-vs)
        add_custom_target(${target_name} DEPENDS ${outputs_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/spv")
    endif()

    if (BUILD_DXVK_SPIRV)
        set(target_name ${PROJECT_NAME}-dxvk-spv-vs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxvk_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxvk_spv")
    endif()
endfunction()

function(CompileShadersPS)
    set(HLSL_FILES  ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})
          
    set(BUILD_DXBC50     FALSE)
    set(BUILD_DXBC51     FALSE)
    set(BUILD_DXIL       FALSE)
    set(BUILD_SPIRV      FALSE)
    set(BUILD_DXVK_SPIRV FALSE)
    
    if (PPX_D3D11 OR PPX_DXVK)
        set(BUILD_DXBC50 TRUE)
    endif()
    if (PPX_D3D12)
        set(BUILD_DXBC51 TRUE)
        set(BUILD_DXIL   TRUE)
    endif()
    if (PPX_VULKAN)
        set(BUILD_SPIRV TRUE)
    endif()
    if (PPX_DXVK_SPV)
        set(BUILD_DXVK_SPIRV TRUE)
    endif()

    list(APPEND outputs_dxbc50)
    list(APPEND outputs_dxbc51)
    list(APPEND outputs_dxil)
    list(APPEND outputs_spv)
    list(APPEND outputs_dxvk_spv)
    foreach(HLSL_PATH ${HLSL_FILES})   
        # SPV targets
        string(REPLACE "hlsl" "ps.spv" ps_file ${HLSL_PATH})
        get_filename_component(ps_file ${ps_file} NAME)
        set(spv_ps_file ${OUTPUT_DIR}/spv/${ps_file})

        # DXVK SPV targets
        set(dxvk_spv_ps_file ${OUTPUT_DIR}/dxvk_spv/${ps_file})

        # DXIL targets
        string(REPLACE "hlsl" "ps.dxil" ps_file ${HLSL_PATH})
        get_filename_component(ps_file ${ps_file} NAME)
        set(dxil_ps_file ${OUTPUT_DIR}/dxil/${ps_file})
        
        # DXBC 5.0 targets
        string(REPLACE "hlsl" "ps.dxbc50" ps_file ${HLSL_PATH})
        get_filename_component(ps_file ${ps_file} NAME)
        set(dxbc50_ps_file ${OUTPUT_DIR}/dxbc50/${ps_file})
        
        # DXBC 5.1 targets
        string(REPLACE "hlsl" "ps.dxbc51" ps_file ${HLSL_PATH})
        get_filename_component(ps_file ${ps_file} NAME)
        set(dxbc51_ps_file ${OUTPUT_DIR}/dxbc51/${ps_file})
        
        if (BUILD_DXBC50)
            CompileToDXBC50_PS(${HLSL_PATH} "${dxbc50_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc50 ${dxbc50_ps_file} ${dxbc50_ps_file})            
        endif()
        
        if (BUILD_DXBC51)
            CompileToDXBC51_PS(${HLSL_PATH} "${dxbc51_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc51 ${dxbc51_ps_file} ${dxbc51_ps_file})          
        endif()
        
        if (BUILD_DXIL)
            CompileToDXIL_PS(${HLSL_PATH} "${dxil_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxil ${dxil_ps_file} ${dxil_ps_file})          
        endif()
        
        if (BUILD_SPIRV)
            CompileToSPV_PS(${HLSL_PATH} "${spv_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_spv ${spv_ps_file} ${spv_ps_file})          
        endif()

        if (BUILD_DXVK_SPIRV)
            CompileToDxvkSPV_PS(${HLSL_PATH} "${dxvk_spv_ps_file}" ${WORKING_DIR})
            list(APPEND outputs_dxvk_spv ${dxvk_spv_ps_file} ${dxvk_spv_ps_file})
        endif()
    endforeach()   
    
    if (BUILD_DXBC50)
        set(target_name ${PROJECT_NAME}-dxbc50-ps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc50})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc50")
    endif()
    
    if (BUILD_DXBC51)
        set(target_name ${PROJECT_NAME}-dxbc51-ps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc51})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc51")
    endif()
    
    if (BUILD_DXIL)
        set(target_name ${PROJECT_NAME}-dxil-ps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxil})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxil")
    endif()
    
    if (BUILD_SPIRV)
        set(target_name ${PROJECT_NAME}-spv-ps)
        add_custom_target(${target_name} DEPENDS ${outputs_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/spv")
    endif()

    if (BUILD_DXVK_SPIRV)
        set(target_name ${PROJECT_NAME}-dxvk-spv-ps)
        add_custom_target(${target_name} DEPENDS ${outputs_dxvk_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxvk_spv")
    endif()
endfunction()

function(CompileShadersCS)
    set(HLSL_FILES  ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})
          
    set(BUILD_DXBC50     FALSE)
    set(BUILD_DXBC51     FALSE)
    set(BUILD_DXIL       FALSE)
    set(BUILD_SPIRV      FALSE)
    set(BUILD_DXVK_SPIRV FALSE)
    
    if (PPX_D3D11 OR PPX_DXVK)
        set(BUILD_DXBC50 TRUE)
    endif()
    if (PPX_D3D12)
        set(BUILD_DXBC51 TRUE)
        set(BUILD_DXIL   TRUE)
    endif()
    if (PPX_VULKAN)
        set(BUILD_SPIRV TRUE)
    endif()
    if (PPX_DXVK_SPV)
        set(BUILD_DXVK_SPIRV TRUE)
    endif()

    list(APPEND outputs_dxbc50)
    list(APPEND outputs_dxbc51)
    list(APPEND outputs_dxil)
    list(APPEND outputs_spv)
    foreach(HLSL_PATH ${HLSL_FILES})   
        # SPV targets
        string(REPLACE "hlsl" "cs.spv" cs_file ${HLSL_PATH})
        get_filename_component(cs_file ${cs_file} NAME)
        set(spv_cs_file ${OUTPUT_DIR}/spv/${cs_file})

        # DXVK SPV targets
        set(dxvk_spv_cs_file ${OUTPUT_DIR}/dxvk_spv/${cs_file})

        # DXIL targets
        string(REPLACE "hlsl" "cs.dxil" cs_file ${HLSL_PATH})
        get_filename_component(cs_file ${cs_file} NAME)
        set(dxil_cs_file ${OUTPUT_DIR}/dxil/${cs_file})
        
        # DXBC 5.0 targets
        string(REPLACE "hlsl" "cs.dxbc50" cs_file ${HLSL_PATH})
        get_filename_component(cs_file ${cs_file} NAME)
        set(dxbc50_cs_file ${OUTPUT_DIR}/dxbc50/${cs_file})
        
        # DXBC 5.1 targets
        string(REPLACE "hlsl" "cs.dxbc51" cs_file ${HLSL_PATH})
        get_filename_component(cs_file ${cs_file} NAME)
        set(dxbc51_cs_file ${OUTPUT_DIR}/dxbc51/${cs_file})
        
        if (BUILD_DXBC50)
            CompileToDXBC50_CS(${HLSL_PATH} "${dxbc50_cs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc50 ${dxbc50_cs_file} ${dxbc50_ps_file})            
        endif()
        
        if (BUILD_DXBC51)
            CompileToDXBC51_CS(${HLSL_PATH} "${dxbc51_cs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxbc51 ${dxbc51_cs_file} ${dxbc51_ps_file})          
        endif()
        
        if (BUILD_DXIL)
            CompileToDXIL_CS(${HLSL_PATH} "${dxil_cs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxil ${dxil_cs_file} ${dxil_ps_file})          
        endif()
        
        if (BUILD_SPIRV)
            CompileToSPV_CS(${HLSL_PATH} "${spv_cs_file}" ${WORKING_DIR})
            list(APPEND outputs_spv ${spv_cs_file} ${spv_ps_file})          
        endif()

        if (BUILD_DXVK_SPIRV)
            CompileToDxvkSPV_CS(${HLSL_PATH} "${dxvk_spv_cs_file}" ${WORKING_DIR})
            list(APPEND outputs_dxvk_spv ${dxvk_spv_cs_file} ${dxvk_spv_cs_file})
        endif()
    endforeach()   
    
    if (BUILD_DXBC50)
        set(target_name ${PROJECT_NAME}-dxbc50-cs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc50})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc50")
    endif()
    
    if (BUILD_DXBC51)
        set(target_name ${PROJECT_NAME}-dxbc51-cs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxbc51})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxbc51")
    endif()
    
    if (BUILD_DXIL)
        set(target_name ${PROJECT_NAME}-dxil-cs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxil})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxil")
    endif()
    
    if (BUILD_SPIRV)
        set(target_name ${PROJECT_NAME}-spv-cs)
        add_custom_target(${target_name} DEPENDS ${outputs_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/spv")
    endif()

    if (BUILD_DXVK_SPIRV)
        set(target_name ${PROJECT_NAME}-dxvk-spv-cs)
        add_custom_target(${target_name} DEPENDS ${outputs_dxvk_spv})
        set_target_properties(${target_name} PROPERTIES FOLDER "ppx/projects/0_shaders-targets/dxvk_spv")
    endif()
endfunction()

