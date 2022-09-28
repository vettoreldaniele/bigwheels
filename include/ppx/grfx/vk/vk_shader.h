#ifndef ppx_grfx_vk_shader_h
#define ppx_grfx_vk_shader_h

#include "ppx/grfx/vk/vk_config.h"
#include "ppx/grfx/grfx_shader.h"

namespace ppx {
namespace grfx {
namespace vk {

//! @class Fence
//!
//!
class ShaderModule
    : public grfx::ShaderModule
{
public:
    ShaderModule() {}
    virtual ~ShaderModule() {}

    VkShaderModulePtr GetVkShaderModule() const { return mShaderModule; }

protected:
    virtual Result CreateApiObjects(const grfx::ShaderModuleCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkShaderModulePtr mShaderModule;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_shader_h
