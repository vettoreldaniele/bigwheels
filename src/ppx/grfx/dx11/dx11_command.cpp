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
    mViewportState.Reset();
    mScissorState.Reset();
    mActionCmds.clear();
    return ppx::SUCCESS;
}

Result CommandBuffer::End()
{
    return ppx::SUCCESS;
}

void CommandBuffer::BeginRenderPassImpl(const grfx::RenderPassBeginInfo* pBeginInfo)
{
    uint32_t rtvCount = pBeginInfo->pRenderPass->GetRenderTargetCount();
    PPX_ASSERT_MSG((rtvCount <= PPX_MAX_RENDER_TARGETS), "Number of clear values exceeds limit");

    mActionCmds.emplace_back(ActionCmd(CMD_BEGIN_RENDER_PASS));

    ActionCmd&      cmd  = mActionCmds.back();
    RenderPassArgs& args = cmd.args.renderPass;

    args.rtvs.numViews = rtvCount;
    for (uint32_t i = 0; i < args.rtvs.numViews; ++i) {
        auto pApiRtv         = ToApi(pBeginInfo->pRenderPass->GetRenderTargetView(i));
        args.rtvs.views[i]   = pApiRtv->GetDxRenderTargetView();
        args.rtvs.loadOps[i] = pApiRtv->GetLoadOp();
        memcpy(args.rtvs.clearValues[i].rgba.data(), pBeginInfo->RTVClearValues[i].rgba, 4 * sizeof(float));
    }

    args.dsv.pView  = nullptr;
    args.dsv.loadOp = grfx::ATTACHMENT_LOAD_OP_LOAD;
    if (pBeginInfo->pRenderPass->HasDepthStencil()) {
        PPX_ASSERT_MSG(false, "not implemented");
    }
}

void CommandBuffer::EndRenderPassImpl()
{
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
    viewportCount = std::min<uint32_t>(viewportCount, PPX_MAX_VIEWPORTS);

    ViewportState* pState = mViewportState.GetCurrent();
    pState->numViewports  = static_cast<UINT>(viewportCount);
    for (uint32_t i = 0; i < viewportCount; ++i) {
        pState->viewports[i].TopLeftX = pViewports[i].x;
        pState->viewports[i].TopLeftY = pViewports[i].y;
        pState->viewports[i].Width    = pViewports[i].width;
        pState->viewports[i].Height   = pViewports[i].height;
        pState->viewports[i].MinDepth = pViewports[i].minDepth;
        pState->viewports[i].MaxDepth = pViewports[i].maxDepth;
    }
}

void CommandBuffer::SetScissors(
    uint32_t          scissorCount,
    const grfx::Rect* pScissors)
{
    scissorCount = std::min<uint32_t>(scissorCount, PPX_MAX_SCISSORS);

    ScissorState* pState = mScissorState.GetCurrent();
    pState->numRects     = scissorCount;
    for (uint32_t i = 0; i < scissorCount; ++i) {
        pState->rects[i].left   = pScissors[i].x;
        pState->rects[i].top    = pScissors[i].y;
        pState->rects[i].right  = pScissors[i].x + pScissors[i].width;
        pState->rects[i].bottom = pScissors[i].y + pScissors[i].height;
    }
}

void CommandBuffer::BindGraphicsDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    setCount = std::min<uint32_t>(setCount, PPX_MAX_BOUND_DESCRIPTOR_SETS);
}

void CommandBuffer::BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)
{
    const dx11::GraphicsPipeline* pApiPipeline = ToApi(pPipeline);

    GraphicsPipelineState* pState = mGraphicsPipelineStack.GetCurrent();
    pState->VS                    = pApiPipeline->GetVS();
    pState->HS                    = pApiPipeline->GetHS();
    pState->DS                    = pApiPipeline->GetDS();
    pState->GS                    = pApiPipeline->GetGS();
    pState->PS                    = pApiPipeline->GetPS();
    pState->inputLayout           = pApiPipeline->GetInputLayout();
    pState->primitiveTopology     = pApiPipeline->GetPrimitiveTopology();
    pState->rasterizerState       = pApiPipeline->GetRasterizerState();
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
    IndexBufferState* pState = mIndexBufferState.GetCurrent();
    pState->buffer           = ToApi(pView->pBuffer)->GetDxBuffer();
    pState->format           = ToD3D11IndexFormat(pView->indexType);
    pState->offset           = static_cast<UINT>(pView->offset);
}

void CommandBuffer::BindVertexBuffers(
    uint32_t                      viewCount,
    const grfx::VertexBufferView* pViews)
{
    VertexBufferState* pState = mVertexBuffersState.GetCurrent();
    pState->startSlot         = 0;
    pState->numBuffers        = static_cast<UINT>(viewCount);
    for (uint32_t i = 0; i < viewCount; ++i) {
        pState->buffers[i] = ToApi(pViews[i].pBuffer)->GetDxBuffer();
        pState->strides[i] = pViews[i].stride;
        pState->offsets[i] = 0;
    }
}

void CommandBuffer::Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    mActionCmds.emplace_back(ActionCmd(CMD_DRAW));

    ActionCmd& cmd                 = mActionCmds.back();
    cmd.viewportStateIndex         = mViewportState.Commit();
    cmd.scissorStateIndex          = mScissorState.Commit();
    cmd.indexBuffereStateIndex     = kInvalidStateIndex;
    cmd.vertexBufferStateIndex     = mVertexBuffersState.Commit();
    cmd.graphicsPipleineStateIndex = mGraphicsPipelineStack.Commit();

    DrawArgs& args              = cmd.args.draw;
    args.vertexCountPerInstance = static_cast<UINT>(vertexCount);
    args.instanceCount          = static_cast<UINT>(instanceCount);
    args.startVertexLocation    = static_cast<UINT>(firstVertex);
    args.startInstanceLocation  = static_cast<UINT>(firstInstance);
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
