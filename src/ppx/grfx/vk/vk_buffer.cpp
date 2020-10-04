#include "ppx/grfx/vk/vk_buffer.h"
#include "ppx/grfx/vk/vk_device.h"

namespace ppx {
namespace grfx {
namespace vk {

Result Buffer::CreateApiObjects(const grfx::BufferCreateInfo* pCreateInfo)
{
    vk::Device* pDevice = ToApi(GetDevice());

    // Align buffer size in case we want to use dynamic uniform buffers
    // later. Dynamic uniform buffers requires a specific size alignment.
    // VK_CONSTANT_BUFFER_ALIGNMENT is selected to work across the widest
    // set of GPUs.
    //
    VkDeviceSize alignedSize = static_cast<VkDeviceSize>(pCreateInfo->size);
    if (pCreateInfo->usageFlags.bits.uniformBuffer) {
        alignedSize = RoundUp<VkDeviceSize>(pCreateInfo->size, VK_CONSTANT_BUFFER_ALIGNMENT);
    }

    VkBufferCreateInfo createInfo    = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    createInfo.flags                 = 0;
    createInfo.size                  = alignedSize;
    createInfo.usage                 = ToVkBufferUsageFlags(pCreateInfo->usageFlags);
    createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices   = nullptr;

    VkAllocationCallbacks* pAllocator = nullptr;

    VkResult vkres = vkCreateBuffer(ToApi(GetDevice())->GetVkDevice(), &createInfo, pAllocator, &mBuffer);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateBuffer failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
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

        VkResult vkres = vmaAllocateMemoryForBuffer(
            pDevice->GetVmaAllocator(),
            mBuffer,
            &vma_alloc_ci,
            &mAllocation,
            &mAllocationInfo);
        if (vkres != VK_SUCCESS) {
            PPX_ASSERT_MSG(false, "vmaAllocateMemoryForBuffer failed: " << ToString(vkres));
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Bind memory
    {
        VkResult vkres = vmaBindBufferMemory(
            pDevice->GetVmaAllocator(),
            mAllocation,
            mBuffer);
        if (vkres != VK_SUCCESS) {
            PPX_ASSERT_MSG(false, "vmaBindBufferMemory failed: " << ToString(vkres));
            return ppx::ERROR_API_FAILURE;
        }
    }

    return ppx::SUCCESS;
}

void Buffer::DestroyApiObjects()
{
    if (mAllocation) {
        vmaFreeMemory(ToApi(GetDevice())->GetVmaAllocator(), mAllocation);
        mAllocation.Reset();

        mAllocationInfo = {};
    }

    if (mBuffer) {
        vkDestroyBuffer(ToApi(GetDevice())->GetVkDevice(), mBuffer, nullptr);
        mBuffer.Reset();
    }
}

Result Buffer::MapMemory(uint64_t offset, void** ppMappedAddress)
{
    if (IsNull(ppMappedAddress)) {
        return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
    }

    VkResult vkres = vmaMapMemory(
        ToApi(GetDevice())->GetVmaAllocator(),
        mAllocation,
        ppMappedAddress);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vmaMapMemory failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void Buffer::UnmapMemory()
{
    vmaUnmapMemory(
        ToApi(GetDevice())->GetVmaAllocator(),
        mAllocation);
}

} // namespace vk
} // namespace grfx
} // namespace ppx
