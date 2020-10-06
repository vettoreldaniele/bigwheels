#ifndef ppx_grfx_texture_h
#define ppx_grfx_texture_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct TextureCreateInfo
//!
//!
struct TextureCreateInfo
{
    grfx::ImageType       type               = grfx::IMAGE_TYPE_2D;
    uint32_t              width              = 0;
    uint32_t              height             = 0;
    uint32_t              depth              = 0;
    grfx::Format          sampledImageFormat = grfx::FORMAT_UNDEFINED;
    grfx::Format          renderTargetFormat = grfx::FORMAT_UNDEFINED;
    grfx::Format          depthStencilFormat = grfx::FORMAT_UNDEFINED;
    grfx::SampleCount     sampleCount        = grfx::SAMPLE_COUNT_1;
    uint32_t              mipLevelCount      = 1;
    uint32_t              arrayLayerCount    = 1;
    grfx::ImageUsageFlags usageFlags         = grfx::ImageUsageFlags::SampledImage();
    grfx::MemoryUsage     memoryUsage        = grfx::MEMORY_USAGE_GPU_ONLY;
    void*                 pApiObject         = nullptr; // [OPTIONAL] For external images such as swapchain images
};

//! @class Texture
//!
//!
class Texture
    : public grfx::DeviceObject<grfx::TextureCreateInfo>
{
public:
    Texture() {}
    virtual ~Texture() {}

    grfx::ImageType              GetType() const { return mCreateInfo.type; }
    uint32_t                     GetWidth() const { return mCreateInfo.width; }
    uint32_t                     GetHeight() const { return mCreateInfo.height; }
    uint32_t                     GetDepth() const { return mCreateInfo.depth; }
    grfx::Format                 GetSampledImageFormat() const { return mCreateInfo.sampledImageFormat; }
    grfx::Format                 GetRenderTargetFormat() const { return mCreateInfo.renderTargetFormat; }
    grfx::Format                 GetDepthStencilFormat() const { return mCreateInfo.depthStencilFormat; }
    grfx::SampleCount            GetSampleCount() const { return mCreateInfo.sampleCount; }
    uint32_t                     GetMipLevelCount() const { return mCreateInfo.mipLevelCount; }
    uint32_t                     GetArrayLayerCount() const { return mCreateInfo.arrayLayerCount; }
    const grfx::ImageUsageFlags& GetUsageFlags() const { return mCreateInfo.usageFlags; }
    grfx::MemoryUsage            GetMemoryUsage() const { return mCreateInfo.memoryUsage; }

    grfx::ImagePtr            GetImage() const { return mImage; }
    grfx::SampledImageViewPtr GetSampledImageView() const { return mSampledImageView; }
    grfx::RenderTargetViewPtr GetRenderTargetView() const { return mRenderTargetView; }
    grfx::DepthStencilViewPtr GetDepthStencilView() const { return mDepthStencilView; }
    grfx::StorageImageViewPtr GetStorageImageView() const { return mStorageImageView; }

protected:
    virtual Result CreateApiObjects(const grfx::TextureCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    grfx::ImagePtr            mImage;
    grfx::SampledImageViewPtr mSampledImageView;
    grfx::RenderTargetViewPtr mRenderTargetView;
    grfx::DepthStencilViewPtr mDepthStencilView;
    grfx::StorageImageViewPtr mStorageImageView;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_texture_h
