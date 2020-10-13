#include "ppx/grfx/grfx_swapchain.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_render_pass.h"

namespace ppx {
namespace grfx {

Result Swapchain::Create(const grfx::SwapchainCreateInfo* pCreateInfo)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo->pQueue);
    PPX_ASSERT_NULL_ARG(pCreateInfo->pSurface);
    if (IsNull(pCreateInfo->pQueue) || IsNull(pCreateInfo->pSurface)) {
        return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
    }

    Result ppxres = grfx::DeviceObject<grfx::SwapchainCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    if (pCreateInfo->depthFormat != grfx::FORMAT_UNDEFINED) {
        for (uint32_t i = 0; i < pCreateInfo->imageCount; ++i) {
            grfx::ImageCreateInfo dpCreateInfo = ImageCreateInfo::DepthStencilTarget(pCreateInfo->width, pCreateInfo->height, pCreateInfo->depthFormat);

            grfx::ImagePtr depthStencilTarget;
            ppxres = GetDevice()->CreateImage(&dpCreateInfo, &depthStencilTarget);
            if (Failed(ppxres)) {
                return ppxres;
            }

            mDepthStencilImages.push_back(depthStencilTarget);
        }
    }

    // Create render passes
    for (size_t i = 0; i < pCreateInfo->imageCount; ++i) {
        grfx::RenderPassCreateInfo3 rpCreateInfo = {};
        rpCreateInfo.width                       = pCreateInfo->width;
        rpCreateInfo.height                      = pCreateInfo->height;
        rpCreateInfo.renderTargetCount           = 1;
        rpCreateInfo.pRenderTargetImages[0]      = mColorImages[i];
        rpCreateInfo.pDepthStencilImage          = mDepthStencilImages.empty() ? nullptr : mDepthStencilImages[i];
        rpCreateInfo.renderTargetClearValues[0]  = {0.0f, 0.0f, 0.0f, 0.0f};
        rpCreateInfo.depthStencilClearValue      = {1.0f, 0xFF};
        rpCreateInfo.renderTargetLoadOps[0]      = {grfx::ATTACHMENT_LOAD_OP_CLEAR};
        rpCreateInfo.depthLoadOp                 = grfx::ATTACHMENT_LOAD_OP_CLEAR;

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
    if (!IsIndexInRange(imageIndex, mColorImages)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppImage = mColorImages[imageIndex];
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
