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
    mIndexBufferState.Reset();
    mVertexBuffersState.Reset();
    mComputePipelineState.Reset();
    mGraphicsPipelineState.Reset();
    mComputeDescriptorState.Reset();
    mGraphicsDescriptorState.Reset();

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

    args.dsv.pView         = nullptr;
    args.dsv.depthLoadOp   = grfx::ATTACHMENT_LOAD_OP_LOAD;
    args.dsv.stencilLoadOp = grfx::ATTACHMENT_LOAD_OP_LOAD;
    if (pBeginInfo->pRenderPass->HasDepthStencil()) {
        auto pApiDsv                = ToApi(pBeginInfo->pRenderPass->GetDepthStencilView());
        args.dsv.pView              = pApiDsv->GetDxDepthStencilView();
        args.dsv.depthLoadOp        = pApiDsv->GetDepthLoadOp();
        args.dsv.stencilLoadOp      = pApiDsv->GetDepthLoadOp();
        args.dsv.clearValue.depth   = pBeginInfo->DSVClearValue.depth;
        args.dsv.clearValue.stencil = static_cast<UINT8>(pBeginInfo->DSVClearValue.depth);
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
    mActionCmds.emplace_back(ActionCmd(CMD_TRANSITION_IMAGE_LAYOUT));

    ActionCmd&      cmd  = mActionCmds.back();
    TransitionArgs& args = cmd.args.transition;
    args.resource        = ToApi(pImage)->GetDxResource();
    args.beforeState     = beforeState;
    args.afterState      = afterState;
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

static bool IsVS(grfx::ShaderStageBits shaderVisbility)
{
    bool match = (shaderVisbility == grfx::SHADER_STAGE_VS) || (shaderVisbility == grfx::SHADER_STAGE_ALL_GRAPHICS) || (shaderVisbility == grfx::SHADER_STAGE_ALL);
    return match;
}

static bool IsHS(grfx::ShaderStageBits shaderVisbility)
{
    bool match = (shaderVisbility == grfx::SHADER_STAGE_HS) || (shaderVisbility == grfx::SHADER_STAGE_ALL_GRAPHICS) || (shaderVisbility == grfx::SHADER_STAGE_ALL);
    return match;
}

static bool IsDS(grfx::ShaderStageBits shaderVisbility)
{
    bool match = (shaderVisbility == grfx::SHADER_STAGE_DS) || (shaderVisbility == grfx::SHADER_STAGE_ALL_GRAPHICS) || (shaderVisbility == grfx::SHADER_STAGE_ALL);
    return match;
}

static bool IsGS(grfx::ShaderStageBits shaderVisbility)
{
    bool match = (shaderVisbility == grfx::SHADER_STAGE_GS) || (shaderVisbility == grfx::SHADER_STAGE_ALL_GRAPHICS) || (shaderVisbility == grfx::SHADER_STAGE_ALL);
    return match;
}

static bool IsPS(grfx::ShaderStageBits shaderVisbility)
{
    bool match = (shaderVisbility == grfx::SHADER_STAGE_PS) || (shaderVisbility == grfx::SHADER_STAGE_ALL_GRAPHICS) || (shaderVisbility == grfx::SHADER_STAGE_ALL);
    return match;
}

void CommandBuffer::CopyDescriptors(const dx11::DescriptorArray& bindings, std::vector<ResourceSlotArray>& slots)
{
    const size_t numResources = bindings.resources.size();
    size_t       numPopulated = 0;
    for (size_t i = 0; i < numResources; ++i) {
        if (!IsNull(bindings.resources[i])) {
            numPopulated += 1;
        }
    }

    // This means that the descriptors bindings are contiguous and
    // can be set in a single call to one of the ID3D11DeviceContext::*Set*()
    // functions. For example, the following sets 3 constant buffers for
    // slots 1, 2, and 3:
    //
    //    ResourceSlotArray slots = {};
    //    slots.descriptorType    = D3D_DESCRIPTOR_TYPE_CBV;
    //    slots.startSlot         = 1;
    //    slots.resources.push_back(buffer0);
    //    slots.resources.push_back(buffer1);
    //    slots.resources.push_back(buffer2);
    //
    //    VSSetConstantBuffers(
    //        slots.startSlot,
    //        slots.resources.size(),
    //        reinterpret_cast<ID3D11Buffer* const*>(slots.resources.data()));
    //
    if (numPopulated == numResources) {
        // Create entry in resource slot array
        slots.emplace_back(ResourceSlotArray{bindings.descriptorType, bindings.binding});
        // Allocate space
        ResourceSlotArray& dst = slots.back();
        dst.resources.resize(numResources);
        // Copy resource pointers from bindings to slots
        size_t copySize = numResources * sizeof(void*);
        memcpy(dst.resources.data(), bindings.resources.data(), copySize);
    }
    //
    // This means that descriptor bindings are disparate and
    // must be sent individually. In this case each ResourceSlotArray
    // object will only contain one resource.
    //
    else {
        // Create an entry for every populated element
        for (size_t i = 0; i < numResources; ++i) {
            // Skip anything that isn't populated
            if (IsNull(bindings.resources[i])) {
                continue;
            }
            // Start slot is binding plus offset in array
            const UINT startSlot = static_cast<UINT>(bindings.binding + i);
            // Create entry in dst bindings
            slots.emplace_back(ResourceSlotArray{bindings.descriptorType, startSlot});
            // Copy descriptor
            ResourceSlotArray& slot = slots.back();
            slot.resources.push_back(bindings.resources[i]);
        }
    }
}

void SetVSSlots(const dx11::DescriptorArray& descriptorArray)
{
    
}

void CommandBuffer::BindGraphicsDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    setCount = std::min<uint32_t>(setCount, PPX_MAX_BOUND_DESCRIPTOR_SETS);

    GraphicsDescriptorState* pState = mGraphicsDescriptorState.GetCurrent();

    for (uint32_t setIndex = 0; setIndex < setCount; ++setIndex) {
        const grfx::dx11::DescriptorSet* pApiSet          = ToApi(ppSets[setIndex]);
        const auto&                      descriptorArrays = pApiSet->GetDescriptorArrays();

        size_t daCount = descriptorArrays.size();
        for (size_t daIndex = 0; daIndex < daCount; ++daIndex) {
            const DescriptorArray& binding = descriptorArrays[daIndex];
            if (IsVS(binding.shaderVisibility)) {
                CopyDescriptors(binding, pState->VS);
            }
            if (IsHS(binding.shaderVisibility)) {
                CopyDescriptors(binding, pState->HS);
            }
            if (IsDS(binding.shaderVisibility)) {
                CopyDescriptors(binding, pState->HS);
            }
            if (IsGS(binding.shaderVisibility)) {
                CopyDescriptors(binding, pState->GS);
            }
            if (IsPS(binding.shaderVisibility)) {
                CopyDescriptors(binding, pState->PS);
            }
        }
    }
}

void CommandBuffer::BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)
{
    const dx11::GraphicsPipeline* pApiPipeline = ToApi(pPipeline);

    GraphicsPipelineState* pState = mGraphicsPipelineState.GetCurrent();
    pState->VS                    = pApiPipeline->GetVS();
    pState->HS                    = pApiPipeline->GetHS();
    pState->DS                    = pApiPipeline->GetDS();
    pState->GS                    = pApiPipeline->GetGS();
    pState->PS                    = pApiPipeline->GetPS();
    pState->inputLayout           = pApiPipeline->GetInputLayout();
    pState->primitiveTopology     = pApiPipeline->GetPrimitiveTopology();
    pState->rasterizerState       = pApiPipeline->GetRasterizerState();
    pState->depthStencilState     = pApiPipeline->GetDepthStencilstate();
}

static bool IsCS(grfx::ShaderStageBits shaderVisbility)
{
    bool match = (shaderVisbility == grfx::SHADER_STAGE_CS) || (shaderVisbility == grfx::SHADER_STAGE_ALL_GRAPHICS) || (shaderVisbility == grfx::SHADER_STAGE_ALL);
    return match;
}

void CommandBuffer::BindComputeDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    setCount = std::min<uint32_t>(setCount, PPX_MAX_BOUND_DESCRIPTOR_SETS);

    ComputeDescriptorState* pState = mComputeDescriptorState.GetCurrent();

    for (uint32_t setIndex = 0; setIndex < setCount; ++setIndex) {
        const grfx::dx11::DescriptorSet* pApiSet          = ToApi(ppSets[setIndex]);
        const auto&                      descriptorArrays = pApiSet->GetDescriptorArrays();

        size_t daCount = descriptorArrays.size();
        for (size_t daIndex = 0; daIndex < daCount; ++daIndex) {
            const dx11::DescriptorArray& descriptorArray = descriptorArrays[daIndex];
            if (IsCS(descriptorArray.shaderVisibility)) {
                CopyDescriptors(descriptorArray, pState->CS);
            }
        }
    }
}

void CommandBuffer::BindComputePipeline(const grfx::ComputePipeline* pPipeline)
{
    const dx11::ComputePipeline* pApiPipeline = ToApi(pPipeline);

    ComputePipelineState* pState = mComputePipelineState.GetCurrent();
    pState->CS                   = pApiPipeline->GetCS();
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

    ActionCmd& cmd                   = mActionCmds.back();
    cmd.viewportStateIndex           = mViewportState.Commit();
    cmd.scissorStateIndex            = mScissorState.Commit();
    cmd.indexBuffereStateIndex       = kInvalidStateIndex;
    cmd.vertexBufferStateIndex       = mVertexBuffersState.Commit();
    cmd.graphicsPipleineStateIndex   = mGraphicsPipelineState.Commit();
    cmd.graphicsDescriptorstateIndex = mGraphicsDescriptorState.Commit();

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
    mActionCmds.emplace_back(ActionCmd(CMD_DRAW_INDEXED));

    ActionCmd& cmd                   = mActionCmds.back();
    cmd.viewportStateIndex           = mViewportState.Commit();
    cmd.scissorStateIndex            = mScissorState.Commit();
    cmd.indexBuffereStateIndex       = mIndexBufferState.Commit();
    cmd.vertexBufferStateIndex       = mVertexBuffersState.Commit();
    cmd.graphicsPipleineStateIndex   = mGraphicsPipelineState.Commit();
    cmd.graphicsDescriptorstateIndex = mGraphicsDescriptorState.Commit();

    DrawIndexedArgs& args      = cmd.args.drawIndexed;
    args.indexCountPerInstance = static_cast<UINT>(indexCount);
    args.instanceCount         = static_cast<UINT>(instanceCount);
    args.startIndexLocation    = static_cast<UINT>(firstIndex);
    args.baseVertexLocation    = static_cast<UINT>(vertexOffset);
    args.startInstanceLocation = static_cast<UINT>(firstInstance);
}

void CommandBuffer::Dispatch(
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    mActionCmds.emplace_back(ActionCmd(CMD_DISPATCH));

    ActionCmd& cmd                  = mActionCmds.back();
    cmd.computePipleineStateIndex   = mComputePipelineState.Commit();
    cmd.computeDescriptorstateIndex = mComputeDescriptorState.Commit();

    DispatchArgs& args     = cmd.args.dispatch;
    args.threadGroupCountX = static_cast<UINT>(groupCountX);
    args.threadGroupCountY = static_cast<UINT>(groupCountY);
    args.threadGroupCountZ = static_cast<UINT>(groupCountZ);
}

void CommandBuffer::CopyBufferToBuffer(
    const grfx::BufferToBufferCopyInfo* pCopyInfo,
    grfx::Buffer*                       pSrcBuffer,
    grfx::Buffer*                       pDstBuffer)
{
    mActionCmds.emplace_back(ActionCmd(CMD_COPY_BUFFER_TO_BUFFER));

    ActionCmd&              cmd  = mActionCmds.back();
    CopyBufferToBufferArgs& args = cmd.args.copyBufferToBuffer;
    args.copyInfo                = *pCopyInfo;
    args.pSrcBuffer              = pSrcBuffer;
    args.pDstBuffer              = pDstBuffer;
}

void CommandBuffer::CopyBufferToImage(
    const grfx::BufferToImageCopyInfo* pCopyInfo,
    grfx::Buffer*                      pSrcBuffer,
    grfx::Image*                       pDstImage)
{
    mActionCmds.emplace_back(ActionCmd(CMD_COPY_BUFFER_TO_IMAGE));

    ActionCmd&             cmd  = mActionCmds.back();
    CopyBufferToImageArgs& args = cmd.args.copyBufferToImage;
    args.copyInfo               = *pCopyInfo;
    args.pSrcBuffer             = pSrcBuffer;
    args.pDstImage              = pDstImage;

    //const uint32_t mipLevelCount = pDstImage->GetMipLevelCount();
    //
    //for (uint32_t i = 0; i < pCopyInfo->dstImage.arrayLayerCount; ++i) {
    //    uint32_t arrayLayer = pCopyInfo->dstImage.arrayLayer + i;
    //
    //    mActionCmds.emplace_back(ActionCmd(CMD_COPY_BUFFER_TO_IMAGE));
    //
    //    ActionCmd& cmd                  = mActionCmds.back();
    //    cmd.computePipleineStateIndex   = mComputePipelineState.Commit();
    //    cmd.computeDescriptorstateIndex = mComputeDescriptorState.Commit();
    //
    //    UINT subresourceIndex = static_cast<UINT>((arrayLayer * mipLevelCount) + pCopyInfo->dstImage.mipLevel);
    //
    //    CopyBufferToImageArgs& args = cmd.args.copyBufferToImage;
    //    args.pDstResource           = ToApi(pDstImage)->GetDxResource();
    //    args.dstSubresource         = subresourceIndex;
    //    args.dstX                   = static_cast<UINT>(pCopyInfo->dstImage.x);
    //    args.dstY                   = static_cast<UINT>(pCopyInfo->dstImage.y);
    //    args.dstZ                   = static_cast<UINT>(pCopyInfo->dstImage.z);
    //    args.pSrcResource           = ToApi(pSrcBuffer)->GetDxBuffer();
    //    args.srcSubresource         = 0;
    //}
}

void CommandBuffer::CopyImageToBuffer(
    const grfx::ImageToBufferCopyInfo* pCopyInfo,
    grfx::Image*                       pSrcImage,
    grfx::Buffer*                      pDstBuffer)
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
