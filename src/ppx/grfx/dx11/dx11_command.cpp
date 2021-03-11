#include "ppx/grfx/dx11/dx11_command.h"
#include "ppx/grfx/dx11/dx11_buffer.h"
#include "ppx/grfx/dx11/dx11_descriptor.h"
#include "ppx/grfx/dx11/dx11_device.h"
#include "ppx/grfx/dx11/dx11_image.h"
#include "ppx/grfx/dx11/dx11_pipeline.h"
#include "ppx/grfx/dx11/dx11_query.h"
#include "ppx/grfx/dx11/dx11_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx11 {

// -------------------------------------------------------------------------------------------------
// CommandBuffer
// -------------------------------------------------------------------------------------------------
Result CommandBuffer::CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo)
{
    mActionCmds.reserve(32);
    return ppx::SUCCESS;
}

void CommandBuffer::DestroyApiObjects()
{
}

Result CommandBuffer::Begin()
{
    mActionCmds.clear();
    return ppx::SUCCESS;
}

Result CommandBuffer::End()
{
    return ppx::SUCCESS;
}

void CommandBuffer::BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo)
{
    mActionCmds.emplace_back(ActionCmd(CMD_BEGIN_RENDER_PASS));

    uint32_t renderTargetCount = pBeginInfo->pRenderPass->GetRenderTargetCount();

    ActionCmd&           cmd  = mActionCmds.back();
    BeginRenderPassArgs& args = cmd.args.beginRenderPass;

    cmd.args.beginRenderPass.NumViews = static_cast<UINT>(renderTargetCount);

    for (uint32_t i = 0; i < renderTargetCount; ++i) {
        typename D3D11RenderTargetViewPtr::InterfaceType* pRTV = ToApi(pBeginInfo->pRenderPass->GetRenderTargetView(i))->GetDxRenderTargetView();
        args.ppRenderTargetViews[i]                            = pRTV;

        mPipelineState.currentRenderTargetCount    = renderTargetCount;
        mPipelineState.currentRenderTargetViews[i] = pRTV;
    }

    for (uint32_t i = 0; i < pBeginInfo->RTVClearCount; ++i) {
        args.RTVClearOp[i].pRenderTargetView = args.ppRenderTargetViews[i];

        FLOAT*       pDst = args.RTVClearOp[i].ColorRGBA;
        const float* pSrc = pBeginInfo->RTVClearValues->rgba;
        size_t       size = sizeof(pBeginInfo->RTVClearValues->rgba);
        memcpy(pDst, pSrc, size);
    }

    if (pBeginInfo->pRenderPass->HasDepthStencil()) {
    }
}

void CommandBuffer::EndRenderPass()
{
    for (uint32_t i = 0; i < mPipelineState.currentRenderTargetCount; ++i) {
        mPipelineState.currentRenderTargetViews[i] = nullptr;
        mPipelineState.currentDepthStencilViews    = nullptr;
    }
}

void CommandBuffer::TransitionImageLayout(
    const grfx::Image*  pImage,
    uint32_t            mipLevel,
    uint32_t            mipLevelCount,
    uint32_t            arrayLayer,
    uint32_t            arrayLayerCount,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState,
    const grfx::Queue*  pSrcQueue,
    const grfx::Queue*  pDstQueue)
{
}

void CommandBuffer::BufferResourceBarrier(
    const grfx::Buffer* pBuffer,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState,
    const grfx::Queue*  pSrcQueue,
    const grfx::Queue*  pDstQueue)
{
}

void CommandBuffer::SetViewports(
    uint32_t              viewportCount,
    const grfx::Viewport* pViewports)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::SetScissors(
    uint32_t          scissorCount,
    const grfx::Rect* pScissors)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets,
    size_t&                           rdtCountCBVSRVUAV,
    size_t&                           rdtCountSampler)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindGraphicsDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindComputeDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
}

void CommandBuffer::BindComputePipeline(const grfx::ComputePipeline* pPipeline)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindIndexBuffer(const grfx::IndexBufferView* pView)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindVertexBuffers(
    uint32_t                      viewCount,
    const grfx::VertexBufferView* pViews)
{
}

void CommandBuffer::Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::DrawIndexed(
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t  vertexOffset,
    uint32_t firstInstance)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::Dispatch(
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::CopyBufferToBuffer(
    const grfx::BufferToBufferCopyInfo* pCopyInfo,
    const grfx::Buffer*                 pSrcBuffer,
    const grfx::Buffer*                 pDstBuffer)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::CopyBufferToImage(
    const grfx::BufferToImageCopyInfo* pCopyInfo,
    const grfx::Buffer*                pSrcBuffer,
    const grfx::Image*                 pDstImage)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::CopyImageToBuffer(
    const grfx::ImageToBufferCopyInfo* pCopyInfo,
    const grfx::Image*                 pSrcImage,
    const grfx::Buffer*                pDstBuffer)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BeginQuery(
    const grfx::QueryPool* pQueryPool,
    uint32_t               queryIndex)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::EndQuery(
    const grfx::QueryPool* pQueryPool,
    uint32_t               queryIndex)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::WriteTimestamp(
    grfx::PipelineStage    pipelineStage,
    const grfx::QueryPool* pQueryPool,
    uint32_t               queryIndex)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::ImGuiRender(void (*pFn)(void))
{
    mActionCmds.emplace_back(ActionCmd(CMD_IM_GUI_RENDER));

    ActionCmd& cmd                 = mActionCmds.back();
    cmd.args.imGuiRender.pRenderFn = pFn;
}

// -------------------------------------------------------------------------------------------------
// CommandPool
// -------------------------------------------------------------------------------------------------
Result CommandPool::CreateApiObjects(const grfx::CommandPoolCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void CommandPool::DestroyApiObjects()
{
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
