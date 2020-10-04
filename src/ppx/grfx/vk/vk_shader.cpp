#include "ppx/grfx/vk/vk_shader.h"
#include "ppx/grfx/vk/vk_device.h"

namespace ppx {
namespace grfx {
namespace vk {

// -------------------------------------------------------------------------------------------------
// ShaderModule
// -------------------------------------------------------------------------------------------------
Result ShaderModule::CreateApiObjects(const grfx::ShaderModuleCreateInfo* pCreateInfo)
{
    VkShaderModuleCreateInfo vkci = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    vkci.flags                    = 0;
    vkci.codeSize                 = static_cast<size_t>(pCreateInfo->size);
    vkci.pCode                    = reinterpret_cast<const uint32_t*>(pCreateInfo->pCode);

    VkResult vkres = vkCreateShaderModule(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mShaderModule);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateShaderModule failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void ShaderModule::DestroyApiObjects()
{
    if (mShaderModule) {
        vkDestroyShaderModule(
            ToApi(GetDevice())->GetVkDevice(),
            mShaderModule,
            nullptr);

        mShaderModule.Reset();
    }
}

} // namespace vk
} // namespace grfx
} // namespace ppx
