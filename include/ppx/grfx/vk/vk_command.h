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
        const grfx::Image*   pImage,
        uint32_t             mipLevel,
        uint32_t             mipLevelCount,
        uint32_t             arrayLayer,
        uint32_t             arrayLayerCount,
        grfx::ResourceState  beforeState,
        grfx::ResourceState  afterState,
        const grfx::Queue*   pSrcQueue,
        const grfx::Queue*   pDstQueue) override;

    virtual void BufferResourceBarrier(
        const grfx::Buffer*  pBuffer,
        grfx::ResourceState  beforeState,
        grfx::ResourceState  afterState,
        const grfx::Queue*   pSrcQueue = nullptr,
        const grfx::Queue*   pDstQueue = nullptr) override;

    virtual void SetViewports(
        uint32_t              viewportCount,
        const grfx::Viewport* pViewports) override;

    virtual void SetScissors(
        uint32_t          scissorCount,
        const grfx::Rect* pScissors) override;

    virtual void BindGraphicsDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets) override;

    virtual void BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline) override;

    virtual void BindComputeDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets) override;

    virtual void BindComputePipeline(const grfx::ComputePipeline* pPipeline) override;

    virtual void BindIndexBuffer(const grfx::IndexBufferView* pView) override;

    virtual void BindVertexBuffers(
        uint32_t                      viewCount,
        const grfx::VertexBufferView* pViews) override;

    virtual void Draw(
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance) override;

    virtual void DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t  vertexOffset,
        uint32_t firstInstance) override;

    virtual void Dispatch(
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ) override;

    virtual void CopyBufferToBuffer(
        const grfx::BufferToBufferCopyInfo* pCopyInfo,
        const grfx::Buffer*                 pSrcBuffer,
        const grfx::Buffer*                 pDstBuffer) override;

    virtual void CopyBufferToImage(
        const grfx::BufferToImageCopyInfo* pCopyInfo,
        const grfx::Buffer*                pSrcBuffer,
        const grfx::Image*                 pDstImage) override;

    virtual void CopyImageToBuffer(
        const grfx::ImageToBufferCopyInfo* pCopyInfo,
        const grfx::Image*                 pSrcImage,
        const grfx::Buffer*                pDstBuffer) override;

    virtual void BeginQuery(
        const grfx::QueryPool* pQueryPool,
        uint32_t               queryIndex) override;

    virtual void EndQuery(
        const grfx::QueryPool* pQueryPool,
        uint32_t               queryIndex) override;

    virtual void WriteTimestamp(
        grfx::PipelineStage    pipelineStage,
        const grfx::QueryPool* pQueryPool,
        uint32_t               queryIndex) override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    void BindDescriptorSets(
        VkPipelineBindPoint               bindPoint,
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets);

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
