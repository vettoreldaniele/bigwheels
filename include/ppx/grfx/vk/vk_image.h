#ifndef ppx_grfx_vk_image_h
#define ppx_grfx_vk_image_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {
namespace vk {

class Image
    : public grfx::Image
{
public:
    Image() {}
    virtual ~Image() {}

    VkImagePtr         GetVkImage() const { return mImage; }
    VkFormat           GetVkFormat() const { return mVkFormat; }
    VkImageAspectFlags GetVkImageAspectFlags() const { return mImageAspect; }

protected:
    virtual Result CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkImagePtr         mImage;
    VmaAllocationPtr   mAllocation;
    VmaAllocationInfo  mAllocationInfo = {};
    VkFormat           mVkFormat       = VK_FORMAT_UNDEFINED;
    VkImageAspectFlags mImageAspect    = ppx::InvalidValue<VkImageAspectFlags>();
};

// -------------------------------------------------------------------------------------------------

namespace internal {

class ImageResourceView
    : public grfx::internal::ImageResourceView
{
public:
    ImageResourceView(VkImageViewPtr vkImageView, VkImageLayout layout)
        : mImageView(vkImageView), mImageLayout(layout) {}
    virtual ~ImageResourceView() {}

    VkImageViewPtr GetVkImageView() const { return mImageView; }
    VkImageLayout  GetVkImageLayout() const { return mImageLayout; }

private:
    VkImageViewPtr mImageView;
    VkImageLayout  mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

} // namespace internal

// -------------------------------------------------------------------------------------------------

class Sampler
    : public grfx::Sampler
{
public:
    Sampler() {}
    virtual ~Sampler() {}

    VkSamplerPtr GetVkSampler() const { return mSampler; }

protected:
    virtual Result CreateApiObjects(const grfx::SamplerCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkSamplerPtr mSampler;
};

// -------------------------------------------------------------------------------------------------

class DepthStencilView
    : public grfx::DepthStencilView
{
public:
    DepthStencilView() {}
    virtual ~DepthStencilView() {}

    VkImageViewPtr GetVkImageView() const { return mImageView; }

protected:
    virtual Result CreateApiObjects(const grfx::DepthStencilViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkImageViewPtr mImageView;
};

// -------------------------------------------------------------------------------------------------

class RenderTargetView
    : public grfx::RenderTargetView
{
public:
    RenderTargetView() {}
    virtual ~RenderTargetView() {}

    VkImageViewPtr GetVkImageView() const { return mImageView; }

protected:
    virtual Result CreateApiObjects(const grfx::RenderTargetViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkImageViewPtr mImageView;
};

// -------------------------------------------------------------------------------------------------

class SampledImageView
    : public grfx::SampledImageView
{
public:
    SampledImageView() {}
    virtual ~SampledImageView() {}

    VkImageViewPtr GetVkImageView() const { return mImageView; }

protected:
    virtual Result CreateApiObjects(const grfx::SampledImageViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkImageViewPtr mImageView;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_image_h
