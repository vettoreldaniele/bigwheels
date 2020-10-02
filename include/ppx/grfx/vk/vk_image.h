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
    VkFormat           GetVkFormat() const { return mFormat; }
    VkImageAspectFlags GetVkImageAspectFlags() const { return mImageAspect; }

protected:
    virtual Result CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkImagePtr         mImage;
    VkFormat           mFormat      = VK_FORMAT_UNDEFINED;
    VkImageAspectFlags mImageAspect = InvalidValue<VkImageAspectFlags>();
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

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_image_h
