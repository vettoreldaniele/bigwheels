#include "ppx/grfx/dx12/dx12_command.h"
#include "ppx/grfx/dx12/dx12_buffer.h"
#include "ppx/grfx/dx12/dx12_descriptor.h"
#include "ppx/grfx/dx12/dx12_device.h"
#include "ppx/grfx/dx12/dx12_image.h"
#include "ppx/grfx/dx12/dx12_pipeline.h"
#include "ppx/grfx/dx12/dx12_query.h"
#include "ppx/grfx/dx12/dx12_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx12 {

// -------------------------------------------------------------------------------------------------
// CommandBuffer
// -------------------------------------------------------------------------------------------------
Result CommandBuffer::CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo)
{
    D3D12DevicePtr device = ToApi(GetDevice())->GetDxDevice();

    UINT                     nodeMask = 0;
    D3D12_COMMAND_LIST_TYPE  type     = ToApi(pCreateInfo->pPool)->GetDxCommandType();
    D3D12_COMMAND_LIST_FLAGS flags    = D3D12_COMMAND_LIST_FLAG_NONE;

    // NOTE: CreateCommandList1 creates a command list in closed state. No need to
    //       call Close() it after creation unlike command lists created with
    //       CreateCommandList.
    //
    HRESULT hr = device->CreateCommandList1(nodeMask, type, flags, IID_PPV_ARGS(&mCommandList));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateCommandList1 failed");
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(D3D12GraphicsCommandList, mCommandList.Get());

    // Store command allocator for reset
    mCommandAllocator = ToApi(pCreateInfo->pPool)->GetDxCommandAllocator();

    // Heap sizes
    mHeapSizeCBVSRVUAV = static_cast<UINT>(pCreateInfo->resourceDescriptorCount);
    mHeapSizeSampler   = static_cast<UINT>(pCreateInfo->samplerDescriptorCount);

    // Allocate CBVSRVUAV heap
    if (mHeapSizeCBVSRVUAV > 0) {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors             = mHeapSizeCBVSRVUAV;
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask                   = 0;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapCBVSRVUAV));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(CBVSRVUAV) failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Allocate Sampler heap
    if (mHeapSizeSampler > 0) {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors             = mHeapSizeSampler;
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask                   = 0;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapSampler));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(Sampler) failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    return ppx::SUCCESS;
}

void CommandBuffer::DestroyApiObjects()
{
    if (mCommandList) {
        mCommandList.Reset();
    }

    if (mCommandAllocator) {
        mCommandAllocator.Reset();
    }

    if (mHeapCBVSRVUAV) {
        mHeapCBVSRVUAV.Reset();
    }

    if (mHeapSampler) {
        mHeapSampler.Reset();
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

    // Set descriptor heaps
    ID3D12DescriptorHeap* heaps[2]  = {nullptr};
    uint32_t              heapCount = 0;
    if (mHeapCBVSRVUAV) {
        heaps[heapCount] = mHeapCBVSRVUAV.Get();
        ++heapCount;
    }
    if (mHeapSampler) {
        heaps[heapCount] = mHeapSampler.Get();
        ++heapCount;
    }
    if (heapCount > 0) {
        mCommandList->SetDescriptorHeaps(heapCount, heaps);
    }

    // Reset heap offsets
    mHeapOffsetCBVSRVUAV = 0;
    mHeapOffsetSampler   = 0;

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

void CommandBuffer::BeginRenderPassImpl(const grfx::RenderPassBeginInfo* pBeginInfo)
{
    PPX_ASSERT_NULL_ARG(pBeginInfo->pRenderPass);

    const grfx::RenderPass* pRenderPass = pBeginInfo->pRenderPass;

    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptors[PPX_MAX_RENDER_TARGETS] = {};
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDesciptor                           = {};

    // Get handle to render target descirptors
    uint32_t renderTargetCount = pRenderPass->GetRenderTargetCount();
    for (uint32_t i = 0; i < renderTargetCount; ++i) {
        dx12::RenderTargetView* pRTV = ToApi(pRenderPass->GetRenderTargetView(i).Get());
        renderTargetDescriptors[i]   = pRTV->GetCpuDescriptorHandle();
    }

    // Get handle for depth stencil descriptor
    bool hasDepthStencil = false;
    if (pRenderPass->GetDepthStencilView()) {
        depthStencilDesciptor = ToApi(pRenderPass->GetDepthStencilView())->GetCpuDescriptorHandle();
        hasDepthStencil       = true;
    }

    // Set render targets
    mCommandList->OMSetRenderTargets(
        static_cast<UINT>(renderTargetCount),
        renderTargetDescriptors,
        FALSE,
        hasDepthStencil ? &depthStencilDesciptor : nullptr);

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

    // Clear depth/stencil if load op is clear
    if (hasDepthStencil && (pRenderPass->GetDepthStencilView()->GetDepthLoadOp() == grfx::ATTACHMENT_LOAD_OP_CLEAR)) {
        D3D12_CLEAR_FLAGS                   flags      = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
        const grfx::DepthStencilClearValue& clearValue = pBeginInfo->DSVClearValue;
        mCommandList->ClearDepthStencilView(
            depthStencilDesciptor,
            flags,
            static_cast<FLOAT>(clearValue.depth),
            static_cast<UINT8>(clearValue.stencil),
            0,
            nullptr);
    }
}

void CommandBuffer::EndRenderPassImpl()
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
    grfx::ResourceState afterState,
    const grfx::Queue*  pSrcQueue,
    const grfx::Queue*  pDstQueue)
{
    PPX_ASSERT_NULL_ARG(pImage);

    (void)pSrcQueue;
    (void)pDstQueue;

    if (beforeState == afterState) {
        return;
    }

    bool allMipLevels    = (mipLevel == 0) && (mipLevelCount == PPX_ALL_MIP_LEVELS);
    bool allArrayLayers  = (arrayLayer == 0) && (arrayLayerCount == PPX_ALL_ARRAY_LAYERS);
    bool allSubresources = allMipLevels && allArrayLayers;

    std::vector<D3D12_RESOURCE_BARRIER> barriers;
    if (allSubresources) {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = ToApi(pImage)->GetDxResource();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = ToD3D12ResourceStates(beforeState);
        barrier.Transition.StateAfter  = ToD3D12ResourceStates(afterState);

        barriers.push_back(barrier);
    }
    else {
        //
        // For details about subresource indexing see this:
        //   https://docs.microsoft.com/en-us/windows/win32/direct3d12/subresources
        //

        uint32_t mipSpan = pImage->GetMipLevelCount();

        for (uint32_t i = 0; i < arrayLayerCount; ++i) {
            uint32_t baseSubresource = (arrayLayer + i) * mipSpan;
            for (uint32_t j = 0; j < mipLevelCount; ++j) {
                uint32_t targetSubResource = baseSubresource + (mipLevel + j);

                D3D12_RESOURCE_BARRIER barrier = {};
                barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource   = ToApi(pImage)->GetDxResource();
                barrier.Transition.Subresource = static_cast<UINT>(targetSubResource);
                barrier.Transition.StateBefore = ToD3D12ResourceStates(beforeState);
                barrier.Transition.StateAfter  = ToD3D12ResourceStates(afterState);

                barriers.push_back(barrier);
            }
        }
    }

    if (barriers.empty()) {
        PPX_ASSERT_MSG(false, "parameters resulted in no barriers - try not to do this!")
    }

    mCommandList->ResourceBarrier(
        static_cast<UINT>(barriers.size()),
        DataPtr(barriers));
}

void CommandBuffer::BufferResourceBarrier(
    const grfx::Buffer* pBuffer,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState,
    const grfx::Queue*  pSrcQueue,
    const grfx::Queue*  pDstQueue)
{
    PPX_ASSERT_NULL_ARG(pBuffer);

    (void)pSrcQueue;
    (void)pDstQueue;

    if (beforeState == afterState) {
        return;
    }
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = ToApi(pBuffer)->GetDxResource();
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

void CommandBuffer::BindDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets,
    size_t&                           rdtCountCBVSRVUAV,
    size_t&                           rdtCountSampler)
{
    dx12::Device*                  pApiDevice             = ToApi(GetDevice());
    D3D12DevicePtr                 device                 = pApiDevice->GetDxDevice();
    const dx12::PipelineInterface* pApiPipelineInterface  = ToApi(pInterface);
    const std::vector<uint32_t>&   setNumbers             = pApiPipelineInterface->GetSetNumbers();
    UINT                           incrementSizeCBVSRVUAV = pApiDevice->GetHandleIncrementSizeCBVSRVUAV();
    UINT                           incrementSizeSampler   = pApiDevice->GetHandleIncrementSizeSampler();

    uint32_t parameterIndexCount = pApiPipelineInterface->GetParameterIndexCount();
    if (parameterIndexCount > mRootDescriptorTablesCBVSRVUAV.size()) {
        mRootDescriptorTablesCBVSRVUAV.resize(parameterIndexCount);
        mRootDescriptorTablesSampler.resize(parameterIndexCount);
    }

    // Root descriptor tables
    rdtCountCBVSRVUAV = 0;
    rdtCountSampler   = 0;
    for (uint32_t setIndex = 0; setIndex < setCount; ++setIndex) {
        PPX_ASSERT_MSG(ppSets[setIndex] != nullptr, "ppSets[" << setIndex << "] is null");
        uint32_t                   set      = setNumbers[setIndex];
        const dx12::DescriptorSet* pApiSet  = ToApi(ppSets[setIndex]);
        auto&                      bindings = pApiSet->GetLayout()->GetBindings();

        // Copy the descriptors
        {
            UINT numDescriptors = pApiSet->GetNumDescriptorsCBVSRVUAV();
            if (numDescriptors > 0) {
                D3D12_CPU_DESCRIPTOR_HANDLE dstRangeStart = mHeapCBVSRVUAV->GetCPUDescriptorHandleForHeapStart();
                D3D12_CPU_DESCRIPTOR_HANDLE srcRangeStart = pApiSet->GetHeapCBVSRVUAV()->GetCPUDescriptorHandleForHeapStart();

                dstRangeStart.ptr += (mHeapOffsetCBVSRVUAV * incrementSizeCBVSRVUAV);

                device->CopyDescriptorsSimple(
                    numDescriptors,
                    dstRangeStart,
                    srcRangeStart,
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            numDescriptors = pApiSet->GetNumDescriptorsSampler();
            if (numDescriptors > 0) {
                D3D12_CPU_DESCRIPTOR_HANDLE dstRangeStart = mHeapSampler->GetCPUDescriptorHandleForHeapStart();
                D3D12_CPU_DESCRIPTOR_HANDLE srcRangeStart = pApiSet->GetHeapSampler()->GetCPUDescriptorHandleForHeapStart();

                dstRangeStart.ptr += (mHeapOffsetSampler * incrementSizeSampler);

                device->CopyDescriptorsSimple(
                    numDescriptors,
                    dstRangeStart,
                    srcRangeStart,
                    D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            }
        }

        size_t bindingCount = bindings.size();
        for (size_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex) {
            auto& binding        = bindings[bindingIndex];
            UINT  parameterIndex = pApiPipelineInterface->FindParameterIndex(set, binding.binding);
            PPX_ASSERT_MSG(parameterIndex != UINT32_MAX, "invalid parameter index for set=" << set << ", binding=" << binding.binding);

            if (binding.type == grfx::DESCRIPTOR_TYPE_SAMPLER) {
                RootDescriptorTable& rdt = mRootDescriptorTablesSampler[rdtCountSampler];
                rdt.parameterIndex       = parameterIndex;
                rdt.baseDescriptor       = mHeapSampler->GetGPUDescriptorHandleForHeapStart();
                rdt.baseDescriptor.ptr += (mHeapOffsetSampler * incrementSizeSampler);

                mHeapOffsetSampler += static_cast<UINT>(binding.arrayCount);
                rdtCountSampler += 1;
            }
            else {
                RootDescriptorTable& rdt = mRootDescriptorTablesCBVSRVUAV[rdtCountCBVSRVUAV];
                rdt.parameterIndex       = parameterIndex;
                rdt.baseDescriptor       = mHeapCBVSRVUAV->GetGPUDescriptorHandleForHeapStart();
                rdt.baseDescriptor.ptr += (mHeapOffsetCBVSRVUAV * incrementSizeCBVSRVUAV);

                mHeapOffsetCBVSRVUAV += static_cast<UINT>(binding.arrayCount);
                rdtCountCBVSRVUAV += 1;
            }
        }
    }
}

void CommandBuffer::BindGraphicsDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    // Set root signature
    mCommandList->SetGraphicsRootSignature(ToApi(pInterface)->GetDxRootSignature().Get());

    // Fill out mRootDescriptorTablesCBVSRVUAV and mRootDescriptorTablesSampler
    size_t rdtCountCBVSRVUAV = 0;
    size_t rdtCountSampler   = 0;
    BindDescriptorSets(pInterface, setCount, ppSets, rdtCountCBVSRVUAV, rdtCountSampler);

    // Set CBVSRVUAV root descriptor tables
    for (uint32_t i = 0; i < rdtCountCBVSRVUAV; ++i) {
        const RootDescriptorTable& rdt = mRootDescriptorTablesCBVSRVUAV[i];
        mCommandList->SetGraphicsRootDescriptorTable(rdt.parameterIndex, rdt.baseDescriptor);
    }

    // Set Sampler root descriptor tables
    for (uint32_t i = 0; i < rdtCountSampler; ++i) {
        const RootDescriptorTable& rdt = mRootDescriptorTablesSampler[i];
        mCommandList->SetGraphicsRootDescriptorTable(rdt.parameterIndex, rdt.baseDescriptor);
    }
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
    // Set root signature
    mCommandList->SetComputeRootSignature(ToApi(pInterface)->GetDxRootSignature().Get());

    // Fill out mRootDescriptorTablesCBVSRVUAV and mRootDescriptorTablesSampler
    size_t rdtCountCBVSRVUAV = 0;
    size_t rdtCountSampler   = 0;
    BindDescriptorSets(pInterface, setCount, ppSets, rdtCountCBVSRVUAV, rdtCountSampler);

    // Set CBVSRVUAV root descriptor tables
    for (uint32_t i = 0; i < rdtCountCBVSRVUAV; ++i) {
        const RootDescriptorTable& rdt = mRootDescriptorTablesCBVSRVUAV[i];
        mCommandList->SetComputeRootDescriptorTable(rdt.parameterIndex, rdt.baseDescriptor);
    }

    // Set Sampler root descriptor tables
    for (uint32_t i = 0; i < rdtCountSampler; ++i) {
        const RootDescriptorTable& rdt = mRootDescriptorTablesSampler[i];
        mCommandList->SetComputeRootDescriptorTable(rdt.parameterIndex, rdt.baseDescriptor);
    }
}

void CommandBuffer::BindComputePipeline(const grfx::ComputePipeline* pPipeline)
{
    mCommandList->SetPipelineState(ToApi(pPipeline)->GetDxPipeline().Get());
}

void CommandBuffer::BindIndexBuffer(const grfx::IndexBufferView* pView)
{
    D3D12_INDEX_BUFFER_VIEW view = {};
    view.BufferLocation          = ToApi(pView->pBuffer)->GetDxResource()->GetGPUVirtualAddress();
    view.SizeInBytes             = static_cast<UINT>(pView->pBuffer->GetSize());
    view.Format                  = ToD3D12IndexFormat(pView->indexType);
    PPX_ASSERT_MSG(view.Format != DXGI_FORMAT_UNKNOWN, "unknown index  format");

    mCommandList->IASetIndexBuffer(&view);
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
    mCommandList->DrawIndexedInstanced(
        static_cast<UINT>(indexCount),
        static_cast<UINT>(instanceCount),
        static_cast<UINT>(firstIndex),
        static_cast<UINT>(vertexOffset),
        static_cast<UINT>(firstInstance));
}

void CommandBuffer::Dispatch(
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    mCommandList->Dispatch(
        static_cast<UINT>(groupCountX),
        static_cast<UINT>(groupCountY),
        static_cast<UINT>(groupCountZ));
}

void CommandBuffer::CopyBufferToBuffer(
    const grfx::BufferToBufferCopyInfo* pCopyInfo,
    grfx::Buffer*                       pSrcBuffer,
    grfx::Buffer*                       pDstBuffer)
{
    mCommandList->CopyBufferRegion(
        ToApi(pDstBuffer)->GetDxResource(),
        static_cast<UINT64>(pCopyInfo->dstBuffer.offset),
        ToApi(pSrcBuffer)->GetDxResource(),
        static_cast<UINT64>(pCopyInfo->srcBuffer.offset),
        static_cast<UINT64>(pCopyInfo->size));
}

void CommandBuffer::CopyBufferToImage(
    const grfx::BufferToImageCopyInfo* pCopyInfo,
    grfx::Buffer*                      pSrcBuffer,
    grfx::Image*                       pDstImage)
{
    D3D12DevicePtr      device        = ToApi(GetDevice())->GetDxDevice();
    D3D12_RESOURCE_DESC resouceDesc   = ToApi(pDstImage)->GetDxResource()->GetDesc();
    const uint32_t      mipLevelCount = pDstImage->GetMipLevelCount();

    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource                   = ToApi(pDstImage)->GetDxResource();
    dst.Type                        = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource                   = ToApi(pSrcBuffer)->GetDxResource();
    src.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    for (uint32_t i = 0; i < pCopyInfo->dstImage.arrayLayerCount; ++i) {
        uint32_t arrayLayer = pCopyInfo->dstImage.arrayLayer + i;

        dst.SubresourceIndex = static_cast<UINT>((arrayLayer * mipLevelCount) + pCopyInfo->dstImage.mipLevel);

        UINT   numSubresources = 1;
        UINT   numRows         = 0;
        UINT64 rowSizeInBytes  = 0;
        UINT64 totalBytes      = 0;

        // Grab the format
        device->GetCopyableFootprints(
            &resouceDesc,
            dst.SubresourceIndex,
            numSubresources,
            static_cast<UINT64>(pCopyInfo->srcBuffer.footprintOffset),
            &src.PlacedFootprint,
            &numRows,
            &rowSizeInBytes,
            &totalBytes);
        //
        // Replace the values in case the footprint is a submimage
        //
        // NOTE: D3D12's debug layer will throw an error if RowPitch
        //       isn't aligned to D3D12_TEXTURE_DATA_PITCH_ALIGNMENT(256).
        //       But generally, we want to do this in the calling code
        //       and not here.
        //
        src.PlacedFootprint.Offset             = static_cast<UINT64>(pCopyInfo->srcBuffer.footprintOffset);
        src.PlacedFootprint.Footprint.Width    = static_cast<UINT>(pCopyInfo->srcBuffer.footprintWidth);
        src.PlacedFootprint.Footprint.Height   = static_cast<UINT>(pCopyInfo->srcBuffer.footprintHeight);
        src.PlacedFootprint.Footprint.Depth    = static_cast<UINT>(pCopyInfo->srcBuffer.footprintDepth);
        src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(pCopyInfo->srcBuffer.imageRowStride);

        mCommandList->CopyTextureRegion(
            &dst,
            static_cast<UINT>(pCopyInfo->dstImage.x),
            static_cast<UINT>(pCopyInfo->dstImage.y),
            static_cast<UINT>(pCopyInfo->dstImage.z),
            &src,
            nullptr);
    }
}

void CommandBuffer::CopyImageToBuffer(
    const grfx::ImageToBufferCopyInfo* pCopyInfo,
    grfx::Image*                       pSrcImage,
    grfx::Buffer*                      pDstBuffer)
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
    PPX_LOG_OBJECT_CREATION(D3D12CommandAllocator, mCommandAllocator.Get());

    return ppx::SUCCESS;
}

void CommandPool::DestroyApiObjects()
{
    if (mCommandAllocator) {
        mCommandAllocator.Reset();
    }
}

} // namespace dx12
} // namespace grfx
} // namespace ppx
