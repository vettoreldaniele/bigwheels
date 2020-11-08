#ifndef ppx_grfx_command_buffer_h
#define ppx_grfx_command_buffer_h

// *** Graphics API Note ***
//
// In the cosmos of game engines, there's more than one way to build
// command buffers and track various bits that company that.
//
// Smaller engines and graphics demo may favor command buffer reuse or
// at least reusing the same resources in a similar order per frame.
//
// Larger engines may have have an entire job system where available
// command buffers are use for the next immediate task. There may
// or may not be any affinity for command buffers and tasks.
//
// We're going to favor the second case - command buffers do not
// have affinity for tasks. This means that for D3D12 we'll copy
// descriptors from the set's CPU heaps to the command buffer's
// GPU visible heaps when the indGraphicsDescriptorSets or
// BindComputeDescriptorSets is called. This may not be the most
// efficient way to do this but it does give us the flexiblity
// to shape D3D12 to look like Vulkan.
//

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct BufferToBufferCopyInfo
//!
//!
struct BufferToBufferCopyInfo
{
    uint64_t size = 0;

    struct srcBuffer
    {
        uint64_t offset = 0;
    } srcBuffer;

    struct
    {
        uint32_t offset = 0;
    } dstBuffer;
};

//! @struct BufferToImageCopyInfo
//!
//!
struct BufferToImageCopyInfo
{
    struct
    {
        uint32_t imageWidth      = 0; // [pixels]
        uint32_t imageHeight     = 0; // [pixels]
        uint32_t imageRowStride  = 0; // [bytes]
        uint64_t footprintOffset = 0; // [bytes]
        uint32_t footprintWidth  = 0; // [pixels]
        uint32_t footprintHeight = 0; // [pixels]
        uint32_t footprintDepth  = 0; // [pixels]
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

//! @struct ImageToBufferCopyInfo
//!
//!
struct ImageToBufferCopyInfo
{
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
    } srcImage;

    struct
    {
        uint64_t offset          = 0;
        uint32_t footprintWidth  = 0; // [pixels] Use 0 for tight packing
        uint32_t footprintHeight = 0; // [pixels] Use 0 for tight packing
    } dstBuffer;
};

// -------------------------------------------------------------------------------------------------

struct RenderPassBeginInfo
{
    const grfx::RenderPass*      pRenderPass                            = nullptr;
    grfx::Rect                   renderArea                             = {};
    uint32_t                     RTVClearCount                          = 0;
    grfx::RenderTargetClearValue RTVClearValues[PPX_MAX_RENDER_TARGETS] = {0.0f, 0.0f, 0.0f, 0.0f};
    grfx::DepthStencilClearValue DSVClearValue                          = {1.0f, 0xFF};
};

// -------------------------------------------------------------------------------------------------

namespace internal {

//! @struct CommandBufferCreateInfo
//!
//! For D3D12 every command buffer will have two GPU visible descriptor heaps:
//!   - one for CBVSRVUAV descriptors
//!   - one for Sampler descriptors
//!
//! Both of heaps are set when the command buffer begins.
//!
//! Each time that BindGraphicsDescriptorSets or BindComputeDescriptorSets
//! is called, the contents of each descriptor set's CBVSRVAUV and Sampler heaps
//! will be copied into the command buffer's respective heap.
//!
//! The offsets from used in the copies will be saved and used to set the
//! root descriptor tables.
//!
//! 'resourceDescriptorCount' and 'samplerDescriptorCount' tells the
//! D3D12 command buffer how large CBVSRVUAV and Sampler heaps should be.
//!
//! 'samplerDescriptorCount' cannot exceed PPX_MAX_SAMPLER_DESCRIPTORS.
//!
//! Vulkan does not use 'samplerDescriptorCount' or 'samplerDescriptorCount'.
//!
struct CommandBufferCreateInfo
{
    const grfx::CommandPool* pPool                   = nullptr;
    uint32_t                 resourceDescriptorCount = PPX_DEFAULT_RESOURCE_DESCRIPTOR_COUNT;
    uint32_t                 samplerDescriptorCount  = PPX_DEFAULT_SAMPLE_DESCRIPTOR_COUNT;
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

    //virtual void TransitionImageLayout(
    //    const grfx::Image*   pImage,
    //    uint32_t             mipLevel,
    //    uint32_t             mipLevelCount,
    //    uint32_t             arrayLayer,
    //    uint32_t             arrayLayerCount,
    //    grfx::ResourceState  beforeState,
    //    grfx::ResourceState  afterState,
    //    const grfx::Queue*   pSrcQueue = nullptr,
    //    const grfx::Queue*   pDstQueue = nullptr,
    //    grfx::TransitionFlag flag      = grfx::TRANSITION_FLAG_API_REQUIRED) = 0;

    virtual void SetViewports(
        uint32_t              viewportCount,
        const grfx::Viewport* pViewports) = 0;

    virtual void SetScissors(
        uint32_t          scissorCount,
        const grfx::Rect* pScissors) = 0;

    virtual void BindGraphicsDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets) = 0;

    virtual void BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline) = 0;

    virtual void BindComputeDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets) = 0;

    virtual void BindComputePipeline(const grfx::ComputePipeline* pPipeline) = 0;

    virtual void BindIndexBuffer(const grfx::IndexBufferView* pView) = 0;

    virtual void BindVertexBuffers(
        uint32_t                      viewCount,
        const grfx::VertexBufferView* pViews) = 0;

    virtual void Draw(
        uint32_t vertexCount,
        uint32_t instanceCount = 1,
        uint32_t firstVertex   = 0,
        uint32_t firstInstance = 0) = 0;

    virtual void DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount = 1,
        uint32_t firstIndex    = 0,
        int32_t  vertexOffset  = 0,
        uint32_t firstInstance = 0) = 0;

    virtual void Dispatch(
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ) = 0;

    virtual void CopyBufferToBuffer(
        const grfx::BufferToBufferCopyInfo* pCopyInfo,
        const grfx::Buffer*                 pSrcBuffer,
        const grfx::Buffer*                 pDstBuffer) = 0;

    virtual void CopyBufferToImage(
        const grfx::BufferToImageCopyInfo* pCopyInfo,
        const grfx::Buffer*                pSrcBuffer,
        const grfx::Image*                 pDstImage) = 0;

    virtual void CopyImageToBuffer(
        const grfx::ImageToBufferCopyInfo* pCopyInfo,
        const grfx::Image*                 pSrcImage,
        const grfx::Buffer*                pDstBuffer) = 0;

    // ---------------------------------------------------------------------------------------------
    // Convenience functions
    // ---------------------------------------------------------------------------------------------
    void BeginRenderPass(const grfx::RenderPass* pRenderPass);

    void BeginRenderPass(
        const grfx::DrawPass*           pDrawPass,
        const grfx::DrawPassClearFlags& clearFlags = grfx::DRAW_PASS_CLEAR_FLAG_CLEAR_ALL);

    void TransitionImageLayout(
        grfx::RenderPass*   pRenderPass,
        grfx::ResourceState renderTargetBeforeState,
        grfx::ResourceState renderTargetAfterState,
        grfx::ResourceState depthStencilTargetBeforeState,
        grfx::ResourceState depthStencilTargetAfterState);

    void TransitionImageLayout(
        grfx::DrawPass*     pDrawPass,
        grfx::ResourceState renderTargetBeforeState,
        grfx::ResourceState renderTargetAfterState,
        grfx::ResourceState depthStencilTargetBeforeState,
        grfx::ResourceState depthStencilTargetAfterState);

    void SetViewports(const grfx::Viewport& viewport);

    void SetScissors(const grfx::Rect& scissor);

    void BindIndexBuffer(const grfx::Buffer* pBuffer, grfx::IndexType indexType, uint64_t offset = 0);

    void BindIndexBuffer(const grfx::Model* pModel, uint64_t offset = 0);

    void BindVertexBuffers(
        uint32_t                   bufferCount,
        const grfx::Buffer* const* ppBuffers,
        const uint32_t*            pStrides,
        const uint64_t*            pOffsets = nullptr);

    void BindVertexBuffers(const grfx::Model* pModel, const uint64_t* pOffsets = nullptr);
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
