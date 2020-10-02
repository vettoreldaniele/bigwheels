#ifndef ppx_grfx_vk_command_h
#define ppx_grfx_vk_command_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_command.h"

namespace ppx {
namespace grfx {
namespace vk {

class CommandBuffer
    : public grfx::CommandBuffer
{
public:
    CommandBuffer() {}
    virtual ~CommandBuffer() {}

    VkCommandBufferPtr GetVkCommandBuffer() const { return mCommandBuffer; }

    virtual Result Begin() override;
    virtual Result End() override;

    virtual void BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo) override;
    virtual void EndRenderPass() override;

    virtual void TransitionImageLayout(
        const grfx::Image*  pImage,
        uint32_t            mipLevel,
        uint32_t            mipLevelCount,
        uint32_t            arrayLayer,
        uint32_t            arrayLayerCount,
        grfx::ResourceState beforeState,
        grfx::ResourceState afterState) override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkCommandBufferPtr mCommandBuffer;
};

// -------------------------------------------------------------------------------------------------

class CommandPool
    : public grfx::CommandPool
{
public:
    CommandPool() {}
    virtual ~CommandPool() {}

    VkCommandPoolPtr GetVkCommandPool() const { return mCommandPool; }

protected:
    virtual Result CreateApiObjects(const grfx::CommandPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkCommandPoolPtr mCommandPool;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_command_h
