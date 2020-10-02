#include "ppx/grfx/vk/vk_image.h"
#include "ppx/grfx/vk/vk_device.h"

namespace ppx {
namespace grfx {
namespace vk {

// -------------------------------------------------------------------------------------------------
// Image
// -------------------------------------------------------------------------------------------------
Result Image::CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo)
{
    if (IsNull(pCreateInfo->pApiObject)) {
        return ppx::ERROR_FAILED;
    }
    else {
        mImage = static_cast<VkImage>(pCreateInfo->pApiObject);
    }

    mFormat = ToVkFormat(pCreateInfo->format);
    mImageAspect = DetermineAspectMask(mFormat);

    return ppx::SUCCESS;
}

void Image::DestroyApiObjects()
{
    // Don't destroy image unless we created it
    if (!IsNull(mCreateInfo.pApiObject)) {
        return;
    }
}

// -------------------------------------------------------------------------------------------------
// DepthStencilView
// -------------------------------------------------------------------------------------------------
Result DepthStencilView::CreateApiObjects(const grfx::DepthStencilViewCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void DepthStencilView::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// RenderTargetView
// -------------------------------------------------------------------------------------------------
Result RenderTargetView::CreateApiObjects(const grfx::RenderTargetViewCreateInfo* pCreateInfo)
{
    VkImageViewCreateInfo vkci           = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    vkci.flags                           = 0;
    vkci.image                           = ToApi(pCreateInfo->pImage)->GetVkImage();
    vkci.viewType                        = ToVkImageViewType(pCreateInfo->imageViewType);
    vkci.format                          = ToVkFormat(pCreateInfo->format);
    vkci.components                      = ToVkComponentMapping(pCreateInfo->components);
    vkci.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    vkci.subresourceRange.baseMipLevel   = pCreateInfo->mipLevel;
    vkci.subresourceRange.levelCount     = pCreateInfo->mipLevelCount;
    vkci.subresourceRange.baseArrayLayer = pCreateInfo->arrayLayer;
    vkci.subresourceRange.layerCount     = pCreateInfo->arrayLayerCount;

    VkResult vkres = vkCreateImageView(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mImageView);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateImageView failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void RenderTargetView::DestroyApiObjects()
{
    if (mImageView) {
        vkDestroyImageView(
            ToApi(GetDevice())->GetVkDevice(),
            mImageView,
            nullptr);
        mImageView.Reset();
    }
}

} // namespace vk
} // namespace grfx
} // namespace ppx
