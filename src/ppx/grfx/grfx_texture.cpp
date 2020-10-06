#include "ppx/grfx/grfx_texture.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {

Result Texture::CreateApiObjects(const grfx::TextureCreateInfo* pCreateInfo)
{
    // Image
    {
        grfx::ImageCreateInfo ci = {};
        ci.type                  = pCreateInfo->type;
        ci.width                 = pCreateInfo->width;
        ci.height                = pCreateInfo->height;
        ci.depth                 = pCreateInfo->depth;
        ci.format                = pCreateInfo->sampledImageFormat;
        ci.sampleCount           = pCreateInfo->sampleCount;
        ci.mipLevelCount         = pCreateInfo->mipLevelCount;
        ci.arrayLayerCount       = pCreateInfo->arrayLayerCount;
        ci.usageFlags            = pCreateInfo->usageFlags;
        ci.memoryUsage           = pCreateInfo->memoryUsage;
        ci.pApiObject            = pCreateInfo->pApiObject;

        Result ppxres = GetDevice()->CreateImage(&ci, &mImage);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    if (pCreateInfo->usageFlags.bits.colorAttachment) {
    }

    if (pCreateInfo->usageFlags.bits.depthStencilAttachment) {
    }

    if (pCreateInfo->usageFlags.bits.storage) {
    }

    return ppx::SUCCESS;
}

void Texture::DestroyApiObjects()
{
    if (mSampledImageView) {
    }

    if (mRenderTargetView) {
        GetDevice()->DestroyRenderTargetView(mRenderTargetView);
    }

    if (mDepthStencilView) {
        GetDevice()->DestroyDepthStencilView(mDepthStencilView);
    }

    if (mStorageImageView) {
    }

    if (mImage) {
        GetDevice()->DestroyImage(mImage);
    }
}

} // namespace grfx
} // namespace ppx
