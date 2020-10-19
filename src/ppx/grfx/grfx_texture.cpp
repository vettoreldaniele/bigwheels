#include "ppx/grfx/grfx_texture.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {

Result Texture::CreateApiObjects(const grfx::TextureCreateInfo* pCreateInfo)
{
    // Image
    if (!IsNull(pCreateInfo->pImage)) {
        mImage = pCreateInfo->pImage;
    }
    else {
        if (pCreateInfo->usageFlags.bits.colorAttachment && pCreateInfo->usageFlags.bits.depthStencilAttachment) {
            PPX_ASSERT_MSG(false, "texture cannot be both color attachment and depth stencil attachment");
            return ppx::ERROR_INVALID_CREATE_ARGUMENT;
        }

        grfx::ImageCreateInfo ci = {};
        ci.type                  = pCreateInfo->imageType;
        ci.width                 = pCreateInfo->width;
        ci.height                = pCreateInfo->height;
        ci.depth                 = pCreateInfo->depth;
        ci.format                = pCreateInfo->imageFormat;
        ci.sampleCount           = pCreateInfo->sampleCount;
        ci.mipLevelCount         = pCreateInfo->mipLevelCount;
        ci.arrayLayerCount       = pCreateInfo->arrayLayerCount;
        ci.usageFlags            = pCreateInfo->usageFlags;
        ci.memoryUsage           = pCreateInfo->memoryUsage;
        ci.initialState          = pCreateInfo->initialState;
        ci.RTVClearValue         = pCreateInfo->RTVClearValue;
        ci.DSVClearValue         = pCreateInfo->DSVClearValue;
        ci.pApiObject            = nullptr;
        ci.ownership             = pCreateInfo->ownership;

        Result ppxres = GetDevice()->CreateImage(&ci, &mImage);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "texture create image failed");
            return ppxres;
        }
    }

    if (pCreateInfo->usageFlags.bits.sampled) {
        grfx::SampledImageViewCreateInfo ci = grfx::SampledImageViewCreateInfo::GuessFromImage(mImage);
        if (pCreateInfo->sampledImageViewType != grfx::IMAGE_VIEW_TYPE_UNDEFINED) {
            ci.imageViewType = pCreateInfo->sampledImageViewType;
        }

        Result ppxres = GetDevice()->CreateSampledImageView(&ci, &mSampledImageView);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "texture create sampled image view failed");
            return ppxres;
        }
    }

    if (pCreateInfo->usageFlags.bits.colorAttachment) {
        grfx::RenderTargetViewCreateInfo ci = grfx::RenderTargetViewCreateInfo::GuessFromImage(mImage);

        Result ppxres = GetDevice()->CreateRenderTargetView(&ci, &mRenderTargetView);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "texture create render target view failed");
            return ppxres;
        }
    }

    if (pCreateInfo->usageFlags.bits.depthStencilAttachment) {
        grfx::DepthStencilViewCreateInfo ci = grfx::DepthStencilViewCreateInfo::GuessFromImage(mImage);

        Result ppxres = GetDevice()->CreateDepthStencilView(&ci, &mDepthStencilView);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "texture create depth stencil view failed");
            return ppxres;
        }
    }

    if (pCreateInfo->usageFlags.bits.storage) {
        grfx::StorageImageViewCreateInfo ci = grfx::StorageImageViewCreateInfo::GuessFromImage(mImage);

        Result ppxres = GetDevice()->CreateStorageImageView(&ci, &mStorageImageView);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "texture create storage image view failed");
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

void Texture::DestroyApiObjects()
{
    if (mSampledImageView && (mSampledImageView->GetOwnership() != grfx::OWNERSHIP_REFERENCE)) {
        GetDevice()->DestroySampledImageView(mSampledImageView);
        mSampledImageView.Reset();
    }

    if (mRenderTargetView && (mRenderTargetView->GetOwnership() != grfx::OWNERSHIP_REFERENCE)) {
        GetDevice()->DestroyRenderTargetView(mRenderTargetView);
        mRenderTargetView.Reset();
    }

    if (mDepthStencilView && (mDepthStencilView->GetOwnership() != grfx::OWNERSHIP_REFERENCE)) {
        GetDevice()->DestroyDepthStencilView(mDepthStencilView);
        mDepthStencilView.Reset();
    }

    if (mStorageImageView && (mStorageImageView->GetOwnership() != grfx::OWNERSHIP_REFERENCE)) {
        GetDevice()->DestroyStorageImageView(mStorageImageView);
        mStorageImageView.Reset();
    }

    if (mImage && (mImage->GetOwnership() != grfx::OWNERSHIP_REFERENCE)) {
        GetDevice()->DestroyImage(mImage);
        mImage.Reset();
    }
}

grfx::ImageType Texture::GetImageType() const
{
    return mImage->GetType();
}

uint32_t Texture::GetWidth() const
{
    return mImage->GetWidth();
}

uint32_t Texture::GetHeight() const
{
    return mImage->GetHeight();
}

uint32_t Texture::GetDepth() const
{
    return mImage->GetDepth();
}

grfx::Format Texture::GeImageFormat() const
{
    return mImage->GetFormat();
}

grfx::SampleCount Texture::GetSampleCount() const
{
    return mImage->GetSampleCount();
}

uint32_t Texture::GetMipLevelCount() const
{
    return mImage->GetMipLevelCount();
}

uint32_t Texture::GetArrayLayerCount() const
{
    return mImage->GetArrayLayerCount();
}

const grfx::ImageUsageFlags& Texture::GetUsageFlags() const
{
    return mImage->GetUsageFlags();
}

grfx::MemoryUsage Texture::GetMemoryUsage() const
{
    return mImage->GetMemoryUsage();
}

grfx::Format Texture::GetSampledImageViewFormat() const
{
    return mSampledImageView ? mSampledImageView->GetFormat() : grfx::FORMAT_UNDEFINED;
}

grfx::Format Texture::GetRenderTargetViewFormat() const
{
    return mRenderTargetView ? mRenderTargetView->GetFormat() : grfx::FORMAT_UNDEFINED;
}

grfx::Format Texture::GetDepthStencilViewFormat() const
{
    return mDepthStencilView ? mDepthStencilView->GetFormat() : grfx::FORMAT_UNDEFINED;
}

grfx::Format Texture::GetStorageImageViewFormat() const
{
    return mStorageImageView ? mStorageImageView->GetFormat() : grfx::FORMAT_UNDEFINED;
}

} // namespace grfx
} // namespace ppx
