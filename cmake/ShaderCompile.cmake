
message("Adding shader compile functions for HLSL shaders")

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

if (PPX_MSW AND NOT FXC_PATH)
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

# @fn compile_vs_ps - Compiles an HLSL file with VS and PS to DXBC, DXIL, and SPV
#   
# This function is a bit clumsy because I couldn't find a way to get the shaders
# to conditionally build based on the current program target. When a shader has 
# source changes, it will get built for all bytecode targets but only once.
#
# @param ARGV0 HLSL_PATH
# @param ARGV1 OUTPUT_DIR
# @parma ARGV2 WORKING_DIR
#
function(CompileShaderVSPS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})

    # SPV targets
    string(REPLACE "hlsl" "vs.spv" vs_file ${HLSL_PATH})
    string(REPLACE "hlsl" "ps.spv" ps_file ${HLSL_PATH})
    get_filename_component(vs_file ${vs_file} NAME)
    get_filename_component(ps_file ${ps_file} NAME)
    set(spv_vs_file ${OUTPUT_DIR}/spv/${vs_file})
    set(spv_ps_file ${OUTPUT_DIR}/spv/${ps_file})
    
    # DXIL targets
    string(REPLACE "hlsl" "vs.dxil" vs_file ${HLSL_PATH})
    string(REPLACE "hlsl" "ps.dxil" ps_file ${HLSL_PATH})
    get_filename_component(vs_file ${vs_file} NAME)
    get_filename_component(ps_file ${ps_file} NAME)
    set(dxil_vs_file ${OUTPUT_DIR}/dxil/${vs_file})
    set(dxil_ps_file ${OUTPUT_DIR}/dxil/${ps_file})    
    
    # DXBC targets
    string(REPLACE "hlsl" "vs.dxbc" vs_file ${HLSL_PATH})
    string(REPLACE "hlsl" "ps.dxbc" ps_file ${HLSL_PATH})
    get_filename_component(vs_file ${vs_file} NAME)
    get_filename_component(ps_file ${ps_file} NAME)
    set(dxbc_vs_file ${OUTPUT_DIR}/dxbc/${vs_file})
    set(dxbc_ps_file ${OUTPUT_DIR}/dxbc/${ps_file})    

    if (PPX_VULKAN AND PPX_D3D12)
        CompileShaderMakeOutputDir(${spv_vs_file})
        CompileShaderMakeOutputDir(${spv_ps_file})
        CompileShaderMakeOutputDir(${dxil_vs_file}) 
        CompileShaderMakeOutputDir(${dxil_ps_file})
        CompileShaderMakeOutputDir(${dxbc_vs_file})
        CompileShaderMakeOutputDir(${dxbc_ps_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling VS ${HLSL_PATH} to ${spv_vs_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T vs_6_0 -E vsmain -Fo ${spv_vs_file} ${HLSL_PATH}
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling PS ${HLSL_PATH} to ${spv_ps_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T ps_6_0 -E psmain -Fo ${spv_ps_file} ${HLSL_PATH}
            
            # Compile to DXIL
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling VS ${HLSL_PATH} to ${dxil_vs_file}"
            COMMAND ${DXC_PATH} -T vs_6_0 -E vsmain -Fo ${dxil_vs_file} ${HLSL_PATH}
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling PS ${HLSL_PATH} to ${dxil_ps_file}"       
            COMMAND ${DXC_PATH} -T ps_6_0 -E psmain -Fo ${dxil_ps_file} ${HLSL_PATH}
            
            # Compile to DXBC
            COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling VS ${HLSL_PATH} to ${dxbc_vs_file}"
            COMMAND ${FXC_PATH} -T vs_5_1 -E vsmain -Fo ${dxbc_vs_file} ${HLSL_PATH}
            COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling PS ${HLSL_PATH} to ${dxbc_ps_file}"
            COMMAND ${FXC_PATH} -T ps_5_1 -E psmain -Fo ${dxbc_ps_file} ${HLSL_PATH}
            
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_vs_file}
                   ${spv_ps_file} 
                   ${dxil_vs_file} 
                   ${dxil_ps_file} 
                   ${dxbc_vs_file}
                   ${dxbc_ps_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling VSPS Shader(s) ------"        
        )
    else()
        CompileShaderMakeOutputDir(${spv_vs_file})
        CompileShaderMakeOutputDir(${spv_ps_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling VS ${HLSL_PATH} to ${spv_vs_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T vs_6_0 -E vsmain -Fo ${spv_vs_file} ${HLSL_PATH}
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling PS ${HLSL_PATH} to ${spv_ps_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T ps_6_0 -E psmain -Fo ${spv_ps_file} ${HLSL_PATH}
                       
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_vs_file}
                   ${spv_ps_file} 
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling VSPS Shader(s) ------"        
        )
    endif()
endfunction()

function(CompileShaderCS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})

    # SPV targets
    string(REPLACE "hlsl" "cs.spv" cs_file ${HLSL_PATH})
    get_filename_component(cs_file ${cs_file} NAME)
    set(spv_cs_file ${OUTPUT_DIR}/spv/${cs_file})
    
    # DXIL targets
    string(REPLACE "hlsl" "cs.dxil" cs_file ${HLSL_PATH})
    get_filename_component(cs_file ${cs_file} NAME)
    set(dxil_cs_file ${OUTPUT_DIR}/dxil/${cs_file})
    
    # DXBC targets
    string(REPLACE "hlsl" "cs.dxbc" cs_file ${HLSL_PATH})
    get_filename_component(cs_file ${cs_file} NAME)
    set(dxbc_cs_file ${OUTPUT_DIR}/dxbc/${cs_file})

    if (PPX_VULKAN AND PPX_D3D12)
        CompileShaderMakeOutputDir(${spv_cs_file})
        CompileShaderMakeOutputDir(${dxil_cs_file}) 
        CompileShaderMakeOutputDir(${dxbc_cs_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling VS ${HLSL_PATH} to ${spv_cs_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T cs_6_0 -E csmain -Fo ${spv_cs_file} ${HLSL_PATH}
            
            # Compile to DXIL
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling VS ${HLSL_PATH} to ${dxil_cs_file}"
            COMMAND ${DXC_PATH} -T cs_6_0 -E csmain -Fo ${dxil_cs_file} ${HLSL_PATH}
            
            # Compile to DXBC
            COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling VS ${HLSL_PATH} to ${dxbc_cs_file}"
            COMMAND ${FXC_PATH} -T cs_5_1 -E csmain -Fo ${dxbc_cs_file} ${HLSL_PATH}
            
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_cs_file}
                   ${dxil_cs_file} 
                   ${dxbc_cs_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling CS Shader(s) ------"        
        )
    else()
        CompileShaderMakeOutputDir(${spv_cs_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling VS ${HLSL_PATH} to ${spv_cs_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T cs_6_0 -E csmain -Fo ${spv_cs_file} ${HLSL_PATH}
            
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_cs_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling CS Shader(s) ------"        
        )
    endif()
endfunction()

function(CompileShaderVS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})

    # SPV targets
    string(REPLACE "hlsl" "vs.spv" vs_file ${HLSL_PATH})
    get_filename_component(vs_file ${vs_file} NAME)
    set(spv_vs_file ${OUTPUT_DIR}/spv/${vs_file})
    
    # DXIL targets
    string(REPLACE "hlsl" "vs.dxil" vs_file ${HLSL_PATH})
    get_filename_component(vs_file ${vs_file} NAME)
    set(dxil_vs_file ${OUTPUT_DIR}/dxil/${vs_file})
    
    # DXBC targets
    string(REPLACE "hlsl" "vs.dxbc" vs_file ${HLSL_PATH})
    get_filename_component(vs_file ${vs_file} NAME)
    set(dxbc_vs_file ${OUTPUT_DIR}/dxbc/${vs_file})

    if (PPX_VULKAN AND PPX_D3D12)
        CompileShaderMakeOutputDir(${spv_vs_file})
        CompileShaderMakeOutputDir(${dxil_vs_file}) 
        CompileShaderMakeOutputDir(${dxbc_vs_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling VS ${HLSL_PATH} to ${spv_vs_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T vs_6_0 -E vsmain -Fo ${spv_vs_file} ${HLSL_PATH}
            
            # Compile to DXIL
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling VS ${HLSL_PATH} to ${dxil_vs_file}"
            COMMAND ${DXC_PATH} -T vs_6_0 -E vsmain -Fo ${dxil_vs_file} ${HLSL_PATH}
            
            # Compile to DXBC
            COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling VS ${HLSL_PATH} to ${dxbc_vs_file}"
            COMMAND ${FXC_PATH} -T vs_5_1 -E vsmain -Fo ${dxbc_vs_file} ${HLSL_PATH}
            
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_vs_file}
                   ${dxil_vs_file} 
                   ${dxbc_vs_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling VS (depth only) Shader(s) ------"        
        )        
    else()
        CompileShaderMakeOutputDir(${spv_vs_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling VS ${HLSL_PATH} to ${spv_vs_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T vs_6_0 -E vsmain -Fo ${spv_vs_file} ${HLSL_PATH}
           
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_vs_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling VS (depth only) Shader(s) ------"
        )
    endif()
endfunction()

function(CompileShaderPS)
    set(HLSL_PATH   ${ARGV0})
    set(OUTPUT_DIR  ${ARGV1})
    set(WORKING_DIR ${ARGV2})

    # SPV targets
    string(REPLACE "hlsl" "ps.spv" ps_file ${HLSL_PATH})
    get_filename_component(ps_file ${ps_file} NAME)
    set(spv_ps_file ${OUTPUT_DIR}/spv/${ps_file})
    
    # DXIL targets
    string(REPLACE "hlsl" "ps.dxil" ps_file ${HLSL_PATH})
    get_filename_component(ps_file ${ps_file} NAME)
    set(dxil_ps_file ${OUTPUT_DIR}/dxil/${ps_file})
    
    # DXBC targets
    string(REPLACE "hlsl" "ps.dxbc" ps_file ${HLSL_PATH})
    get_filename_component(ps_file ${ps_file} NAME)
    set(dxbc_ps_file ${OUTPUT_DIR}/dxbc/${ps_file})

    if (PPX_VULKAN AND PPX_D3D12)
        CompileShaderMakeOutputDir(${spv_ps_file})
        CompileShaderMakeOutputDir(${dxil_ps_file}) 
        CompileShaderMakeOutputDir(${dxbc_ps_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling PS ${HLSL_PATH} to ${spv_ps_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T ps_6_0 -E psmain -Fo ${spv_ps_file} ${HLSL_PATH}
            
            # Compile to DXIL
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-DXIL] Compiling PS ${HLSL_PATH} to ${dxil_ps_file}"
            COMMAND ${DXC_PATH} -T ps_6_0 -E psmain -Fo ${dxil_ps_file} ${HLSL_PATH}
            
            # Compile to DXBC
            COMMAND ${CMAKE_COMMAND} -E echo "[FXC-DXBC] Compiling PS ${HLSL_PATH} to ${dxbc_ps_file}"
            COMMAND ${FXC_PATH} -T ps_5_1 -E psmain -Fo ${dxbc_ps_file} ${HLSL_PATH}
            
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_ps_file}
                   ${dxil_ps_file} 
                   ${dxbc_ps_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling PS (depth only) Shader(s) ------"        
        )        
    else()
        CompileShaderMakeOutputDir(${spv_ps_file})

        add_custom_command(
            # Compile to SPV
            COMMAND ${CMAKE_COMMAND} -E echo "[DXC-SPV ] Compiling PS ${HLSL_PATH} to ${spv_ps_file}"
            COMMAND ${DXC_PATH} -spirv -fvk-use-dx-layout -T ps_6_0 -E psmain -Fo ${spv_ps_file} ${HLSL_PATH}
           
            MAIN_DEPENDENCY ${HLSL_PATH}
            OUTPUT ${spv_ps_file}
            WORKING_DIRECTORY ${WORKING_DIR}
            COMMENT "------ Compiling PS (pixel shader only) Shader(s) ------"
        )
    endif()
endfunction()
