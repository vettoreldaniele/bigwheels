#include "ppx/grfx/grfx_swapchain.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_render_pass.h"

namespace ppx {
namespace grfx {

Result Swapchain::Create(const grfx::SwapchainCreateInfo* pCreateInfo)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo->pSurface);

    Result ppxres = grfx::DeviceObject<grfx::SwapchainCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    for (size_t i = 0; i < mImages.size(); ++i) {
        grfx::RenderPassCreateInfo3 rpCreateInfo = {};
        rpCreateInfo.width                       = pCreateInfo->width;
        rpCreateInfo.height                      = pCreateInfo->height;
        rpCreateInfo.renderTargetCount           = 1;
        rpCreateInfo.pRenderTargetImages[0]      = mImages[i];
        rpCreateInfo.pDepthStencilImage          = nullptr;
        rpCreateInfo.renderTargetClearValues[0]  = {0.0f, 0.0f, 0.0f, 0.0f};
        rpCreateInfo.depthStencilClearValue      = {1.0f, 0xFF};
        rpCreateInfo.renderTargetLoadOps[0]      = {grfx::ATTACHMENT_LOAD_OP_CLEAR};

        grfx::RenderPassPtr renderPass;
        ppxres = GetDevice()->CreateRenderPass(&rpCreateInfo, &renderPass);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Swapchain::CreateRenderPass failed");
            return ppxres;
        }

        mRenderPasses.push_back(renderPass);
    }

    return ppx::SUCCESS;
}

Result Swapchain::GetImage(uint32_t imageIndex, grfx::Image** ppImage) const
{
    if (!IsIndexInRange(imageIndex, mImages)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppImage = mImages[imageIndex];
    return ppx::SUCCESS;
}

Result Swapchain::GetRenderPass(uint32_t imageIndex, grfx::RenderPass** ppRenderPass) const
{
    if (!IsIndexInRange(imageIndex, mRenderPasses)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppRenderPass = mRenderPasses[imageIndex];
    return ppx::SUCCESS;
}

grfx::ImagePtr Swapchain::GetImage(uint32_t imageIndex) const
{
    grfx::ImagePtr object;
    GetImage(imageIndex, &object);
    return object;
}

grfx::RenderPassPtr Swapchain::GetRenderPass(uint32_t imageIndex) const
{
    grfx::RenderPassPtr object;
    GetRenderPass(imageIndex, &object);
    return object;
}

} // namespace grfx
} // namespace ppx
