#include "ppx/grfx/grfx_render_pass.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {

// -------------------------------------------------------------------------------------------------
// RenderPassCreateInfo
// -------------------------------------------------------------------------------------------------
void RenderPassCreateInfo::SetAllRenderTargetClearValue(const grfx::RenderTargetClearValue& value)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = value;
    }
}

// -------------------------------------------------------------------------------------------------
// RenderPassCreateInfo2
// -------------------------------------------------------------------------------------------------
void RenderPassCreateInfo2::SetAllRenderTargetUsageFlags(const grfx::ImageUsageFlags& flags)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetUsageFlags[i] = flags;
    }
}

void RenderPassCreateInfo2::SetAllRenderTargetClearValue(const grfx::RenderTargetClearValue& value)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = value;
    }
}

void RenderPassCreateInfo2::SetAllRenderTargetLoadOp(grfx::AttachmentLoadOp op)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetLoadOps[i] = op;
    }
}

void RenderPassCreateInfo2::SetAllRenderTargetStoreOp(grfx::AttachmentStoreOp op)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetStoreOps[i] = op;
    }
}

void RenderPassCreateInfo2::SetAllRenderTargetToClear()
{
    SetAllRenderTargetLoadOp(grfx::ATTACHMENT_LOAD_OP_CLEAR);
}

// -------------------------------------------------------------------------------------------------
// RenderPassCreateInfo3
// -------------------------------------------------------------------------------------------------
void RenderPassCreateInfo3::SetAllRenderTargetClearValue(const grfx::RenderTargetClearValue& value)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = value;
    }
}

void RenderPassCreateInfo3::SetAllRenderTargetLoadOp(grfx::AttachmentLoadOp op)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetLoadOps[i] = op;
    }
}

void RenderPassCreateInfo3::SetAllRenderTargetStoreOp(grfx::AttachmentStoreOp op)
{
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetStoreOps[i] = op;
    }
}

void RenderPassCreateInfo3::SetAllRenderTargetToClear()
{
    SetAllRenderTargetLoadOp(grfx::ATTACHMENT_LOAD_OP_CLEAR);
}

// -------------------------------------------------------------------------------------------------
// internal
// -------------------------------------------------------------------------------------------------
namespace internal {

RenderPassCreateInfo::RenderPassCreateInfo(const grfx::RenderPassCreateInfo& obj)
{
    this->version = CREATE_INFO_VERSION_1;
    this->width   = obj.width;
    this->height  = obj.height;

    // Views
    this->renderTargetCount = this->renderTargetCount;
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V1.pRenderTargetViews[i] = obj.pRenderTargetViews[i];
    }
    this->V1.pDepthStencilView = obj.pDepthStencilView;

    // Clear values
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = obj.renderTargetClearValues[i];
    }
    this->depthStencilClearValue = obj.depthStencilClearValue;
}

RenderPassCreateInfo::RenderPassCreateInfo(const grfx::RenderPassCreateInfo2& obj)
{
    this->version           = CREATE_INFO_VERSION_2;
    this->width             = obj.width;
    this->height            = obj.height;
    this->renderTargetCount = obj.renderTargetCount;

    // Formats
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V2.renderTargetFormats[i] = obj.renderTargetFormats[i];
    }
    this->V2.depthStencilFormat = obj.depthStencilFormat;

    // Sample count
    this->V2.sampleCount = obj.sampleCount;

    // Usage flags
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V2.renderTargetUsageFlags[i] = obj.renderTargetUsageFlags[i];
    }
    this->V2.depthStencilUsageFlags = obj.depthStencilUsageFlags;

    // Clear values
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = obj.renderTargetClearValues[i];
    }
    this->depthStencilClearValue = obj.depthStencilClearValue;

    // Load/store ops
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetLoadOps[i]  = obj.renderTargetLoadOps[i];
        this->renderTargetStoreOps[i] = obj.renderTargetStoreOps[i];
    }
    this->depthLoadOp    = obj.depthLoadOp;
    this->depthStoreOp   = obj.depthStoreOp;
    this->stencilLoadOp  = obj.stencilLoadOp;
    this->stencilStoreOp = obj.stencilStoreOp;
}

RenderPassCreateInfo::RenderPassCreateInfo(const grfx::RenderPassCreateInfo3& obj)
{
    this->version           = CREATE_INFO_VERSION_3;
    this->width             = obj.width;
    this->height            = obj.height;
    this->renderTargetCount = obj.renderTargetCount;

    // Images
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->V3.pRenderTargetImages[i] = obj.pRenderTargetImages[i];
    }
    this->V3.pDepthStencilImage = obj.pDepthStencilImage;

    // Clear values
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetClearValues[i] = obj.renderTargetClearValues[i];
    }
    this->depthStencilClearValue = obj.depthStencilClearValue;

    // Load/store ops
    for (uint32_t i = 0; i < this->renderTargetCount; ++i) {
        this->renderTargetLoadOps[i]  = obj.renderTargetLoadOps[i];
        this->renderTargetStoreOps[i] = obj.renderTargetStoreOps[i];
    }
    this->depthLoadOp    = obj.depthLoadOp;
    this->depthStoreOp   = obj.depthStoreOp;
    this->stencilLoadOp  = obj.stencilLoadOp;
    this->stencilStoreOp = obj.stencilStoreOp;
}

} // namespace internal

// -------------------------------------------------------------------------------------------------
// RenderPass
// -------------------------------------------------------------------------------------------------
Result RenderPass::CreateImagesAndViewsV1(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    // Copy RTV and images
    for (uint32_t i = 0; i < pCreateInfo->renderTargetCount; ++i) {
        grfx::RenderTargetViewPtr rtv = pCreateInfo->V1.pRenderTargetViews[i];
        if (!rtv) {
            PPX_ASSERT_MSG(false, "RTV << " << i << " is null");
            return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
        }
        if (!rtv->GetImage()) {
            PPX_ASSERT_MSG(false, "image << " << i << " is null");
            return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
        }

        mRenderTargetViews.push_back({true, rtv});
        mRenderTargetImages.push_back({true, rtv->GetImage()});
    }
    // Copy DSV and image
    if (!IsNull(pCreateInfo->V1.pDepthStencilView)) {
        grfx::DepthStencilViewPtr dsv = pCreateInfo->V1.pDepthStencilView;

        mDepthStencilView  = ExtObjPtr<grfx::DepthStencilViewPtr>{true, dsv};
        mDepthStencilImage = ExtObjPtr<grfx::ImagePtr>{true, dsv->GetImage()};
    }

    return ppx::SUCCESS;
}

Result RenderPass::CreateImagesAndViewsV2(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
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
            imageCreateInfo.format                = pCreateInfo->V2.renderTargetFormats[i];
            imageCreateInfo.sampleCount           = pCreateInfo->V2.sampleCount;
            imageCreateInfo.mipLevelCount         = 1;
            imageCreateInfo.arrayLayerCount       = 1;
            imageCreateInfo.usageFlags            = pCreateInfo->V2.renderTargetUsageFlags[i];

            grfx::ImagePtr image;
            Result         ppxres = GetDevice()->CreateImage(&imageCreateInfo, &image);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "RTV image create failed");
                return ppxres;
            }

            mRenderTargetImages.push_back({false, image});
        }

        // DSV image
        if (pCreateInfo->V2.depthStencilFormat != grfx::FORMAT_UNDEFINED) {
            grfx::ImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.type                  = grfx::IMAGE_TYPE_2D;
            imageCreateInfo.width                 = pCreateInfo->width;
            imageCreateInfo.height                = pCreateInfo->height;
            imageCreateInfo.depth                 = 1;
            imageCreateInfo.format                = pCreateInfo->V2.depthStencilFormat;
            imageCreateInfo.sampleCount           = pCreateInfo->V2.sampleCount;
            imageCreateInfo.mipLevelCount         = 1;
            imageCreateInfo.arrayLayerCount       = 1;
            imageCreateInfo.usageFlags            = pCreateInfo->V2.depthStencilUsageFlags;

            grfx::ImagePtr image;
            Result         ppxres = GetDevice()->CreateImage(&imageCreateInfo, &image);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "DSV image create failed");
                return ppxres;
            }

            mDepthStencilImage = ExtObjPtr<grfx::ImagePtr>{false, image};
        }
    }

    // Create views
    {
        // RTVs
        for (uint32_t i = 0; i < pCreateInfo->renderTargetCount; ++i) {
            grfx::ImagePtr image = mRenderTargetImages[i].object;

            grfx::RenderTargetViewCreateInfo rtvCreateInfo = {};
            rtvCreateInfo.pImage                           = image;
            rtvCreateInfo.imageViewType                    = grfx::IMAGE_VIEW_TYPE_2D;
            rtvCreateInfo.format                           = pCreateInfo->V2.renderTargetFormats[i];
            rtvCreateInfo.sampleCount                      = image->GetSampleCount();
            rtvCreateInfo.mipLevel                         = 0;
            rtvCreateInfo.mipLevelCount                    = 1;
            rtvCreateInfo.arrayLayer                       = 0;
            rtvCreateInfo.arrayLayerCount                  = 1;
            rtvCreateInfo.components                       = {};
            rtvCreateInfo.loadOp                           = pCreateInfo->renderTargetLoadOps[i];
            rtvCreateInfo.storeOp                          = pCreateInfo->renderTargetStoreOps[i];

            grfx::RenderTargetViewPtr rtv;
            Result                    ppxres = GetDevice()->CreateRenderTargetView(&rtvCreateInfo, &rtv);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "RTV create failed");
                return ppxres;
            }

            mRenderTargetViews.push_back({false, rtv});
        }

        // DSV
        if (pCreateInfo->V2.depthStencilFormat != grfx::FORMAT_UNDEFINED) {
            grfx::ImagePtr image = mDepthStencilImage.object;

            grfx::DepthStencilViewCreateInfo dsvCreateInfo = {};
            dsvCreateInfo.pImage                           = image;
            dsvCreateInfo.imageViewType                    = grfx::IMAGE_VIEW_TYPE_2D;
            dsvCreateInfo.format                           = pCreateInfo->V2.depthStencilFormat;
            dsvCreateInfo.mipLevel                         = 0;
            dsvCreateInfo.mipLevelCount                    = 1;
            dsvCreateInfo.arrayLayer                       = 0;
            dsvCreateInfo.arrayLayerCount                  = 1;
            dsvCreateInfo.components                       = {};
            dsvCreateInfo.depthLoadOp                      = pCreateInfo->depthLoadOp;
            dsvCreateInfo.depthStoreOp                     = pCreateInfo->depthStoreOp;
            dsvCreateInfo.stencilLoadOp                    = pCreateInfo->stencilLoadOp;
            dsvCreateInfo.stencilStoreOp                   = pCreateInfo->stencilStoreOp;

            grfx::DepthStencilViewPtr dsv;
            Result                    ppxres = GetDevice()->CreateDepthStencilView(&dsvCreateInfo, &dsv);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "RTV create failed");
                return ppxres;
            }

            mDepthStencilView = ExtObjPtr<grfx::DepthStencilViewPtr>{false, dsv};
        }
    }

    return ppx::SUCCESS;
}

Result RenderPass::CreateImagesAndViewsV3(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    // Copy images
    {
        // Copy RTV images
        for (uint32_t i = 0; i < pCreateInfo->renderTargetCount; ++i) {
            grfx::ImagePtr image = pCreateInfo->V3.pRenderTargetImages[i];
            if (!image) {
                PPX_ASSERT_MSG(false, "image << " << i << " is null");
                return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
            }

            mRenderTargetImages.push_back({true, image});
        }
        // Copy DSV image
        if (!IsNull(pCreateInfo->V3.pDepthStencilImage)) {
            grfx::ImagePtr image = pCreateInfo->V3.pDepthStencilImage;

            mDepthStencilImage = ExtObjPtr<grfx::ImagePtr>{true, image};
        }
    }

    // Create views
    {
        // RTVs
        for (uint32_t i = 0; i < pCreateInfo->renderTargetCount; ++i) {
            grfx::ImagePtr image = mRenderTargetImages[i].object;

            grfx::RenderTargetViewCreateInfo rtvCreateInfo = {};
            rtvCreateInfo.pImage                           = image;
            rtvCreateInfo.imageViewType                    = image->GuessImageViewType();
            rtvCreateInfo.format                           = image->GetFormat();
            rtvCreateInfo.sampleCount                      = image->GetSampleCount();
            rtvCreateInfo.mipLevel                         = 0;
            rtvCreateInfo.mipLevelCount                    = image->GetMipLevelCount();
            rtvCreateInfo.arrayLayer                       = 0;
            rtvCreateInfo.arrayLayerCount                  = image->GetArrayLayerCount();
            rtvCreateInfo.components                       = {};
            rtvCreateInfo.loadOp                           = pCreateInfo->renderTargetLoadOps[i];
            rtvCreateInfo.storeOp                          = pCreateInfo->renderTargetStoreOps[i];

            grfx::RenderTargetViewPtr rtv;
            Result                    ppxres = GetDevice()->CreateRenderTargetView(&rtvCreateInfo, &rtv);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "RTV create failed");
                return ppxres;
            }

            mRenderTargetViews.push_back({false, rtv});
        }

        // DSV
        if (mDepthStencilImage.object) {
            grfx::ImagePtr image = mDepthStencilImage.object;

            grfx::DepthStencilViewCreateInfo dsvCreateInfo = {};
            dsvCreateInfo.pImage                           = image;
            dsvCreateInfo.imageViewType                    = image->GuessImageViewType();
            dsvCreateInfo.format                           = image->GetFormat();
            dsvCreateInfo.mipLevel                         = 0;
            dsvCreateInfo.mipLevelCount                    = image->GetMipLevelCount();
            dsvCreateInfo.arrayLayer                       = 0;
            dsvCreateInfo.arrayLayerCount                  = image->GetArrayLayerCount();
            dsvCreateInfo.components                       = {};
            dsvCreateInfo.depthLoadOp                      = pCreateInfo->depthLoadOp;
            dsvCreateInfo.depthStoreOp                     = pCreateInfo->depthStoreOp;
            dsvCreateInfo.stencilLoadOp                    = pCreateInfo->stencilLoadOp;
            dsvCreateInfo.stencilStoreOp                   = pCreateInfo->stencilStoreOp;

            grfx::DepthStencilViewPtr dsv;
            Result                    ppxres = GetDevice()->CreateDepthStencilView(&dsvCreateInfo, &dsv);
            if (Failed(ppxres)) {
                PPX_ASSERT_MSG(false, "DSV create failed");
                return ppxres;
            }

            mDepthStencilView = ExtObjPtr<grfx::DepthStencilViewPtr>{false, dsv};
        }
    }

    return ppx::SUCCESS;
}

Result RenderPass::Create(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    mRenderArea = {0, 0, pCreateInfo->width, pCreateInfo->height};

    switch (pCreateInfo->version) {
        default: return ppx::ERROR_INVALID_CREATE_ARGUMENT; break;

        case grfx::internal::RenderPassCreateInfo::CREATE_INFO_VERSION_1: {
            Result ppxres = CreateImagesAndViewsV1(pCreateInfo);
            if (Failed(ppxres)) {
                return ppxres;
            }
        } break;

        case grfx::internal::RenderPassCreateInfo::CREATE_INFO_VERSION_2: {
            Result ppxres = CreateImagesAndViewsV2(pCreateInfo);
            if (Failed(ppxres)) {
                return ppxres;
            }
        } break;

        case grfx::internal::RenderPassCreateInfo::CREATE_INFO_VERSION_3: {
            Result ppxres = CreateImagesAndViewsV3(pCreateInfo);
            if (Failed(ppxres)) {
                return ppxres;
            }
        } break;
    }

    Result ppxres = grfx::DeviceObject<grfx::internal::RenderPassCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void RenderPass::Destroy()
{
    if ((mDepthStencilView.object) && (mDepthStencilView.isExternal == false)) {
        GetDevice()->DestroyDepthStencilView(mDepthStencilView.object);
        mDepthStencilView.object.Reset();
    }

    if (mDepthStencilImage.object && (mDepthStencilImage.isExternal == false)) {
        GetDevice()->DestroyImage(mDepthStencilImage.object);
        mDepthStencilImage.object.Reset();
    }

    for (uint32_t i = 0; i < mCreateInfo.renderTargetCount; ++i) {
        ExtObjPtr<grfx::RenderTargetViewPtr> rtv = mRenderTargetViews[i];
        if ((rtv.object) && (rtv.isExternal == false)) {
            GetDevice()->DestroyRenderTargetView(rtv.object);
            rtv.object.Reset();
        }

        ExtObjPtr<grfx::ImagePtr> image = mRenderTargetImages[i];
        if (image.object && (image.isExternal == false)) {
            GetDevice()->DestroyImage(image.object);
            image.object.Reset();
        }
    }

    grfx::DeviceObject<grfx::internal::RenderPassCreateInfo>::Destroy();
}

Result RenderPass::GetRenderTargetView(uint32_t index, grfx::RenderTargetView** ppView) const
{
    if (!IsIndexInRange(index, mRenderTargetViews)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppView = mRenderTargetViews[index].object;
    return ppx::SUCCESS;
}

Result RenderPass::GetDepthStencilView(grfx::DepthStencilView** ppView) const
{
    if (!mDepthStencilView.object) {
        return ppx::ERROR_ELEMENT_NOT_FOUND;
    }
    *ppView = mDepthStencilView.object;
    return ppx::SUCCESS;
}

Result RenderPass::GetRenderTargetImage(uint32_t index, grfx::Image** ppImage) const
{
    if (!IsIndexInRange(index, mRenderTargetImages)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppImage = mRenderTargetImages[index].object;
    return ppx::SUCCESS;
}

Result RenderPass::GetDepthStencilImage(grfx::Image** ppImage) const
{
    if (!mDepthStencilImage.object) {
        return ppx::ERROR_ELEMENT_NOT_FOUND;
    }
    *ppImage = mDepthStencilImage.object;
    return ppx::SUCCESS;
}

grfx::RenderTargetViewPtr RenderPass::GetRenderTargetView(uint32_t index) const
{
    grfx::RenderTargetViewPtr object;
    GetRenderTargetView(index, &object);
    return object;
}

grfx::DepthStencilViewPtr RenderPass::GetDepthStencilView() const
{
    grfx::DepthStencilViewPtr object;
    GetDepthStencilView(&object);
    return object;
}

grfx::ImagePtr RenderPass::GetRenderTargetImage(uint32_t index) const
{
    grfx::ImagePtr object;
    GetRenderTargetImage(index, &object);
    return object;
}

grfx::ImagePtr RenderPass::GetDepthStencilImage() const
{
    grfx::ImagePtr object;
    GetDepthStencilImage(&object);
    return object;
}

Result RenderPass::DisownRenderTargetView(uint32_t index, grfx::RenderTargetView** ppView)
{
    if (IsIndexInRange(index, mRenderTargetViews)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    mRenderTargetViews[index].isExternal = true;
    if (!IsNull(ppView)) {
        *ppView = mRenderTargetViews[index].object;
    }
    return ppx::SUCCESS;
}

Result RenderPass::DisownDepthStencilView(grfx::DepthStencilView** ppView)
{
    if (!mDepthStencilView.object) {
        return ppx::ERROR_ELEMENT_NOT_FOUND;
    }
    mDepthStencilView.isExternal = true;
    if (!IsNull(ppView)) {
        *ppView = mDepthStencilView.object;
    }
    return ppx::SUCCESS;
}

Result RenderPass::DisownRenderTargetImage(uint32_t index, grfx::Image** ppImage)
{
    if (IsIndexInRange(index, mRenderTargetImages)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    mRenderTargetImages[index].isExternal = true;
    if (!IsNull(ppImage)) {
        *ppImage = mRenderTargetImages[index].object;
    }
    return ppx::SUCCESS;
}

Result RenderPass::DisownDepthStencilImage(grfx::Image** ppImage)
{
    if (!mDepthStencilImage.object) {
        return ppx::ERROR_ELEMENT_NOT_FOUND;
    }
    mDepthStencilImage.isExternal = true;
    if (!IsNull(ppImage)) {
        *ppImage = mDepthStencilImage.object;
    }
    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx