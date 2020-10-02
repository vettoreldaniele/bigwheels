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
    const grfx::Semaphore* const*     ppSignalSemaphores   = nullptr;
    const grfx::Fence*                pFence               = nullptr;
};

namespace internal {

//! @struct QueueCreateInfo
//!
//!
struct QueueCreateInfo
{
    uint32_t queueFamilyIndex = kInvalidQueueFamilyIndex; // Vulkan
    uint32_t queueIndex       = kInvalidQueueIndex;       // Vulkan
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

    virtual Result Submit(const grfx::SubmitInfo* pSubmitInfo) = 0;

    virtual Result Present(
        const grfx::Swapchain*        pSwapchain,
        uint32_t                      imageIndex,
        uint32_t                      waitSemaphoreCount,
        const grfx::Semaphore* const* ppWaitSemaphores) = 0;

    Result CreateCommandBuffer(grfx::CommandBuffer** ppCommandBuffer);
    void   DestroyCommandBuffer(const grfx::CommandBuffer* pCommandBuffer);

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
