#ifndef ppx_grfx_image_h
#define ppx_grfx_image_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct ImageCreateInfo
//!
//!
struct ImageCreateInfo
{
    grfx::ImageType       type            = grfx::IMAGE_TYPE_2D;
    uint32_t              width           = 0;
    uint32_t              height          = 0;
    uint32_t              depth           = 0;
    grfx::Format          format          = grfx::FORMAT_UNDEFINED;
    grfx::SampleCount     sampleCount     = grfx::SAMPLE_COUNT_1;
    uint32_t              mipLevelCount   = 1;
    uint32_t              arrayLayerCount = 1;
    grfx::ImageUsageFlags usageFlags      = grfx::ImageUsageFlags::SampledImage();
    grfx::MemoryUsage     memoryUsage     = grfx::MEMORY_USAGE_GPU_ONLY;
    void*                 pApiObject      = nullptr; // [OPTIONAL] For external images such as swapchain images

    static ImageCreateInfo Image2D(uint32_t width, uint32_t height, grfx::Format, grfx::MemoryUsage = grfx::MEMORY_USAGE_GPU_ONLY);
};

//! @class Image
//!
//!
class Image
    : public grfx::DeviceObject<grfx::ImageCreateInfo>
{
public:
    Image() {}
    virtual ~Image() {}

    grfx::ImageType              GetType() const { return mCreateInfo.type; }
    uint32_t                     GetWidth() const { return mCreateInfo.width; }
    uint32_t                     GetHeight() const { return mCreateInfo.height; }
    uint32_t                     GetDepth() const { return mCreateInfo.depth; }
    grfx::Format                 GetFormat() const { return mCreateInfo.format; }
    grfx::SampleCount            GetSampleCount() const { return mCreateInfo.sampleCount; }
    uint32_t                     GetMipLevelCount() const { return mCreateInfo.mipLevelCount; }
    uint32_t                     GetArrayLayerCount() const { return mCreateInfo.arrayLayerCount; }
    const grfx::ImageUsageFlags& GetUsageFlags() const { return mCreateInfo.usageFlags; }
    grfx::MemoryUsage            GetMemoryUsage() const { return mCreateInfo.memoryUsage; }

    // Convenience functions
    grfx::ImageViewType GuessImageViewType(bool isCube = false) const;
};

// -------------------------------------------------------------------------------------------------

//! @struct DepthStencilViewCreateInfo
//!
//!
struct DepthStencilViewCreateInfo
{
    grfx::Image*            pImage          = nullptr;
    grfx::ImageViewType     type            = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
    grfx::Format            format          = grfx::FORMAT_UNDEFINED;
    uint32_t                mipLevel        = 0;
    uint32_t                mipLevelCount   = 0;
    uint32_t                arrayLayer      = 0;
    uint32_t                arrayLayerCount = 0;
    grfx::AttachmentLoadOp  depthLoadOp     = ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp depthStoreOp    = ATTACHMENT_STORE_OP_STORE;
    grfx::AttachmentLoadOp  stencilLoadOp   = ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp stencilStoreOp  = ATTACHMENT_STORE_OP_STORE;
};

//! @class DepthStencilView
//!
//!
class DepthStencilView
    : public grfx::DeviceObject<grfx::DepthStencilViewCreateInfo>
{
public:
    DepthStencilView() {}
    virtual ~DepthStencilView() {}

    grfx::ImagePtr          GetImage() const { return mCreateInfo.pImage; }
    grfx::Format            GetFormat() const { return mCreateInfo.format; }
    grfx::AttachmentLoadOp  GetDepthLoadOp() const { return mCreateInfo.depthLoadOp; }
    grfx::AttachmentStoreOp GetDepthStoreOp() const { return mCreateInfo.depthStoreOp; }
    grfx::AttachmentLoadOp  GetStencilLoadOp() const { return mCreateInfo.stencilLoadOp; }
    grfx::AttachmentStoreOp GetStencilStoreOp() const { return mCreateInfo.stencilStoreOp; }
};

// -------------------------------------------------------------------------------------------------

//! @struct RenderTargetViewCreateInfo
//!
//!
struct RenderTargetViewCreateInfo
{
    grfx::Image*            pImage          = nullptr;
    grfx::ImageViewType     imageViewType   = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
    grfx::Format            format          = grfx::FORMAT_UNDEFINED;
    grfx::SampleCount       sampleCount     = grfx::SAMPLE_COUNT_1;
    uint32_t                mipLevel        = 0;
    uint32_t                mipLevelCount   = 0;
    uint32_t                arrayLayer      = 0;
    uint32_t                arrayLayerCount = 0;
    grfx::AttachmentLoadOp  loadOp          = ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp storeOp         = ATTACHMENT_STORE_OP_STORE;
    grfx::ComponentMapping  components      = {};
};

//! @class RenderTargetView
//!
//!
class RenderTargetView
    : public grfx::DeviceObject<grfx::RenderTargetViewCreateInfo>
{
public:
    RenderTargetView() {}
    virtual ~RenderTargetView() {}

    grfx::ImagePtr          GetImage() const { return mCreateInfo.pImage; }
    grfx::Format            GetFormat() const { return mCreateInfo.format; }
    grfx::SampleCount       GetSampleCount() const { return mCreateInfo.sampleCount; }
    grfx::AttachmentLoadOp  GetLoadOp() const { return mCreateInfo.loadOp; }
    grfx::AttachmentStoreOp GetStoreOp() const { return mCreateInfo.storeOp; }
};

// -------------------------------------------------------------------------------------------------

//! @struct SampledImageViewCreateInfo
//!
//!
struct SampledImageViewCreateInfo
{
};

//! @class SampledImageView
//!
//!
class SampledImageView
    : public grfx::DeviceObject<grfx::SampledImageViewCreateInfo>
{
public:
    SampledImageView() {}
    virtual ~SampledImageView() {}
};

// -------------------------------------------------------------------------------------------------

//! @struct StorageImageViewCreateInfo
//!
//!
struct StorageImageViewCreateInfo
{
};

//! @class StorageImageView
//!
//!
class StorageImageView
    : public grfx::DeviceObject<grfx::StorageImageViewCreateInfo>
{
public:
    StorageImageView() {}
    virtual ~StorageImageView() {}
};
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_image_h
