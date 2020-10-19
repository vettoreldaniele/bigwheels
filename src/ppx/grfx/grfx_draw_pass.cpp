#include "ppx/grfx/grfx_draw_pass.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_render_pass.h"

namespace ppx {
namespace grfx {

// -------------------------------------------------------------------------------------------------
// internal
// -------------------------------------------------------------------------------------------------
namespace internal {

DrawPassCreateInfo::DrawPassCreateInfo(const grfx::DrawPassCreateInfo& obj)
{
    this->version           = CREATE_INFO_VERSION_1;
    this->width             = obj.width;
    this->height            = obj.height;
    this->renderTargetCount = obj.renderTargetCount;

    // Formats
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V1.renderTargetFormats[i] = obj.renderTargetFormats[i];
    }
    this->V1.depthStencilFormat = obj.depthStencilFormat;

    // Sample count
    this->V1.sampleCount = obj.sampleCount;

    // Usage flags
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V1.renderTargetUsageFlags[i] = obj.renderTargetUsageFlags[i];
    }
    this->V1.depthStencilUsageFlags = obj.depthStencilUsageFlags;

    // Clear values
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = obj.renderTargetClearValues[i];
    }
    this->depthStencilClearValue = obj.depthStencilClearValue;
}

DrawPassCreateInfo::DrawPassCreateInfo(const grfx::DrawPassCreateInfo2& obj)
{
    this->version           = CREATE_INFO_VERSION_2;
    this->width             = obj.width;
    this->height            = obj.height;
    this->renderTargetCount = obj.renderTargetCount;

    // Images
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V2.pRenderTargetImages[i] = obj.pRenderTargetImages[i];
    }
    this->V2.pDepthStencilImage = obj.pDepthStencilImage;

    // Clear values
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = obj.renderTargetClearValues[i];
    }
    this->depthStencilClearValue = obj.depthStencilClearValue;
}

} // namespace internal

// -------------------------------------------------------------------------------------------------
// DrawPass
// -------------------------------------------------------------------------------------------------
Result DrawPass::CreateImagesAndViewsV1(const grfx::internal::DrawPassCreateInfo* pCreateInfo)
{
    // Create images
    {
        // RTV images
        for (uint32_t i = 0; i < pCreateInfo->renderTargetCount; ++i) {
            grfx::ImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.type                  = grfx::IMAGE_TYPE_2D;
            imageCreateInfo.width                 = pCreateInfo->width;
            imageCreateInfo.height                = pCreateInfo->height;
            imageCreateInfo.depth                 = 1;
            imageCreateInfo.format                = pCreateInfo->V1.renderTargetFormats[i];
            imageCreateInfo.sampleCount           = pCreateInfo->V1.sampleCount;
            imageCreateInfo.mipLevelCount         = 1;
            imageCreateInfo.arrayLayerCount       = 1;
            imageCreateInfo.usageFlags            = pCreateInfo->V1.renderTargetUsageFlags[i];

            grfx::ImagePtr image;
            Result         ppxres = GetDevice()->CreateImage(&imageCreateInfo, &image);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "RTV image create failed");
                return ppxres;
            }

            mRenderTargetImages.push_back({false, image});
        }

        // DSV image
        if (pCreateInfo->V1.depthStencilFormat != grfx::FORMAT_UNDEFINED) {
            grfx::ImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.type                  = grfx::IMAGE_TYPE_2D;
            imageCreateInfo.width                 = pCreateInfo->width;
            imageCreateInfo.height                = pCreateInfo->height;
            imageCreateInfo.depth                 = 1;
            imageCreateInfo.format                = pCreateInfo->V1.depthStencilFormat;
            imageCreateInfo.sampleCount           = pCreateInfo->V1.sampleCount;
            imageCreateInfo.mipLevelCount         = 1;
            imageCreateInfo.arrayLayerCount       = 1;
            imageCreateInfo.usageFlags            = pCreateInfo->V1.depthStencilUsageFlags;

            grfx::ImagePtr image;
            Result         ppxres = GetDevice()->CreateImage(&imageCreateInfo, &image);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "DSV image create failed");
                return ppxres;
            }

            mDepthStencilImage = ExtObjPtr<grfx::ImagePtr>{false, image};
        }
    }
    return ppx::SUCCESS;
}

Result DrawPass::CreateImagesAndViewsV2(const grfx::internal::DrawPassCreateInfo* pCreateInfo)
{
    // Copy images
    {
        // Copy RTV images
        for (uint32_t i = 0; i < pCreateInfo->renderTargetCount; ++i) {
            grfx::ImagePtr image = pCreateInfo->V2.pRenderTargetImages[i];
            if (!image) {
                PPX_ASSERT_MSG(false, "image << " << i << " is null");
                return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
            }

            mRenderTargetImages.push_back({true, image});
        }
        // Copy DSV image
        if (!IsNull(pCreateInfo->V2.pDepthStencilImage)) {
            grfx::ImagePtr image = pCreateInfo->V2.pDepthStencilImage;

            mDepthStencilImage = ExtObjPtr<grfx::ImagePtr>{true, image};
        }
    }

    return ppx::SUCCESS;
}

Result DrawPass::CreateApiObjects(const grfx::internal::DrawPassCreateInfo* pCreateInfo)
{
    mRenderArea = {0, 0, pCreateInfo->width, pCreateInfo->height};

    // Create backing resources
    switch (pCreateInfo->version) {
        default: return ppx::ERROR_INVALID_CREATE_ARGUMENT; break;

        case grfx::internal::DrawPassCreateInfo::CREATE_INFO_VERSION_1: {
            Result ppxres = CreateImagesAndViewsV1(pCreateInfo);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "create images and views(V1) failed");
                return ppxres;
            }
        } break;

        case grfx::internal::DrawPassCreateInfo::CREATE_INFO_VERSION_2: {
            Result ppxres = CreateImagesAndViewsV2(pCreateInfo);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "create images and views(V2) failed");
                return ppxres;
            }
        } break;
    }

    // Create render passes
    for (uint32_t clearMask = 0; clearMask <= static_cast<uint32_t>(DRAW_PASS_CLEAR_FLAG_CLEAR_ALL); ++clearMask) {
        grfx::AttachmentLoadOp renderTargetLoadOp = grfx::ATTACHMENT_LOAD_OP_LOAD;
        grfx::AttachmentLoadOp depthLoadOp        = grfx::ATTACHMENT_LOAD_OP_LOAD;
        grfx::AttachmentLoadOp stencilLoadOp      = grfx::ATTACHMENT_LOAD_OP_LOAD;

        if ((clearMask & DRAW_PASS_CLEAR_FLAG_CLEAR_RENDER_TARGETS) != 0) {
            renderTargetLoadOp = grfx::ATTACHMENT_LOAD_OP_CLEAR;
        }
        if ((clearMask & DRAW_PASS_CLEAR_FLAG_CLEAR_DEPTH) != 0) {
            depthLoadOp = grfx::ATTACHMENT_LOAD_OP_CLEAR;
        }
        if ((clearMask & DRAW_PASS_CLEAR_FLAG_CLEAR_STENCIL) != 0) {
            stencilLoadOp = grfx::ATTACHMENT_LOAD_OP_CLEAR;
        }

        grfx::RenderPassCreateInfo3 rpCreateInfo = {};
        rpCreateInfo.width                       = pCreateInfo->width;
        rpCreateInfo.height                      = pCreateInfo->height;
        rpCreateInfo.renderTargetCount           = pCreateInfo->renderTargetCount;

        for (uint32_t i = 0; i < rpCreateInfo.renderTargetCount; ++i) {
            if (!mRenderTargetImages[i].object) {
                continue;
            }
            rpCreateInfo.pRenderTargetImages[i]     = mRenderTargetImages[i].object;
            rpCreateInfo.renderTargetClearValues[i] = pCreateInfo->renderTargetClearValues[i];
            rpCreateInfo.renderTargetLoadOps[i]     = renderTargetLoadOp;
            rpCreateInfo.renderTargetStoreOps[i]    = grfx::ATTACHMENT_STORE_OP_STORE;
        }

        if (mDepthStencilImage.object) {
            rpCreateInfo.pDepthStencilImage     = mDepthStencilImage.object;
            rpCreateInfo.depthStencilClearValue = pCreateInfo->depthStencilClearValue;
            rpCreateInfo.depthLoadOp            = depthLoadOp;
            rpCreateInfo.depthStoreOp           = grfx::ATTACHMENT_STORE_OP_STORE;
            rpCreateInfo.stencilLoadOp          = stencilLoadOp;
            rpCreateInfo.stencilStoreOp         = grfx::ATTACHMENT_STORE_OP_STORE;
        }

        Pass pass      = {};
        pass.clearMask = clearMask;

        Result ppxres = GetDevice()->CreateRenderPass(&rpCreateInfo, &pass.renderPass);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "create render pass failed for clearMask=" << clearMask);
            return ppxres;
        }

        mPasses.push_back(pass);
    }

    return ppx::SUCCESS;
}

void DrawPass::DestroyApiObjects()
{
    for (uint32_t i = 0; i < mCreateInfo.renderTargetCount; ++i) {
        ExtObjPtr<grfx::ImagePtr> image = mRenderTargetImages[i];
        if (image.object && (image.isExternal == false)) {
            GetDevice()->DestroyImage(image.object);
            image.object.Reset();
        }
    }

    if (mDepthStencilImage.object && (mDepthStencilImage.isExternal == false)) {
        GetDevice()->DestroyImage(mDepthStencilImage.object);
        mDepthStencilImage.object.Reset();
    }

    for (size_t i = 0; i < mPasses.size(); ++i) {
        if (mPasses[i].renderPass) {
            GetDevice()->DestroyRenderPass(mPasses[i].renderPass);
            mPasses[i].renderPass.Reset();
        }
    }
    mPasses.clear();
}

void DrawPass::PrepareRenderPassBeginInfo(const grfx::DrawPassClearFlags& clearFlags, grfx::RenderPassBeginInfo* pBeginInfo) const
{
    uint32_t clearMask = clearFlags.flags;

    auto it = FindIf(
        mPasses,
        [clearMask](const Pass& elem) -> bool {
            bool isMatch = (elem.clearMask == clearMask);
            return isMatch; });
    if (it == std::end(mPasses)) {
        PPX_ASSERT_MSG(false, "couldn't find matching pass for clearMask=" << clearMask);
        return;
    }

    pBeginInfo->pRenderPass   = it->renderPass;
    pBeginInfo->renderArea    = GetRenderArea();
    pBeginInfo->RTVClearCount = mCreateInfo.renderTargetCount;
    
    for (uint32_t i = 0; i < mCreateInfo.renderTargetCount; ++i) {
        pBeginInfo->RTVClearValues[i] = mCreateInfo.renderTargetClearValues[i];
    }

    pBeginInfo->DSVClearValue = mCreateInfo.depthStencilClearValue;
}

} // namespace grfx
} // namespace ppx
