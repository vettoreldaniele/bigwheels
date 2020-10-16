#ifndef ppx_grfx_vk_device_h
#define ppx_grfx_vk_device_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_device.h"

namespace ppx {
namespace grfx {
namespace vk {

class Device
    : public grfx::Device
{
public:
    Device() {}
    virtual ~Device() {}

    VkDevicePtr     GetVkDevice() const { return mDevice; }
    VmaAllocatorPtr GetVmaAllocator() const { return mVmaAllocator; }

    bool HasTimelineSemaphore() const { return mHasTimelineSemaphore; }
    bool HasExtendedDynamicState() const { return mHasExtendedDynamicState; }
    bool HasUnreistrictedDepthRange() const { return mHasUnrestrictedDepthRange; }

    virtual Result WaitIdle() override;

protected:
    virtual Result AllocateObject(grfx::Buffer** ppObject) override;
    virtual Result AllocateObject(grfx::CommandBuffer** ppObject) override;
    virtual Result AllocateObject(grfx::CommandPool** ppObject) override;
    virtual Result AllocateObject(grfx::ComputePipeline** ppObject) override;
    virtual Result AllocateObject(grfx::DepthStencilView** ppObject) override;
    virtual Result AllocateObject(grfx::DescriptorPool** ppObject) override;
    virtual Result AllocateObject(grfx::DescriptorSet** ppObject) override;
    virtual Result AllocateObject(grfx::DescriptorSetLayout** ppObject) override;
    virtual Result AllocateObject(grfx::Fence** ppObject) override;
    virtual Result AllocateObject(grfx::GraphicsPipeline** ppObject) override;
    virtual Result AllocateObject(grfx::Image** ppObject) override;
    virtual Result AllocateObject(grfx::PipelineInterface** ppObject) override;
    virtual Result AllocateObject(grfx::Queue** ppObject) override;
    virtual Result AllocateObject(grfx::RenderPass** ppObject) override;
    virtual Result AllocateObject(grfx::RenderTargetView** ppObject) override;
    virtual Result AllocateObject(grfx::SampledImageView** ppObject) override;
    virtual Result AllocateObject(grfx::Sampler** ppObject) override;
    virtual Result AllocateObject(grfx::Semaphore** ppObject) override;
    virtual Result AllocateObject(grfx::ShaderModule** ppObject) override;
    virtual Result AllocateObject(grfx::ShaderProgram** ppObject) override;
    virtual Result AllocateObject(grfx::Swapchain** ppObject) override;

protected:
    virtual Result CreateApiObjects(const grfx::DeviceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result ConfigureQueueInfo(const grfx::DeviceCreateInfo* pCreateInfo, std::vector<float>& queuePriorities, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos);
    Result ConfigureExtensions(const grfx::DeviceCreateInfo* pCreateInfo);
    Result ConfigurFeatures(const grfx::DeviceCreateInfo* pCreateInfo, VkPhysicalDeviceFeatures& features);
    Result CreateQueues(const grfx::DeviceCreateInfo* pCreateInfo);

private:
    std::vector<std::string> mFoundExtensions;
    std::vector<std::string> mExtensions;
    VkDevicePtr              mDevice;
    VmaAllocatorPtr          mVmaAllocator;
    bool                     mHasTimelineSemaphore    = false;
    bool                     mHasExtendedDynamicState = false;
    bool                     mHasUnrestrictedDepthRange = false;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_device_h