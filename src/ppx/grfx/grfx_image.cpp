#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {

// -------------------------------------------------------------------------------------------------
// ImageCreateInfo
// -------------------------------------------------------------------------------------------------
ImageCreateInfo ImageCreateInfo::SampledImage2D(
    uint32_t          width,
    uint32_t          height,
    grfx::Format      format,
    grfx::SampleCount sampleCount,
    grfx::MemoryUsage memoryUsage)
{
    ImageCreateInfo ci         = {};
    ci.type                    = grfx::IMAGE_TYPE_2D;
    ci.width                   = width;
    ci.height                  = height;
    ci.depth                   = 1;
    ci.format                  = format;
    ci.sampleCount             = sampleCount;
    ci.mipLevelCount           = 1;
    ci.arrayLayerCount         = 1;
    ci.usageFlags.bits.sampled = true;
    ci.memoryUsage             = memoryUsage;
    ci.initialState            = grfx::RESOURCE_STATE_SHADER_RESOURCE;
    ci.pApiObject              = nullptr;
    return ci;
}

ImageCreateInfo ImageCreateInfo::DepthStencilTarget(
    uint32_t          width,
    uint32_t          height,
    grfx::Format      format,
    grfx::SampleCount sampleCount)
{
    ImageCreateInfo ci                        = {};
    ci.type                                   = grfx::IMAGE_TYPE_2D;
    ci.width                                  = width;
    ci.height                                 = height;
    ci.depth                                  = 1;
    ci.format                                 = format;
    ci.sampleCount                            = sampleCount;
    ci.mipLevelCount                          = 1;
    ci.arrayLayerCount                        = 1;
    ci.usageFlags.bits.sampled                = true;
    ci.usageFlags.bits.depthStencilAttachment = true;
    ci.memoryUsage                            = grfx::MEMORY_USAGE_GPU_ONLY;
    ci.initialState                           = grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE;
    ci.pApiObject                             = nullptr;
    return ci;
}

ImageCreateInfo ImageCreateInfo::RenderTarget2D(
    uint32_t          width,
    uint32_t          height,
    grfx::Format      format,
    grfx::SampleCount sampleCount)
{
    ImageCreateInfo ci                 = {};
    ci.type                            = grfx::IMAGE_TYPE_2D;
    ci.width                           = width;
    ci.height                          = height;
    ci.depth                           = 1;
    ci.format                          = format;
    ci.sampleCount                     = sampleCount;
    ci.mipLevelCount                   = 1;
    ci.arrayLayerCount                 = 1;
    ci.usageFlags.bits.sampled         = true;
    ci.usageFlags.bits.colorAttachment = true;
    ci.memoryUsage                     = grfx::MEMORY_USAGE_GPU_ONLY;
    ci.pApiObject                      = nullptr;
    return ci;
}

// -------------------------------------------------------------------------------------------------
// Image
// -------------------------------------------------------------------------------------------------
grfx::ImageViewType Image::GuessImageViewType(bool isCube) const
{
    const uint32_t arrayLayerCount = GetArrayLayerCount();

    grfx::ImageViewType result = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
    if (isCube) {
        return (arrayLayerCount > 0) ? grfx::IMAGE_VIEW_TYPE_CUBE_ARRAY : grfx::IMAGE_VIEW_TYPE_CUBE;
    }
    else {
        // clang-format off
        switch (mCreateInfo.type) {
            default: break;
            case grfx::IMAGE_TYPE_1D : return (arrayLayerCount > 1) ? grfx::IMAGE_VIEW_TYPE_1D_ARRAY : grfx::IMAGE_VIEW_TYPE_1D; break;
            case grfx::IMAGE_TYPE_2D : return (arrayLayerCount > 1) ? grfx::IMAGE_VIEW_TYPE_2D_ARRAY : grfx::IMAGE_VIEW_TYPE_2D; break;
        }
        // clang-format on
    }

    return grfx::IMAGE_VIEW_TYPE_UNDEFINED;
}

// -------------------------------------------------------------------------------------------------
// DepthStencilView
// -------------------------------------------------------------------------------------------------
grfx::DepthStencilViewCreateInfo DepthStencilViewCreateInfo::GuessFromImage(grfx::Image* pImage)
{
    grfx::DepthStencilViewCreateInfo ci = {};
    ci.pImage                           = pImage;
    ci.imageViewType                    = pImage->GuessImageViewType();
    ci.format                           = pImage->GetFormat();
    ci.mipLevel                         = 0;
    ci.mipLevelCount                    = 1;
    ci.arrayLayer                       = 0;
    ci.arrayLayerCount                  = 1;
    ci.components                       = {};
    ci.depthLoadOp                      = ATTACHMENT_LOAD_OP_LOAD;
    ci.depthStoreOp                     = ATTACHMENT_STORE_OP_STORE;
    ci.stencilLoadOp                    = ATTACHMENT_LOAD_OP_LOAD;
    ci.stencilStoreOp                   = ATTACHMENT_STORE_OP_STORE;
    ci.ownership                        = grfx::OWNERSHIP_REFERENCE;
    return ci;
}

// -------------------------------------------------------------------------------------------------
// SampledImageView
// -------------------------------------------------------------------------------------------------
grfx::RenderTargetViewCreateInfo RenderTargetViewCreateInfo::GuessFromImage(grfx::Image* pImage)
{
    grfx::RenderTargetViewCreateInfo ci = {};
    ci.pImage                           = pImage;
    ci.imageViewType                    = pImage->GuessImageViewType();
    ci.format                           = pImage->GetFormat();
    ci.sampleCount                      = pImage->GetSampleCount();
    ci.mipLevel                         = 0;
    ci.mipLevelCount                    = 1;
    ci.arrayLayer                       = 0;
    ci.arrayLayerCount                  = 1;
    ci.components                       = {};
    ci.loadOp                           = ATTACHMENT_LOAD_OP_LOAD;
    ci.storeOp                          = ATTACHMENT_STORE_OP_STORE;
    ci.ownership                        = grfx::OWNERSHIP_REFERENCE;
    return ci;
}

// -------------------------------------------------------------------------------------------------
// SampledImageView
// -------------------------------------------------------------------------------------------------
grfx::SampledImageViewCreateInfo SampledImageViewCreateInfo::GuessFromImage(grfx::Image* pImage)
{
    grfx::SampledImageViewCreateInfo ci = {};
    ci.pImage                           = pImage;
    ci.imageViewType                    = pImage->GuessImageViewType();
    ci.format                           = pImage->GetFormat();
    ci.sampleCount                      = pImage->GetSampleCount();
    ci.mipLevel                         = 0;
    ci.mipLevelCount                    = pImage->GetMipLevelCount();
    ci.arrayLayer                       = 0;
    ci.arrayLayerCount                  = pImage->GetArrayLayerCount();
    ci.components                       = {};
    ci.ownership                        = grfx::OWNERSHIP_REFERENCE;
    return ci;
}

// -------------------------------------------------------------------------------------------------
// StorageImageView
// -------------------------------------------------------------------------------------------------
grfx::StorageImageViewCreateInfo StorageImageViewCreateInfo::GuessFromImage(grfx::Image* pImage)
{
    grfx::StorageImageViewCreateInfo ci = {};
    ci.pImage                           = pImage;
    ci.imageViewType                    = pImage->GuessImageViewType();
    ci.format                           = pImage->GetFormat();
    ci.sampleCount                      = pImage->GetSampleCount();
    ci.mipLevel                         = 0;
    ci.mipLevelCount                    = pImage->GetMipLevelCount();
    ci.arrayLayer                       = 0;
    ci.arrayLayerCount                  = pImage->GetArrayLayerCount();
    ci.components                       = {};
    ci.ownership                        = grfx::OWNERSHIP_REFERENCE;
    return ci;
}

} // namespace grfx
} // namespace ppx
