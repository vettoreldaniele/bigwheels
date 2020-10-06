#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {

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
// SampledImageView
// -------------------------------------------------------------------------------------------------
SampledImageViewCreateInfo SampledImageViewCreateInfo::GuessFromImage(grfx::Image* pImage)
{
    SampledImageViewCreateInfo ci = {};
    ci.pImage          = pImage;
    ci.imageViewType   = pImage->GuessImageViewType();
    ci.format          = pImage->GetFormat();
    ci.sampleCount     = pImage->GetSampleCount();
    ci.mipLevel        = 0;
    ci.mipLevelCount   = pImage->GetMipLevelCount();
    ci.arrayLayer      = 0;
    ci.arrayLayerCount = pImage->GetArrayLayerCount();
    ci.components      = {};
    return ci;
}

} // namespace grfx
} // namespace ppx
