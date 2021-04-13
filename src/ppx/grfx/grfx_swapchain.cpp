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
            dpCreateInfo.ownership             = grfx::OWNERSHIP_RESTRICTED;

            grfx::ImagePtr depthStencilTarget;
            ppxres = GetDevice()->CreateImage(&dpCreateInfo, &depthStencilTarget);
            if (Failed(ppxres)) {
                return ppxres;
            }

            mDepthImages.push_back(depthStencilTarget);
        }
    }

    // Create render passes with grfx::ATTACHMENT_LOAD_OP_CLEAR for render target
    for (size_t i = 0; i < pCreateInfo->imageCount; ++i) {
        grfx::RenderPassCreateInfo3 rpCreateInfo = {};
        rpCreateInfo.width                       = pCreateInfo->width;
        rpCreateInfo.height                      = pCreateInfo->height;
        rpCreateInfo.renderTargetCount           = 1;
        rpCreateInfo.pRenderTargetImages[0]      = mColorImages[i];
        rpCreateInfo.pDepthStencilImage          = mDepthImages.empty() ? nullptr : mDepthImages[i];
        rpCreateInfo.renderTargetClearValues[0]  = {0.0f, 0.0f, 0.0f, 0.0f};
        rpCreateInfo.depthStencilClearValue      = {1.0f, 0xFF};
        rpCreateInfo.renderTargetLoadOps[0]      = grfx::ATTACHMENT_LOAD_OP_CLEAR;
        rpCreateInfo.depthLoadOp                 = grfx::ATTACHMENT_LOAD_OP_CLEAR;
        rpCreateInfo.ownership                   = grfx::OWNERSHIP_RESTRICTED;

        grfx::RenderPassPtr renderPass;
        ppxres = GetDevice()->CreateRenderPass(&rpCreateInfo, &renderPass);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Swapchain::CreateRenderPass(CLEAR) failed");
            return ppxres;
        }

        mClearRenderPasses.push_back(renderPass);
    }

    // Create render passes with grfx::ATTACHMENT_LOAD_OP_LOAD for render target
    for (size_t i = 0; i < pCreateInfo->imageCount; ++i) {
        grfx::RenderPassCreateInfo3 rpCreateInfo = {};
        rpCreateInfo.width                       = pCreateInfo->width;
        rpCreateInfo.height                      = pCreateInfo->height;
        rpCreateInfo.renderTargetCount           = 1;
        rpCreateInfo.pRenderTargetImages[0]      = mColorImages[i];
        rpCreateInfo.pDepthStencilImage          = mDepthImages.empty() ? nullptr : mDepthImages[i];
        rpCreateInfo.renderTargetClearValues[0]  = {0.0f, 0.0f, 0.0f, 0.0f};
        rpCreateInfo.depthStencilClearValue      = {1.0f, 0xFF};
        rpCreateInfo.renderTargetLoadOps[0]      = grfx::ATTACHMENT_LOAD_OP_LOAD;
        rpCreateInfo.depthLoadOp                 = grfx::ATTACHMENT_LOAD_OP_CLEAR;
        rpCreateInfo.ownership                   = grfx::OWNERSHIP_RESTRICTED;

        grfx::RenderPassPtr renderPass;
        ppxres = GetDevice()->CreateRenderPass(&rpCreateInfo, &renderPass);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Swapchain::CreateRenderPass(LOAD) failed");
            return ppxres;
        }

        mLoadRenderPasses.push_back(renderPass);
    }

    PPX_LOG_INFO("Swapchain created");
    PPX_LOG_INFO("   " << "resolution  : " << pCreateInfo->width << "x" << pCreateInfo->height);
    PPX_LOG_INFO("   " << "image count : " << pCreateInfo->imageCount);

    return ppx::SUCCESS;
}

void Swapchain::Destroy()
{
    for (auto& elem : mClearRenderPasses) {
        if (elem) {
            GetDevice()->DestroyRenderPass(elem);
        }
    }
    mClearRenderPasses.clear();

    for (auto& elem : mLoadRenderPasses) {
        if (elem) {
            GetDevice()->DestroyRenderPass(elem);
        }
    }
    mLoadRenderPasses.clear();

    for (auto& elem : mDepthImages) {
        if (elem) {
            GetDevice()->DestroyImage(elem);
        }
    }
    mDepthImages.clear();

    for (auto& elem : mColorImages) {
        if (elem) {
            GetDevice()->DestroyImage(elem);
        }
    }
    mColorImages.clear();

    grfx::DeviceObject<grfx::SwapchainCreateInfo>::Destroy();
}

Result Swapchain::GetColorImage(uint32_t imageIndex, grfx::Image** ppImage) const
{
    if (!IsIndexInRange(imageIndex, mColorImages)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppImage = mColorImages[imageIndex];
    return ppx::SUCCESS;
}

Result Swapchain::GetDepthImage(uint32_t imageIndex, grfx::Image** ppImage) const
{
    if (!IsIndexInRange(imageIndex, mDepthImages)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppImage = mDepthImages[imageIndex];
    return ppx::SUCCESS;
}

Result Swapchain::GetRenderPass(uint32_t imageIndex, grfx::AttachmentLoadOp loadOp, grfx::RenderPass** ppRenderPass) const
{
    if (!IsIndexInRange(imageIndex, mClearRenderPasses)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    if (loadOp == grfx::ATTACHMENT_LOAD_OP_CLEAR) {
        *ppRenderPass = mClearRenderPasses[imageIndex];
    }
    else {
        *ppRenderPass = mLoadRenderPasses[imageIndex];
    }
    return ppx::SUCCESS;
}

grfx::ImagePtr Swapchain::GetColorImage(uint32_t imageIndex) const
{
    grfx::ImagePtr object;
    GetColorImage(imageIndex, &object);
    return object;
}

grfx::ImagePtr Swapchain::GetDepthImage(uint32_t imageIndex) const
{
    grfx::ImagePtr object;
    GetDepthImage(imageIndex, &object);
    return object;
}

grfx::RenderPassPtr Swapchain::GetRenderPass(uint32_t imageIndex, grfx::AttachmentLoadOp loadOp) const
{
    grfx::RenderPassPtr object;
    GetRenderPass(imageIndex, loadOp, &object);
    return object;
}

} // namespace grfx
} // namespace ppx
