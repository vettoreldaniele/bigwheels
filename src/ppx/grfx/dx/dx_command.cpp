#include "ppx/grfx/dx/dx_command.h"
#include "ppx/grfx/dx/dx_buffer.h"
#include "ppx/grfx/dx/dx_device.h"
#include "ppx/grfx/dx/dx_image.h"
#include "ppx/grfx/dx/dx_pipeline.h"
#include "ppx/grfx/dx/dx_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx {

// -------------------------------------------------------------------------------------------------
// CommandBuffer
// -------------------------------------------------------------------------------------------------
Result CommandBuffer::CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo)
{
    UINT                     nodeMask = 0;
    D3D12_COMMAND_LIST_TYPE  type     = ToApi(pCreateInfo->pPool)->GetDxCommandType();
    D3D12_COMMAND_LIST_FLAGS flags    = D3D12_COMMAND_LIST_FLAG_NONE;

    // NOTE: CreateCommandList1 creates a command list in closed state. No need to
    //       call Close() it after creation unlike command lists created with
    //       CreateCommandList.
    //
    HRESULT hr = ToApi(GetDevice())->GetDxDevice()->CreateCommandList1(nodeMask, type, flags, IID_PPV_ARGS(&mCommandList));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateCommandList1 failed");
        return ppx::ERROR_API_FAILURE;
    }

    // Store command allocator for reset
    mCommandAllocator = ToApi(pCreateInfo->pPool)->GetDxCommandAllocator();

    return ppx::SUCCESS;
}

void CommandBuffer::DestroyApiObjects()
{
    if (mCommandList) {
        mCommandList.Reset();
    }
}

Result CommandBuffer::Begin()
{
    // Command allocators can only be reset when the GPU is
    // done with associated with command lists.
    //
    HRESULT hr = mCommandAllocator->Reset();
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12CommandAllocator::Reset failed");
        return ppx::ERROR_API_FAILURE;
    }

    // Normally a command list can be reset immediately after submission
    // if it gets associated with a different command allocoator.
    // But since we're trying to align with Vulkan, just keep the
    // command allocator and command list paired.
    //
    hr = mCommandList->Reset(mCommandAllocator.Get(), nullptr);
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12CommandList::Reset failed");
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

Result CommandBuffer::End()
{
    HRESULT hr = mCommandList->Close();
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12CommandList::Close failed");
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void CommandBuffer::BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo)
{
    PPX_ASSERT_NULL_ARG(pBeginInfo->pRenderPass);

    const grfx::RenderPass* pRenderPass = pBeginInfo->pRenderPass;

    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptors[PPX_MAX_RENDER_TARGETS] = {};

    // Get handle to render target descirptors
    uint32_t renderTargetCount = pRenderPass->GetRenderTargetCount();
    for (uint32_t i = 0; i < renderTargetCount; ++i) {
        dx::RenderTargetView* pRTV = ToApi(pRenderPass->GetRenderTargetView(i).Get());
        renderTargetDescriptors[i] = pRTV->GetCpuDescriptorHandle();
    }

    // Set render targets
    mCommandList->OMSetRenderTargets(
        static_cast<UINT>(renderTargetCount),
        renderTargetDescriptors,
        FALSE,
        nullptr);

    // Clear render targets if load op is clear
    renderTargetCount = std::min(renderTargetCount, pBeginInfo->RTVClearCount);
    for (uint32_t i = 0; i < renderTargetCount; ++i) {
        grfx::AttachmentLoadOp loadOp = pRenderPass->GetRenderTargetView(i)->GetLoadOp();
        if (loadOp == grfx::ATTACHMENT_LOAD_OP_CLEAR) {
            const D3D12_CPU_DESCRIPTOR_HANDLE&  handle     = renderTargetDescriptors[i];
            const grfx::RenderTargetClearValue& clearValue = pBeginInfo->RTVClearValues[i];
            mCommandList->ClearRenderTargetView(handle, clearValue.rgba, 0, nullptr);
        }
    }
}

void CommandBuffer::EndRenderPass()
{
    // Nothing to do here for now
}

void CommandBuffer::TransitionImageLayout(
    const grfx::Image*  pImage,
    uint32_t            mipLevel,
    uint32_t            mipLevelCount,
    uint32_t            arrayLayer,
    uint32_t            arrayLayerCount,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState)
{
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = ToApi(pImage)->GetDxImage().Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = ToD3D12ResourceStates(beforeState);
    barrier.Transition.StateAfter  = ToD3D12ResourceStates(afterState);

    mCommandList->ResourceBarrier(1, &barrier);
}

void CommandBuffer::SetViewports(
    uint32_t              viewportCount,
    const grfx::Viewport* pViewports)
{
    D3D12_VIEWPORT viewports[PPX_MAX_VIEWPORTS] = {};
    for (uint32_t i = 0; i < viewportCount; ++i) {
        viewports[i].TopLeftX = pViewports[i].x;
        viewports[i].TopLeftY = pViewports[i].y;
        viewports[i].Width    = pViewports[i].width;
        viewports[i].Height   = pViewports[i].height;
        viewports[i].MinDepth = pViewports[i].minDepth;
        viewports[i].MaxDepth = pViewports[i].maxDepth;
    }

    mCommandList->RSSetViewports(static_cast<UINT>(viewportCount), viewports);
}

void CommandBuffer::SetScissors(
    uint32_t          scissorCount,
    const grfx::Rect* pScissors)
{
    D3D12_RECT rects[PPX_MAX_SCISSORS] = {};
    for (uint32_t i = 0; i < scissorCount; ++i) {
        rects[i].left   = pScissors[i].x;
        rects[i].top    = pScissors[i].y;
        rects[i].right  = pScissors[i].x + pScissors[i].width;
        rects[i].bottom = pScissors[i].y + pScissors[i].height;
    }

    mCommandList->RSSetScissorRects(static_cast<UINT>(scissorCount), rects);
}

void CommandBuffer::BindGraphicsDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    mCommandList->SetGraphicsRootSignature(ToApi(pInterface)->GetDxRootSignature().Get());
}

void CommandBuffer::BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)
{
    mCommandList->SetPipelineState(ToApi(pPipeline)->GetDxPipeline().Get());
    mCommandList->IASetPrimitiveTopology(ToApi(pPipeline)->GetPrimitiveTopology());
}

void CommandBuffer::BindComputeDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindComputePipeline(const grfx::ComputePipeline* pPipeline)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BindIndexBuffer(const grfx::IndexBufferView* pView)
{
}

void CommandBuffer::BindVertexBuffers(
    uint32_t                      viewCount,
    const grfx::VertexBufferView* pViews)
{
    D3D12_VERTEX_BUFFER_VIEW views[PPX_MAX_RENDER_TARGETS] = {};
    for (uint32_t i = 0; i < viewCount; ++i) {
        views[i].BufferLocation = ToApi(pViews[i].pBuffer)->GetDxResource()->GetGPUVirtualAddress();
        views[i].SizeInBytes    = static_cast<UINT>(pViews[i].pBuffer->GetSize());
        views[i].StrideInBytes  = static_cast<UINT>(pViews[i].stride);
    }

    mCommandList->IASetVertexBuffers(
        0,
        static_cast<UINT>(viewCount),
        views);
}

void CommandBuffer::Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    mCommandList->DrawInstanced(
        static_cast<UINT>(vertexCount),
        static_cast<UINT>(instanceCount),
        static_cast<UINT>(firstVertex),
        static_cast<UINT>(firstInstance));
}

void CommandBuffer::DrawIndexed(
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t  vertexOffset,
    uint32_t firstInstance)
{
}

void CommandBuffer::Dispatch(
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
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

// -------------------------------------------------------------------------------------------------
// CommandPool
// -------------------------------------------------------------------------------------------------
Result CommandPool::CreateApiObjects(const grfx::CommandPoolCreateInfo* pCreateInfo)
{
    // clang-format off
    switch (pCreateInfo->pQueue->GetCommandType()) {
        default: break;
        case grfx::COMMAND_TYPE_GRAPHICS: mCommandType = D3D12_COMMAND_LIST_TYPE_DIRECT; break;
        case grfx::COMMAND_TYPE_COMPUTE : mCommandType = D3D12_COMMAND_LIST_TYPE_COMPUTE; break;
        case grfx::COMMAND_TYPE_TRANSFER: mCommandType = D3D12_COMMAND_LIST_TYPE_COPY; break;
    }
    // clang-format on
    if (mCommandType == ppx::InvalidValue<D3D12_COMMAND_LIST_TYPE>()) {
        PPX_ASSERT_MSG(false, "invalid command type");
        return ppx::ERROR_INVALID_CREATE_ARGUMENT;
    }

    HRESULT hr = ToApi(GetDevice())->GetDxDevice()->CreateCommandAllocator(mCommandType, IID_PPV_ARGS(&mCommandAllocator));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateCommandAllocator failed");
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void CommandPool::DestroyApiObjects()
{
    if (mCommandAllocator) {
        mCommandAllocator.Reset();
    }
}

} // namespace dx
} // namespace grfx
} // namespace ppx
