#ifndef ppx_grfx_dx11_shader_h
#define ppx_grfx_dx11_shader_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_shader.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class ShaderModule
    : public grfx::ShaderModule
{
public:
    ShaderModule() {}
    virtual ~ShaderModule() {}

    uint32_t    GetSize() const { return CountU32(mCode); }
    const char* GetCode() const { return DataPtr(mCode); }

protected:
    virtual Result CreateApiObjects(const grfx::ShaderModuleCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    std::vector<char> mCode;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_shader_h