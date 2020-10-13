#include "ppx/grfx/vk/vk_descriptor.h"
#include "ppx/grfx/vk/vk_buffer.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_image.h"

namespace ppx {
namespace grfx {
namespace vk {

// -------------------------------------------------------------------------------------------------
// DescriptorPool
// -------------------------------------------------------------------------------------------------
Result DescriptorPool::CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    // clang-format off
    if (pCreateInfo->sampler              > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLER               , pCreateInfo->sampler             });
    if (pCreateInfo->sampledImage         > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, pCreateInfo->sampledImage        });
    if (pCreateInfo->sampledImage         > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE         , pCreateInfo->sampledImage        });
    if (pCreateInfo->storageImage         > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE         , pCreateInfo->storageImage        });
    if (pCreateInfo->uniformTexelBuffer   > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER  , pCreateInfo->uniformTexelBuffer  });
    if (pCreateInfo->storageTexelBuffer   > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER  , pCreateInfo->storageTexelBuffer  });
    if (pCreateInfo->uniformBuffer        > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , pCreateInfo->uniformBuffer       });
    if (pCreateInfo->storageBuffer        > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER        , pCreateInfo->storageBuffer       });
    if (pCreateInfo->uniformBufferDynamic > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, pCreateInfo->uniformBufferDynamic});
    if (pCreateInfo->storageBufferDynamic > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, pCreateInfo->storageBufferDynamic});
    if (pCreateInfo->inputAttachment      > 0) poolSizes.push_back({VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT      , pCreateInfo->inputAttachment     });
    // clang-format on

    // Flags
    uint32_t flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    if (GetDevice()->GetApi() == grfx::API_VK_1_1) {
        flags |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    }
    else {
        flags |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    }

    VkDescriptorPoolCreateInfo vkci = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    vkci.flags                      = flags;
    vkci.maxSets                    = PPX_MAX_SETS_PER_POOL;
    vkci.poolSizeCount              = CountU32(poolSizes);
    vkci.pPoolSizes                 = DataPtr(poolSizes);

    VkResult vkres = vkCreateDescriptorPool(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mDescriptorPool);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateDescriptorPool failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void DescriptorPool::DestroyApiObjects()
{
    if (mDescriptorPool) {
        vkDestroyDescriptorPool(ToApi(GetDevice())->GetVkDevice(), mDescriptorPool, nullptr);
        mDescriptorPool.Reset();
    }
}

// -------------------------------------------------------------------------------------------------
// DescriptorSet
// -------------------------------------------------------------------------------------------------
Result DescriptorSet::CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo)
{
    mDescriptorPool = ToApi(pCreateInfo->pPool)->GetVkDescriptorPool();

    VkDescriptorSetAllocateInfo vkai = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    vkai.descriptorPool              = mDescriptorPool;
    vkai.descriptorSetCount          = 1;
    vkai.pSetLayouts                 = ToApi(pCreateInfo->pLayout)->GetVkDescriptorSetLayout();

    VkResult vkres = vkAllocateDescriptorSets(
        ToApi(GetDevice())->GetVkDevice(),
        &vkai,
        &mDescriptorSet);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkAllocateDescriptorSets failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    // Allocate 32 entries initially
    const uint32_t count = 32;
    mWriteStore.resize(count);
    mImageInfoStore.resize(count);
    mBufferInfoStore.resize(count);
    mTexelBufferStore.resize(count);

    return ppx::SUCCESS;
}

void DescriptorSet::DestroyApiObjects()
{
    if (mDescriptorSet) {
        vkFreeDescriptorSets(
            ToApi(GetDevice())->GetVkDevice(),
            mDescriptorPool,
            1,
            mDescriptorSet);

        mDescriptorSet.Reset();
    }

    if (mDescriptorPool) {
        mDescriptorPool.Reset();
    }
}

Result DescriptorSet::UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites)
{
    if (writeCount == 0) {
        return ppx::ERROR_UNEXPECTED_COUNT_VALUE;
    }

    if (CountU32(mWriteStore) < writeCount) {
        mWriteStore.resize(writeCount);
        mImageInfoStore.resize(writeCount);
        mBufferInfoStore.resize(writeCount);
        mTexelBufferStore.resize(writeCount);
    }

    mImageCount       = 0;
    mBufferCount      = 0;
    mTexelBufferCount = 0;
    for (mWriteCount = 0; mWriteCount < writeCount; ++mWriteCount) {
        const grfx::WriteDescriptor& srcWrite = pWrites[mWriteCount];

        VkDescriptorImageInfo*  pImageInfo       = nullptr;
        VkBufferView*           pTexelBufferView = nullptr;
        VkDescriptorBufferInfo* pBufferInfo      = nullptr;

        VkDescriptorType descriptorType = ToVkDescriptorType(srcWrite.type);
        switch (descriptorType) {
            default: {
                PPX_ASSERT_MSG(false, "unknown descriptor type: " << ToString(descriptorType) << "(" << descriptorType << ")");
                return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
            } break;

            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
                PPX_ASSERT_MSG(mImageCount < mImageInfoStore.size(), "image count exceeds image store capacity");
                pImageInfo = &mImageInfoStore[mImageCount];
                // Fill out info
                pImageInfo->sampler     = VK_NULL_HANDLE;
                pImageInfo->imageView   = VK_NULL_HANDLE;
                pImageInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                switch (descriptorType) {
                    default: break;
                    case VK_DESCRIPTOR_TYPE_SAMPLER: {
                        pImageInfo->sampler = ToApi(srcWrite.pSampler)->GetVkSampler();
                    } break;

                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
                        pImageInfo->sampler     = ToApi(srcWrite.pSampler)->GetVkSampler();
                        pImageInfo->imageView   = ToApi(srcWrite.pImageView->GetResourceView())->GetVkImageView();
                        pImageInfo->imageLayout = ToApi(srcWrite.pImageView->GetResourceView())->GetVkImageLayout();
                    } break;

                    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
                        pImageInfo->imageView   = ToApi(srcWrite.pImageView->GetResourceView())->GetVkImageView();
                        pImageInfo->imageLayout = ToApi(srcWrite.pImageView->GetResourceView())->GetVkImageLayout();
                    } break;
                }
                // Increment count
                mImageCount += 1;
            } break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
                PPX_ASSERT_MSG(false, "TEXEL BUFFER NOT IMPLEMENTED");
                PPX_ASSERT_MSG(mTexelBufferCount < mImageInfoStore.size(), "texel buffer count exceeds texel buffer store capacity");
                pTexelBufferView = &mTexelBufferStore[mTexelBufferCount];
                // Fill out info
                // Increment count
                mTexelBufferCount += 1;
            } break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
                PPX_ASSERT_MSG(mBufferCount < mBufferInfoStore.size(), "buffer count exceeds buffer store capacity");
                pBufferInfo = &mBufferInfoStore[mBufferCount];
                // Fill out info
                pBufferInfo->buffer = ToApi(srcWrite.pBuffer)->GetVkBuffer();
                pBufferInfo->offset = srcWrite.bufferOffset;
                pBufferInfo->range  = (srcWrite.bufferRange == PPX_WHOLE_SIZE) ? VK_WHOLE_SIZE : static_cast<VkDeviceSize>(srcWrite.bufferRange);
                // Increment count
                mBufferCount += 1;
            } break;
        }

        VkWriteDescriptorSet& vkWrite = mWriteStore[mWriteCount];
        vkWrite                       = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        vkWrite.dstSet                = mDescriptorSet;
        vkWrite.dstBinding            = srcWrite.binding;
        vkWrite.dstArrayElement       = 0;
        vkWrite.descriptorCount       = 1;
        vkWrite.descriptorType        = descriptorType;
        vkWrite.pImageInfo            = pImageInfo;
        vkWrite.pBufferInfo           = pBufferInfo;
        vkWrite.pTexelBufferView      = pTexelBufferView;
    }

    vkUpdateDescriptorSets(
        ToApi(GetDevice())->GetVkDevice(),
        mWriteCount,
        mWriteStore.data(),
        0,
        nullptr);

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------
// DescriptorSetLayout
// -------------------------------------------------------------------------------------------------
Result DescriptorSetLayout::CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo)
{
    std::vector<VkDescriptorSetLayoutBinding> vkBindings;
    for (size_t i = 0; i < pCreateInfo->bindings.size(); ++i) {
        //
        // NOTE: To keep D3D12 and Vulkan aligned, we do not support Vulkan's
        //       descriptor arrayness model. This means that the value for
        //       VkDescriptorSetLayoutBinding::descriptorCount is always 1.
        //
        //       If grfx::DescriptorBinding::arrayCount is greater than 1, we
        //       create that many entries. The binding number is incremented
        //       per entry starting from the initial binding value.
        //
        // NOTE: All bindings in a set use the same shader stage flags because
        //       we need to match D3D12's behavior. D3D12 controls shader 
        //       visibility at the root parameter level and not the binding level.
        //
        const grfx::DescriptorBinding& baseBinding = pCreateInfo->bindings[i];

        for (uint32_t bindingOffset = 0; bindingOffset < baseBinding.arrayCount; ++bindingOffset) {
            VkDescriptorSetLayoutBinding vkBinding = {};
            vkBinding.binding                      = baseBinding.binding + bindingOffset;
            vkBinding.descriptorType               = ToVkDescriptorType(baseBinding.type);
            vkBinding.descriptorCount              = 1;
            vkBinding.stageFlags                   = ToVkShaderStageFlags(pCreateInfo->shaderVisiblity);
            vkBinding.pImmutableSamplers           = nullptr;
            vkBindings.push_back(vkBinding);
        }
    }

    VkDescriptorSetLayoutCreateInfo vkci = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    vkci.bindingCount                    = CountU32(vkBindings);
    vkci.pBindings                       = DataPtr(vkBindings);

    VkResult vkres = vkCreateDescriptorSetLayout(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mDescriptorSetLayout);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateDescriptorSetLayout failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void DescriptorSetLayout::DestroyApiObjects()
{
    if (mDescriptorSetLayout) {
        vkDestroyDescriptorSetLayout(ToApi(GetDevice())->GetVkDevice(), mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout.Reset();
    }
}

} // namespace vk
} // namespace grfx
} // namespace ppx
