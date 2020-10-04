#ifndef ppx_grfx_shader_h
#define ppx_grfx_shader_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct ShaderModuleCreateInfo
//!
//!
struct ShaderModuleCreateInfo
{
    uint32_t    size  = 0;
    const char* pCode = nullptr;
};

//! @class ShaderModule
//!
//!
class ShaderModule
    : public grfx::DeviceObject<grfx::ShaderModuleCreateInfo>
{
public:
    ShaderModule() {}
    virtual ~ShaderModule() {}
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_shader_h
