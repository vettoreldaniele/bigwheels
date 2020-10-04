#include "ppx/grfx/grfx_format.h"

namespace ppx {
namespace grfx {

uint32_t FormatSize(grfx::Format format)
{
    // clang-format off
    switch (format) {
        default: break;

        // 8-bit signed normalized
        case grfx::FORMAT_R8_SNORM           : return 1; break;
        case grfx::FORMAT_R8G8_SNORM         : return 2; break;
        case grfx::FORMAT_R8G8B8_SNORM       : return 3; break;
        case grfx::FORMAT_R8G8B8A8_SNORM     : return 4; break;
        case grfx::FORMAT_B8G8R8_SNORM       : return 3; break;
        case grfx::FORMAT_B8G8R8A8_SNORM     : return 4; break;
                                             
        // 8-bit unsigned normalized         
        case grfx::FORMAT_R8_UNORM           : return 1; break;
        case grfx::FORMAT_R8G8_UNORM         : return 2; break;
        case grfx::FORMAT_R8G8B8_UNORM       : return 3; break;
        case grfx::FORMAT_R8G8B8A8_UNORM     : return 4; break;
        case grfx::FORMAT_B8G8R8_UNORM       : return 3; break;
        case grfx::FORMAT_B8G8R8A8_UNORM     : return 4; break;
                                             
        // 8-bit signed integer              
        case grfx::FORMAT_R8_SINT            : return 1; break;
        case grfx::FORMAT_R8G8_SINT          : return 2; break;
        case grfx::FORMAT_R8G8B8_SINT        : return 3; break;
        case grfx::FORMAT_R8G8B8A8_SINT      : return 4; break;
        case grfx::FORMAT_B8G8R8_SINT        : return 3; break;
        case grfx::FORMAT_B8G8R8A8_SINT      : return 4; break;
                                             
        // 8-bit unsigned integer            
        case grfx::FORMAT_R8_UINT            : return 1; break;
        case grfx::FORMAT_R8G8_UINT          : return 2; break;
        case grfx::FORMAT_R8G8B8_UINT        : return 3; break;
        case grfx::FORMAT_R8G8B8A8_UINT      : return 4; break;
        case grfx::FORMAT_B8G8R8_UINT        : return 3; break;
        case grfx::FORMAT_B8G8R8A8_UINT      : return 4; break;
                                             
        // 16-bit signed normalized          
        case grfx::FORMAT_R16_SNORM          : return 2; break;
        case grfx::FORMAT_R16G16_SNORM       : return 4; break;
        case grfx::FORMAT_R16G16B16_SNORM    : return 6; break;
        case grfx::FORMAT_R16G16B16A16_SNORM : return 8; break;
                                             
        // 16-bit unsigned normalized        
        case grfx::FORMAT_R16_UNORM          : return 2; break;
        case grfx::FORMAT_R16G16_UNORM       : return 4; break;
        case grfx::FORMAT_R16G16B16_UNORM    : return 6; break;
        case grfx::FORMAT_R16G16B16A16_UNORM : return 8; break;
                                             
        // 16-bit signed integer             
        case grfx::FORMAT_R16_SINT           : return 2; break;
        case grfx::FORMAT_R16G16_SINT        : return 4; break;
        case grfx::FORMAT_R16G16B16_SINT     : return 6; break;
        case grfx::FORMAT_R16G16B16A16_SINT  : return 8; break;
                                             
        // 16-bit unsigned integer           
        case grfx::FORMAT_R16_UINT           : return 2; break;
        case grfx::FORMAT_R16G16_UINT        : return 4; break;
        case grfx::FORMAT_R16G16B16_UINT     : return 6; break;
        case grfx::FORMAT_R16G16B16A16_UINT  : return 8; break;
                                             
        // 16-bit float                      
        case grfx::FORMAT_R16_FLOAT          : return 2; break;
        case grfx::FORMAT_R16G16_FLOAT       : return 4; break;
        case grfx::FORMAT_R16G16B16_FLOAT    : return 6; break;
        case grfx::FORMAT_R16G16B16A16_FLOAT : return 8; break;
                                             
        // 32-bit signed integer             
        case grfx::FORMAT_R32_SINT           : return 4; break;
        case grfx::FORMAT_R32G32_SINT        : return 8; break;
        case grfx::FORMAT_R32G32B32_SINT     : return 12; break;
        case grfx::FORMAT_R32G32B32A32_SINT  : return 16; break;
                                             
        // 32-bit unsigned integer           
        case grfx::FORMAT_R32_UINT           : return 4; break;
        case grfx::FORMAT_R32G32_UINT        : return 8; break;
        case grfx::FORMAT_R32G32B32_UINT     : return 12; break;
        case grfx::FORMAT_R32G32B32A32_UINT  : return 16; break;
                                             
        // 32-bit float                      
        case grfx::FORMAT_R32_FLOAT          : return 4; break;
        case grfx::FORMAT_R32G32_FLOAT       : return 8; break;
        case grfx::FORMAT_R32G32B32_FLOAT    : return 12; break;
        case grfx::FORMAT_R32G32B32A32_FLOAT : return 16; break;
                                             
        // 8-bit unsigned integer stencil    
        case grfx::FORMAT_S8_UINT            : return 1; break;
                                             
        // 16-bit unsigned normalized depth  
        case grfx::FORMAT_D16_UNORM          : return 2; break;
                                             
        // 32-bit float depth                
        case grfx::FORMAT_D32_FLOAT          : return 4; break;
                                             
        // Depth/stencil combinations        
        case grfx::FORMAT_D16_UNORM_S8_UINT  : return 0; break;
        case grfx::FORMAT_D24_UNORM_S8_UINT  : return 0; break;
        case grfx::FORMAT_D32_FLOAT_S8_UINT  : return 0; break;
                                             
        // SRGB                              
        case grfx::FORMAT_R8_SRGB            : return 1; break;
        case grfx::FORMAT_R8G8_SRGB          : return 2; break;
        case grfx::FORMAT_R8G8B8_SRGB        : return 3; break;
        case grfx::FORMAT_R8G8B8A8_SRGB      : return 4; break;
        case grfx::FORMAT_B8G8R8_SRGB        : return 3; break;
        case grfx::FORMAT_B8G8R8A8_SRGB      : return 4; break;
    }
    // clang-format on
    return 0;
}

} // namespace grfx
} // namespace ppx
