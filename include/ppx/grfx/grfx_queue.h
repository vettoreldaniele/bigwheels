#ifndef ppx_grfx_queue_h
#define ppx_grfx_queue_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

struct SubmitInfo
{
    uint32_t                          commandBufferCount   = 0;
    const grfx::CommandBuffer* const* ppCommandBuffers     = nullptr;
    uint32_t                          waitSemaphoreCount   = 0;
    const grfx::Semaphore* const*     ppWaitSemaphores     = nullptr;
    uint32_t                          signalSemaphoreCount = 0;
    grfx::Semaphore**                 ppSignalSemaphores   = nullptr;
    grfx::Fence*                      pFence               = nullptr;
};

namespace internal {

//! @struct QueueCreateInfo
//!
//!
struct QueueCreateInfo
{
    grfx::CommandType commandType      = grfx::COMMAND_TYPE_UNDEFINED;
    uint32_t          queueFamilyIndex = PPX_VALUE_IGNORED; // Vulkan
    uint32_t          queueIndex       = PPX_VALUE_IGNORED; // Vulkan
    void*             pApiObject       = nullptr;           // D3D12
};

} // namespace internal

//! @class Queue
//!
//!
class Queue
    : public grfx::DeviceObject<grfx::internal::QueueCreateInfo>
{
public:
    Queue() {}
    virtual ~Queue() {}

    grfx::CommandType GetCommandType() const { return mCreateInfo.commandType; }

    virtual Result WaitIdle() = 0;

    virtual Result Submit(const grfx::SubmitInfo* pSubmitInfo) = 0;

    Result CreateCommandBuffer(
        grfx::CommandBuffer** ppCommandBuffer,
        uint32_t              resourceDescriptorCount = PPX_DEFAULT_RESOURCE_DESCRIPTOR_COUNT,
        uint32_t              samplerDescriptorCount  = PPX_DEFAULT_SAMPLE_DESCRIPTOR_COUNT);
    void DestroyCommandBuffer(const grfx::CommandBuffer* pCommandBuffer);

private:
    struct CommandSet
    {
        grfx::CommandPoolPtr   commandPool;
        grfx::CommandBufferPtr commandBuffer;
    };
    std::vector<CommandSet> mCommandSets;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_queue_h