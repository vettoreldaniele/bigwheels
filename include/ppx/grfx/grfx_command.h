#ifndef ppx_grfx_command_buffer_h
#define ppx_grfx_command_buffer_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct BufferToImageCopyInfo
//!
//!
struct BufferToImageCopyInfo
{
    // Se: https://stackoverflow.com/a/46504718 for a good expalnation
    // of how Vulkan's copy parameters.
    //
    struct
    {
        uint64_t offset          = 0;
        uint32_t footprintWidth  = 0; // [pixels] Use 0 for tight packing
        uint32_t footprintHeight = 0; // [pixels] Use 0 for tight packing
    } srcBuffer;

    struct
    {
        uint32_t mipLevel        = 0;
        uint32_t arrayLayer      = 0; // Must be 0 for 3D images
        uint32_t arrayLayerCount = 0; // Must be 1 for 3D images
        uint32_t x               = 0; // [pixels]
        uint32_t y               = 0; // [pixels]
        uint32_t z               = 0; // [pixels]
        uint32_t width           = 0; // [pixels]
        uint32_t height          = 0; // [pixels]
        uint32_t depth           = 0; // [pixels]
    } dstImage;
};

// -------------------------------------------------------------------------------------------------

struct RenderPassBeginInfo
{
    const grfx::RenderPass*      pRenderPass                            = nullptr;
    grfx::Rect                   renderArea                             = {};
    uint32_t                     RTVClearCount                          = 0;
    grfx::RenderTargetClearValue RTVClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue DSVClearValue                          = {};
};

// -------------------------------------------------------------------------------------------------

namespace internal {

struct CommandBufferCreateInfo
{
    const grfx::CommandPool* pPool = nullptr;
};

} // namespace internal

//! @class CommandBuffer
//!
//!
class CommandBuffer
    : public grfx::DeviceObject<grfx::internal::CommandBufferCreateInfo>
{
public:
    CommandBuffer() {}
    virtual ~CommandBuffer() {}

    virtual Result Begin() = 0;
    virtual Result End()   = 0;

    virtual void BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo) = 0;
    virtual void EndRenderPass()                                              = 0;

    virtual void TransitionImageLayout(
        const grfx::Image*  pImage,
        uint32_t            mipLevel,
        uint32_t            mipLevelCount,
        uint32_t            arrayLayer,
        uint32_t            arrayLayerCount,
        grfx::ResourceState beforeState,
        grfx::ResourceState afterState) = 0;

    virtual void SetViewports(uint32_t viewportCount, const grfx::Viewport* pViewports) = 0;
    virtual void SetScissors(uint32_t scissorCount, const grfx::Rect* pScissors)        = 0;

    virtual void BindGraphicsDescriptorSets(const grfx::PipelineInterface* pInterface, uint32_t setCount, const grfx::DescriptorSet* const* ppSets) = 0;
    virtual void BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)                                                                      = 0;

    virtual void BindComputeDescriptorSets(const grfx::PipelineInterface* pInterface, uint32_t setCount, const grfx::DescriptorSet* const* ppSets) = 0;
    virtual void BindComputePipeline(const grfx::ComputePipeline* pPipeline)                                                                       = 0;

    virtual void BindIndexBuffer(const grfx::IndexBufferView* pView)                         = 0;
    virtual void BindVertexBuffers(uint32_t viewCount, const grfx::VertexBufferView* pViews) = 0;

    virtual void Draw(
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance) = 0;

    virtual void DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t  vertexOffset,
        uint32_t firstInstance) = 0;

    virtual void Dispatch(
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ) = 0;

    virtual void CopyBufferToImage(
        const grfx::BufferToImageCopyInfo* pCopyInfo,
        const grfx::Buffer*                pSrcBuffer,
        const grfx::Image*                 pDstImage) = 0;

    // Convenience functions
    void BindIndexBuffer(const grfx::Buffer* pBuffer, grfx::IndexType indexType, uint64_t offset = 0);
    void BindVertexBuffers(uint32_t bufferCount, const grfx::Buffer* const* ppBuffers, const uint64_t* pOffsets = nullptr);
};

// -------------------------------------------------------------------------------------------------

//! @struct CommandPoolCreateInfo
//!
//!
struct CommandPoolCreateInfo
{
    const grfx::Queue* pQueue = nullptr;
};

//! @class CommandPool
//!
//!
class CommandPool
    : public grfx::DeviceObject<grfx::CommandPoolCreateInfo>
{
public:
    CommandPool() {}
    virtual ~CommandPool() {}
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_command_buffer_h
