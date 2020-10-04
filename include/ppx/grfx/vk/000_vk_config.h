#ifndef ppx_grfx_vk_config_h
#define ppx_grfx_vk_config_h

#include "ppx/grfx/000_grfx_config.h"
#include "ppx/grfx/vk/000_vk_config_platform.h"
#include "ppx/grfx/vk/vk_util.h"

namespace ppx {
namespace grfx {
namespace vk {

template <typename VkHandleT>
class VkHandlePtrRef
{
public:
    VkHandlePtrRef(VkHandleT* pHandle)
        : mHandlePtr(pHandle) {}

    ~VkHandlePtrRef() {}

    // clang-format off
    operator VkHandleT* ()
    {
        return mHandlePtr;
    }
    // clang-format on

private:
    VkHandleT* mHandlePtr = nullptr;
};

template <typename VkHandleT>
class VkHandlePtr
{
public:
    VkHandlePtr(const VkHandleT& handle = VK_NULL_HANDLE)
        : mHandle(handle) {}

    ~VkHandlePtr() {}

    VkHandlePtr& operator=(const VkHandleT& rhs)
    {
        mHandle = rhs;
        return *this;
    }

    operator bool() const
    {
        return mHandle != VK_NULL_HANDLE;
    }

    bool operator==(const VkHandlePtr& rhs) const
    {
        return mHandle == rhs.mHandle;
    }

    bool operator==(const VkHandleT& rhs) const
    {
        return mHandle == rhs;
    }

    VkHandleT Get() const
    {
        return mHandle;
    }

    void Reset()
    {
        mHandle = VK_NULL_HANDLE;
    }

    operator VkHandleT() const
    {
        return mHandle;
    }

    VkHandlePtrRef<VkHandleT> operator&()
    {
        return VkHandlePtrRef<VkHandleT>(&mHandle);
    }

    operator const VkHandleT*() const
    {
        const VkHandleT* ptr = &mHandle;
        return ptr;
    }

private:
    VkHandleT mHandle = VK_NULL_HANDLE;
};

// -------------------------------------------------------------------------------------------------

using VkBufferPtr              = VkHandlePtr<VkBuffer>;
using VkCommandBufferPtr       = VkHandlePtr<VkCommandBuffer>;
using VkCommandPoolPtr         = VkHandlePtr<VkCommandPool>;
using VkDebugUtilsMessengerPtr = VkHandlePtr<VkDebugUtilsMessengerEXT>;
using VkDescriptorPoolPtr      = VkHandlePtr<VkDescriptorPool>;
using VkDescriptorSetPtr       = VkHandlePtr<VkDescriptorSet>;
using VkDescriptorSetLayoutPtr = VkHandlePtr<VkDescriptorSetLayout>;
using VkDevicePtr              = VkHandlePtr<VkDevice>;
using VkFencePtr               = VkHandlePtr<VkFence>;
using VkFramebufferPtr         = VkHandlePtr<VkFramebuffer>;
using VkImagePtr               = VkHandlePtr<VkImage>;
using VkImageViewPtr           = VkHandlePtr<VkImageView>;
using VkInstancePtr            = VkHandlePtr<VkInstance>;
using VkPhysicalDevicePtr      = VkHandlePtr<VkPhysicalDevice>;
using VkPipelinePtr            = VkHandlePtr<VkPipeline>;
using VkPipelineLayoutPtr      = VkHandlePtr<VkPipelineLayout>;
using VkQueuePtr               = VkHandlePtr<VkQueue>;
using VkRenderPassPtr          = VkHandlePtr<VkRenderPass>;
using VkSemaphorePtr           = VkHandlePtr<VkSemaphore>;
using VkShaderModulePtr        = VkHandlePtr<VkShaderModule>;
using VkSurfacePtr             = VkHandlePtr<VkSurfaceKHR>;
using VkSwapchainPtr           = VkHandlePtr<VkSwapchainKHR>;

using VmaAllocationPtr = VkHandlePtr<VmaAllocation>;
using VmaAllocatorPtr  = VkHandlePtr<VmaAllocator>;

// -------------------------------------------------------------------------------------------------

class Buffer;
class CommandBuffer;
class CommandPool;
class DepthStencilView;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class Device;
class Fence;
class Gpu;
class GraphicsPipeline;
class Image;
class Instance;
class Pipeline;
class PipelineInterface;
class Queue;
class RenderPass;
class RenderTargetView;
class Semaphore;
class ShaderModule;
class Surface;
class Swapchain;

// -------------------------------------------------------------------------------------------------

template <typename GrfxTypeT>
struct ApiObjectLookUp
{
};

template <>
struct ApiObjectLookUp<grfx::Buffer>
{
    using GrfxType = grfx::Buffer;
    using ApiType  = vk::Buffer;
};

template <>
struct ApiObjectLookUp<grfx::CommandBuffer>
{
    using GrfxType = grfx::CommandBuffer;
    using ApiType  = vk::CommandBuffer;
};

template <>
struct ApiObjectLookUp<grfx::CommandPool>
{
    using GrfxType = grfx::CommandPool;
    using ApiType  = vk::CommandPool;
};

template <>
struct ApiObjectLookUp<grfx::DepthStencilView>
{
    using GrfxType = grfx::DepthStencilView;
    using ApiType  = vk::DepthStencilView;
};

template <>
struct ApiObjectLookUp<grfx::Device>
{
    using GrfxType = grfx::Device;
    using ApiType  = vk::Device;
};

template <>
struct ApiObjectLookUp<grfx::Fence>
{
    using GrfxType = grfx::Fence;
    using ApiType  = vk::Fence;
};

template <>
struct ApiObjectLookUp<grfx::GraphicsPipeline>
{
    using GrfxType = grfx::GraphicsPipeline;
    using ApiType  = vk::GraphicsPipeline;
};

template <>
struct ApiObjectLookUp<grfx::Image>
{
    using GrfxType = grfx::Image;
    using ApiType  = vk::Image;
};

template <>
struct ApiObjectLookUp<grfx::Instance>
{
    using GrfxType = grfx::Instance;
    using ApiType  = Instance;
};

template <>
struct ApiObjectLookUp<grfx::Gpu>
{
    using GrfxType = grfx::Gpu;
    using ApiType  = vk::Gpu;
};

template <>
struct ApiObjectLookUp<grfx::Queue>
{
    using GrfxType = grfx::Queue;
    using ApiType  = vk::Queue;
};

template <>
struct ApiObjectLookUp<grfx::PipelineInterface>
{
    using GrfxType = grfx::PipelineInterface;
    using ApiType  = vk::PipelineInterface;
};

template <>
struct ApiObjectLookUp<grfx::RenderPass>
{
    using GrfxType = grfx::RenderPass;
    using ApiType  = vk::RenderPass;
};

template <>
struct ApiObjectLookUp<grfx::RenderTargetView>
{
    using GrfxType = grfx::RenderTargetView;
    using ApiType  = vk::RenderTargetView;
};

template <>
struct ApiObjectLookUp<grfx::Semaphore>
{
    using GrfxType = grfx::Semaphore;
    using ApiType  = vk::Semaphore;
};

template <>
struct ApiObjectLookUp<grfx::ShaderModule>
{
    using GrfxType = grfx::ShaderModule;
    using ApiType  = vk::ShaderModule;
};

template <>
struct ApiObjectLookUp<grfx::Surface>
{
    using GrfxType = grfx::Surface;
    using ApiType  = vk::Surface;
};

template <>
struct ApiObjectLookUp<grfx::Swapchain>
{
    using GrfxType = grfx::Swapchain;
    using ApiType  = vk::Swapchain;
};

template <typename GrfxTypeT>
typename ApiObjectLookUp<GrfxTypeT>::ApiType* ToApi(GrfxTypeT* pGrfxObject)
{
    using ApiType       = typename ApiObjectLookUp<GrfxTypeT>::ApiType;
    ApiType* pApiObject = static_cast<ApiType*>(pGrfxObject);
    return pApiObject;
}

template <typename GrfxTypeT>
const typename ApiObjectLookUp<GrfxTypeT>::ApiType* ToApi(const GrfxTypeT* pGrfxObject)
{
    using ApiType             = typename ApiObjectLookUp<GrfxTypeT>::ApiType;
    const ApiType* pApiObject = static_cast<const ApiType*>(pGrfxObject);
    return pApiObject;
}

// -------------------------------------------------------------------------------------------------

const uint32_t kAllQueueMask      = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
const uint32_t kGraphicsQueueMask = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
const uint32_t kComputeQueueMask  = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
const uint32_t kTransferQueueMask = VK_QUEUE_TRANSFER_BIT;

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_config_h
