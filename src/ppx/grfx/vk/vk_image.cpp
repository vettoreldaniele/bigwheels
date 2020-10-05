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
        // Create image
        {
            VkExtent3D extent = {};
            extent.width      = pCreateInfo->width;
            extent.height     = pCreateInfo->height;
            extent.depth      = pCreateInfo->depth;

            VkImageCreateFlags createFlags =
                VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT |
                VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT |
                VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT |
                VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;

            VkImageCreateInfo vkci     = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            vkci.flags                 = createFlags;
            vkci.imageType             = ToVkImageType(pCreateInfo->type);
            vkci.format                = ToVkFormat(pCreateInfo->format);
            vkci.extent                = extent;
            vkci.mipLevels             = pCreateInfo->mipLevelCount;
            vkci.arrayLayers           = pCreateInfo->arrayLayerCount;
            vkci.samples               = ToVkSampleCount(pCreateInfo->sampleCount);
            vkci.tiling                = VK_IMAGE_TILING_OPTIMAL;
            vkci.usage                 = ToVkImageUsageFlags(pCreateInfo->usageFlags);
            vkci.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            vkci.queueFamilyIndexCount = 0;
            vkci.pQueueFamilyIndices   = nullptr;
            vkci.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

            VkAllocationCallbacks* pAllocator = nullptr;

            VkResult vkres = vkCreateImage(ToApi(GetDevice())->GetVkDevice(), &vkci, pAllocator, &mImage);
            if (vkres != VK_SUCCESS) {
                PPX_ASSERT_MSG(false, "vkCreateImage failed: " << ToString(vkres));
                return ppx::ERROR_API_FAILURE;
            }
        }

        // Allocate memory
        {
            VmaMemoryUsage memoryUsage = ToVmaMemoryUsage(pCreateInfo->memoryUsage);
            if (memoryUsage == VMA_MEMORY_USAGE_UNKNOWN) {
                PPX_ASSERT_MSG(false, "unknown memory usage");
                return ppx::ERROR_API_FAILURE;
            }

            VmaAllocationCreateFlags createFlags = 0;

            if ((memoryUsage == VMA_MEMORY_USAGE_CPU_ONLY) || (memoryUsage == VMA_MEMORY_USAGE_CPU_ONLY)) {
                createFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
            }

            VmaAllocationCreateInfo vma_alloc_ci = {};
            vma_alloc_ci.flags                   = createFlags;
            vma_alloc_ci.usage                   = memoryUsage;
            vma_alloc_ci.requiredFlags           = 0;
            vma_alloc_ci.preferredFlags          = 0;
            vma_alloc_ci.memoryTypeBits          = 0;
            vma_alloc_ci.pool                    = VK_NULL_HANDLE;
            vma_alloc_ci.pUserData               = nullptr;

            VkResult vkres = vmaAllocateMemoryForImage(
                ToApi(GetDevice())->GetVmaAllocator(),
                mImage,
                &vma_alloc_ci,
                &mAllocation,
                &mAllocationInfo);
            if (vkres != VK_SUCCESS) {
                PPX_ASSERT_MSG(false, "vmaAllocateMemoryForImage failed: " << ToString(vkres));
                return ppx::ERROR_API_FAILURE;
            }
        }

        // Bind memory
        {
            VkResult vkres = vmaBindImageMemory(
                ToApi(GetDevice())->GetVmaAllocator(),
                mAllocation,
                mImage);
            if (vkres != VK_SUCCESS) {
                PPX_ASSERT_MSG(false, "vmaBindImageMemory failed: " << ToString(vkres));
                return ppx::ERROR_API_FAILURE;
            }
        }
    }
    else {
        mImage = static_cast<VkImage>(pCreateInfo->pApiObject);
    }

    mVkFormat    = ToVkFormat(pCreateInfo->format);
    mImageAspect = DetermineAspectMask(mVkFormat);

    return ppx::SUCCESS;
}

void Image::DestroyApiObjects()
{
    // Don't destroy image unless we created it
    if (!IsNull(mCreateInfo.pApiObject)) {
        return;
    }

    if (mAllocation) {
        vmaFreeMemory(ToApi(GetDevice())->GetVmaAllocator(), mAllocation);
        mAllocation.Reset();

        mAllocationInfo = {};
    }

    if (mImage) {
        vkDestroyImage(ToApi(GetDevice())->GetVkDevice(), mImage, nullptr);
        mImage.Reset();
    }
}

// -------------------------------------------------------------------------------------------------
// Sampler
// -------------------------------------------------------------------------------------------------
Result Sampler::CreateApiObjects(const grfx::SamplerCreateInfo* pCreateInfo)
{
    VkSamplerCreateInfo vkci     = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    //vkci.flags                   = 0;
    //vkci.magFilter               = ;
    //vkci.minFilter               = ;
    //vkci.mipmapMode              = ;
    //vkci.addressModeU            = ;
    //vkci.addressModeV            = ;
    //vkci.addressModeW            = ;
    //vkci.mipLodBias              = ;
    //vkci.anisotropyEnable        = ;
    //vkci.maxAnisotropy           = ;
    //vkci.compareEnable           = ;
    //vkci.compareOp               = ;
    //vkci.minLod                  = ;
    //vkci.maxLod                  = ;
    //vkci.borderColor             = ;
    //vkci.unnormalizedCoordinates = ;

    VkResult vkres = vkCreateSampler(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mSampler);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateSampler failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void Sampler::DestroyApiObjects()
{
    if (mSampler) {
        vkDestroySampler(ToApi(GetDevice())->GetVkDevice(), mSampler, nullptr);
        mSampler.Reset();
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

    std::unique_ptr<grfx::internal::ImageResourceView> resourceView(new vk::internal::ImageResourceView(mImageView, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    if (!resourceView) {
        PPX_ASSERT_MSG(false, "vk::internal::ImageResourceView allocation failed");
        return ppx::ERROR_ALLOCATION_FAILED;
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
