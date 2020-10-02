#ifndef ppx_grfx_device_h
#define ppx_grfx_device_h

#include "ppx/grfx/000_grfx_config.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_descriptor.h"
#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_pipeline.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_render_pass.h"
#include "ppx/grfx/grfx_shader.h"
#include "ppx/grfx/grfx_swapchain.h"
#include "ppx/grfx/grfx_sync.h"

namespace ppx {
namespace grfx {

//! @struct DeviceCreateInfo
//!
//!
struct DeviceCreateInfo
{
    grfx::Gpu*               pGpu                  = nullptr;
    uint32_t                 graphicsQueueCount    = 0;
    uint32_t                 computeQueueCount     = 0;
    uint32_t                 transferQueueCount    = 0;
    std::vector<std::string> vulkanExtensions      = {};      // [OPTIONAL] Additional device extensions
    const void*              pVulkanDeviceFeatures = nullptr; // [OPTIONAL] Pointer to custom VkPhysicalDeviceFeatures
};

//! @class Device
//!
//!
class Device
    : public grfx::InstanceObject<grfx::DeviceCreateInfo>
{
public:
    Device() {}
    virtual ~Device() {}

    grfx::GpuPtr GetGpu() const { return mCreateInfo.pGpu; }

    const char*    GetDeviceName() const;
    grfx::VendorId GetDeviceVendorId() const;

    Result CreateBuffer(const grfx::BufferCreateInfo* pCreateInfo, grfx::Buffer** ppBuffer);
    void   DestroyBuffer(const grfx::Buffer* pBuffer);

    Result CreateCommandPool(const grfx::CommandPoolCreateInfo* pCreateInfo, grfx::CommandPool** ppCommandPool);
    void   DestroyCommandPool(const grfx::CommandPool* pCommandPool);

    Result CreateComputePipeline(const grfx::ComputePipelineCreateInfo* pCreateInfo, grfx::ComputePipeline** ppComputePipeline);
    void   DestroyComputePipeline(const grfx::ComputePipeline* pComputePipeline);

    Result CreateDepthStencilView(const grfx::DepthStencilViewCreateInfo* pCreateInfo, grfx::DepthStencilView** ppDepthStencilView);
    void   DestroyDepthStencilView(const grfx::DepthStencilView* pDepthStencilView);

    Result CreateDescriptorPool(const grfx::DescriptorPoolCreateInfo* pCreateInfo, grfx::DescriptorPool** ppDescriptorPool);
    void   DestroyDescriptorPool(const grfx::DescriptorPool* pDescriptorPool);

    Result CreateFence(const grfx::FenceCreateInfo* pCreateInfo, grfx::Fence** ppFence);
    void   DestroyFence(const grfx::Fence* pFence);

    Result CreateGraphicsPipeline(const grfx::GraphicsPipelineCreateInfo* pCreateInfo, grfx::GraphicsPipeline** ppGraphicsPipeline);
    void   DestroyGraphicsPipeline(const grfx::GraphicsPipeline* pGraphicsPipeline);

    Result CreateImage(const grfx::ImageCreateInfo* pCreateInfo, grfx::Image** ppImage);
    void   DestroyImage(const grfx::Image* pImage);

    Result CreatePipelineLayout(const grfx::PipelineLayoutCreateInfo* pCreateInfo, grfx::PipelineLayout** ppPipelineLayout);
    void   DestroyPipelineLayout(const grfx::PipelineLayout* pPipelineLayout);

    Result CreateRenderPass(const grfx::RenderPassCreateInfo* pCreateInfo, grfx::RenderPass** ppRenderPass);
    Result CreateRenderPass(const grfx::RenderPassCreateInfo2* pCreateInfo, grfx::RenderPass** ppRenderPass);
    Result CreateRenderPass(const grfx::RenderPassCreateInfo3* pCreateInfo, grfx::RenderPass** ppRenderPass);
    void   DestroyRenderPass(const grfx::RenderPass* pRenderPass);

    Result CreateRenderTargetView(const grfx::RenderTargetViewCreateInfo* pCreateInfo, grfx::RenderTargetView** ppRenderTargetView);
    void   DestroyRenderTargetView(const grfx::RenderTargetView* pRenderTargetView);

    Result CreateSemaphore(const grfx::SemaphoreCreateInfo* pCreateInfo, grfx::Semaphore** ppSemaphore);
    void   DestroySemaphore(const grfx::Semaphore* pSemaphore);

    Result CreateShaderModule(const grfx::ShaderModuleCreateInfo* pCreateInfo, grfx::ShaderModule** ppShaderModule);
    void   DestroyShaderModule(const grfx::ShaderModule* pShaderModule);

    Result CreateShaderProgram(const grfx::ShaderProgramCreateInfo* pCreateInfo, grfx::ShaderProgram** ppShaderProgram);
    void   DestroyShaderProgram(const grfx::ShaderProgram* pShaderProgram);

    Result CreateSwapchain(const grfx::SwapchainCreateInfo* pCreateInfo, grfx::Swapchain** ppSwapchain);
    void   DestroySwapchain(const grfx::Swapchain* pSwapchain);

    Result AllocateCommandBuffer(const grfx::CommandPool* pPool, grfx::CommandBuffer** ppCommandBuffer);
    void   FreeCommandBuffer(const grfx::CommandBuffer* pCommandBuffer);

    Result AllocateDescriptorSet(const grfx::DescriptorPool* pPool, const grfx::DescriptorSetLayout* pLayout, grfx::DescriptorSet** ppSet);
    void   FreeDescriptorSet(const grfx::DescriptorSet* pSet);

    uint32_t       GetGraphicsQueueCount() const;
    Result         GetGraphicsQueue(uint32_t index, grfx::Queue** ppQueue) const;
    grfx::QueuePtr GetGraphicsQueue(uint32_t index = 0) const;

    uint32_t       GetComputeQueueCount() const;
    Result         GetComputeQueue(uint32_t index, grfx::Queue** ppQueue) const;
    grfx::QueuePtr GetComputeQueue(uint32_t index = 0) const;

    uint32_t       GetTransferQueueCount() const;
    Result         GetTransferQueue(uint32_t index, grfx::Queue** ppQueue) const;
    grfx::QueuePtr GetTransferQueue(uint32_t index = 0) const;

    grfx::QueuePtr GetAnyAvailableQueue() const;

    virtual Result WaitIdle() = 0;

protected:
    virtual Result Create(const grfx::DeviceCreateInfo* pCreateInfo) override;
    virtual void   Destroy() override;
    friend class grfx::Instance;

    virtual Result AllocateObject(grfx::Buffer** ppObject)           = 0;
    virtual Result AllocateObject(grfx::CommandBuffer** ppObject)    = 0;
    virtual Result AllocateObject(grfx::CommandPool** ppObject)      = 0;
    virtual Result AllocateObject(grfx::ComputePipeline** ppObject)  = 0;
    virtual Result AllocateObject(grfx::DepthStencilView** ppObject) = 0;
    virtual Result AllocateObject(grfx::DescriptorPool** ppObject)   = 0;
    virtual Result AllocateObject(grfx::DescriptorSet** ppObject)    = 0;
    virtual Result AllocateObject(grfx::Fence** ppObject)            = 0;
    virtual Result AllocateObject(grfx::GraphicsPipeline** ppObject) = 0;
    virtual Result AllocateObject(grfx::Image** ppObject)            = 0;
    virtual Result AllocateObject(grfx::PipelineLayout** ppObject)   = 0;
    virtual Result AllocateObject(grfx::Queue** ppObject)            = 0;
    virtual Result AllocateObject(grfx::RenderPass** ppObject)       = 0;
    virtual Result AllocateObject(grfx::RenderTargetView** ppObject) = 0;
    virtual Result AllocateObject(grfx::Semaphore** ppObject)        = 0;
    virtual Result AllocateObject(grfx::ShaderModule** ppObject)     = 0;
    virtual Result AllocateObject(grfx::ShaderProgram** ppObject)    = 0;
    virtual Result AllocateObject(grfx::Swapchain** ppObject)        = 0;

    template <
        typename ObjectT,
        typename CreateInfoT,
        typename ContainerT = std::vector<ObjPtr<ObjectT>>>
    Result CreateObject(const CreateInfoT* pCreateInfo, ContainerT& container, ObjectT** ppObject);

    template <
        typename ObjectT,
        typename ContainerT = std::vector<ObjPtr<ObjectT>>>
    void DestroyObject(ContainerT& container, const ObjectT* pObject);

    template <typename ObjectT>
    void DestroyAllObjects(std::vector<ObjPtr<ObjectT>>& container);

    Result CreateGraphicsQueue(const grfx::internal::QueueCreateInfo* pCreateInfo, grfx::Queue** ppQueue);
    Result CreateComputeQueue(const grfx::internal::QueueCreateInfo* pCreateInfo, grfx::Queue** ppQueue);
    Result CreateTransferQueue(const grfx::internal::QueueCreateInfo* pCreateInfo, grfx::Queue** ppQueue);

protected:
    grfx::InstancePtr                         mInstance;
    std::vector<grfx::BufferPtr>              mBuffers;
    std::vector<grfx::CommandBufferPtr>       mCommandBuffer;
    std::vector<grfx::CommandPoolPtr>         mCommandPools;
    std::vector<grfx::ComputePipelinePtr>     mComputePipelines;
    std::vector<grfx::DepthStencilViewPtr>    mDepthStencilViews;
    std::vector<grfx::DescriptorPoolPtr>      mDescriptorPools;
    std::vector<grfx::DescriptorSetPtr>       mDescriptorSets;
    std::vector<grfx::DescriptorSetLayoutPtr> mDescriptorSetLayouts;
    std::vector<grfx::FencePtr>               mFences;
    std::vector<grfx::GraphicsPipelinePtr>    mGraphicsPipelines;
    std::vector<grfx::ImagePtr>               mImages;
    std::vector<grfx::PipelineLayoutPtr>      mPipelineLayouts;
    std::vector<grfx::RenderPassPtr>          mRenderPasses;
    std::vector<grfx::RenderTargetViewPtr>    mRenderTargetViews;
    std::vector<grfx::SemaphorePtr>           mSemaphores;
    std::vector<grfx::ShaderModulePtr>        mShaderModules;
    std::vector<grfx::ShaderProgramPtr>       mShaderPrograms;
    std::vector<grfx::SwapchainPtr>           mSwapchains;
    std::vector<grfx::CommandBufferPtr>       mCommandBuffers;
    std::vector<grfx::DescriptorSetPtr>       mSets;
    std::vector<grfx::QueuePtr>               mGraphicsQueues;
    std::vector<grfx::QueuePtr>               mComputeQueues;
    std::vector<grfx::QueuePtr>               mTransferQueues;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_device_h
