#include "ppx/grfx/dx11/dx11_shader.h"
#include "ppx/grfx/dx11/dx11_device.h"

namespace ppx {
namespace grfx {
namespace dx11 {

Result ShaderModule::CreateApiObjects(const grfx::ShaderModuleCreateInfo* pCreateInfo)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo->pCode);
    if (IsNull(pCreateInfo->pCode) || (pCreateInfo->size ==0)) {
        return ppx::ERROR_INVALID_CREATE_ARGUMENT;
    }

    mCode.resize(pCreateInfo->size);
    std::memcpy(mCode.data(), pCreateInfo->pCode, pCreateInfo->size);

    return ppx::SUCCESS;
}

void ShaderModule::DestroyApiObjects()
{
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
