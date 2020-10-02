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

protected:
    virtual Result CreateApiObjects(const grfx::ShaderModuleCreateInfo* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                                               = 0;
    friend class grfx::Device;

private:
    grfx::ShaderModuleCreateInfo mCreateInfo = {};
};

// -------------------------------------------------------------------------------------------------

//! @struct ShaderProgramCreateInfo
//!
//!
struct ShaderProgramCreateInfo
{
    const ShaderModule* pVS = nullptr;
    const ShaderModule* pHS = nullptr;
    const ShaderModule* pDS = nullptr;
    const ShaderModule* pGS = nullptr;
    const ShaderModule* pPS = nullptr;
    const ShaderModule* pCS = nullptr;
};

//! @class ShaderProgram
//!
//!
class ShaderProgram
    : public grfx::DeviceObject<grfx::ShaderProgramCreateInfo>
{
public:
    ShaderProgram() {}
    virtual ~ShaderProgram() {}

protected:
    virtual Result CreateApiObjects(const grfx::ShaderProgramCreateInfo* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                                                = 0;
    friend class grfx::Device;

private:
    grfx::ShaderProgramCreateInfo mCreateInfo = {};
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_shader_h
