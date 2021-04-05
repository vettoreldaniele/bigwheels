#include "ppx/grfx/dx11/dx11_queue.h"
#include "ppx/grfx/dx11/dx11_buffer.h"
#include "ppx/grfx/dx11/dx11_device.h"
#include "ppx/grfx/dx11/dx11_command.h"
#include "ppx/grfx/dx11/dx11_image.h"

#include "ppx/bitmap.h"

namespace ppx {
namespace grfx {
namespace dx11 {

Result Queue::CreateApiObjects(const grfx::internal::QueueCreateInfo* pCreateInfo)
{
    (void)pCreateInfo;

    mDeviceContext = ToApi(GetDevice())->GetDxDeviceContext();

    return ppx::SUCCESS;
}

void Queue::DestroyApiObjects()
{
}

Result Queue::WaitIdle()
{
    return ppx::SUCCESS;
}

void Queue::ResetGraphicsState()
{
    mViewportStateIndex           = kInvalidStateIndex;
    mScissorStateIndex            = kInvalidStateIndex;
    mIndexBufferStateIndex        = kInvalidStateIndex;
    mVertexBufferStateIndex       = kInvalidStateIndex;
    mGraphicsPipelineStateIndex   = kInvalidStateIndex;
    mGraphicsDescriptorStateIndex = kInvalidStateIndex;
}

static bool IndexChanged(uint32_t cmdStateIndex, uint32_t queueStateIndex)
{
    bool changed = (cmdStateIndex != kInvalidStateIndex) && (cmdStateIndex != queueStateIndex);
    return changed;
}

void Queue::SetGraphicsState(const grfx::dx11::CommandBuffer::ActionCmd& cmd, const grfx::dx11::CommandBuffer* pCmdBuf)
{
    if (IndexChanged(cmd.viewportStateIndex, mViewportStateIndex)) {
        mViewportStateIndex                                   = cmd.viewportStateIndex;
        const grfx::dx11::CommandBuffer::ViewportState& state = pCmdBuf->mViewportState.At(mViewportStateIndex);
        mDeviceContext->RSSetViewports(state.numViewports, state.viewports.data());
    }

    if (IndexChanged(cmd.scissorStateIndex, mScissorStateIndex)) {
        mScissorStateIndex                                   = cmd.scissorStateIndex;
        const grfx::dx11::CommandBuffer::ScissorState& state = pCmdBuf->mScissorState.At(mScissorStateIndex);
        mDeviceContext->RSSetScissorRects(state.numRects, state.rects.data());
    }

    if (IndexChanged(cmd.indexBuffereStateIndex, mIndexBufferStateIndex)) {
        mIndexBufferStateIndex                                   = cmd.indexBuffereStateIndex;
        const grfx::dx11::CommandBuffer::IndexBufferState& state = pCmdBuf->mIndexBufferState.At(mIndexBufferStateIndex);
        mDeviceContext->IASetIndexBuffer(state.buffer, state.format, state.offset);
    }

    if (IndexChanged(cmd.vertexBufferStateIndex, mVertexBufferStateIndex)) {
        mVertexBufferStateIndex                                   = cmd.vertexBufferStateIndex;
        const grfx::dx11::CommandBuffer::VertexBufferState& state = pCmdBuf->mVertexBuffersState.At(mVertexBufferStateIndex);
        mDeviceContext->IASetVertexBuffers(
            state.startSlot,
            state.numBuffers,
            state.buffers.data(),
            state.strides.data(),
            state.offsets.data());
    }

    if (IndexChanged(cmd.graphicsPipleineStateIndex, mGraphicsPipelineStateIndex)) {
        mGraphicsPipelineStateIndex                                   = cmd.graphicsPipleineStateIndex;
        const grfx::dx11::CommandBuffer::GraphicsPipelineState& state = pCmdBuf->mGraphicsPipelineState.At(mGraphicsPipelineStateIndex);

        if (!IsNull(state.VS)) {
            mDeviceContext->VSSetShader(state.VS, nullptr, 0);
        }
        if (!IsNull(state.HS)) {
            mDeviceContext->HSSetShader(state.HS, nullptr, 0);
        }
        if (!IsNull(state.DS)) {
            mDeviceContext->DSSetShader(state.DS, nullptr, 0);
        }
        if (!IsNull(state.GS)) {
            mDeviceContext->GSSetShader(state.GS, nullptr, 0);
        }
        if (!IsNull(state.PS)) {
            mDeviceContext->PSSetShader(state.PS, nullptr, 0);
        }

        mDeviceContext->IASetInputLayout(state.inputLayout);

        mDeviceContext->IASetPrimitiveTopology(state.primitiveTopology);

        mDeviceContext->RSSetState(state.rasterizerState);

        // @TODO: Figure out how to properly determine the StencilRef value
        //
        const UINT stencilRef = 1;
        mDeviceContext->OMSetDepthStencilState(state.depthStencilState, stencilRef);
    }

    if (IndexChanged(cmd.graphicsDescriptorstateIndex, mGraphicsDescriptorStateIndex)) {
        mGraphicsDescriptorStateIndex                                   = cmd.graphicsDescriptorstateIndex;
        const grfx::dx11::CommandBuffer::GraphicsDescriptorState& state = pCmdBuf->mGraphicsDescriptorState.At(mGraphicsDescriptorStateIndex);

        PPX_ASSERT_MSG(mGraphicsPipelineStateIndex != kInvalidStateIndex, "no pipeline bound");
        const grfx::dx11::CommandBuffer::GraphicsPipelineState& pipelineState = pCmdBuf->mGraphicsPipelineState.At(mGraphicsPipelineStateIndex);

        if (!IsNull(pipelineState.VS)) {
            for (size_t i = 0; i < state.VS.size(); ++i) {
                const grfx::dx11::CommandBuffer::ResourceSlotArray& slots = state.VS[i];
                switch (slots.descriptorType) {
                    default: {
                        PPX_ASSERT_MSG(false, "unknown descriptor type");
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_CBV: {
                        UINT                 numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11Buffer* const* ppResources  = reinterpret_cast<ID3D11Buffer* const*>(slots.resources.data());
                        mDeviceContext->VSSetConstantBuffers(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_SRV: {
                        UINT                             numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11ShaderResourceView* const* ppResources  = reinterpret_cast<ID3D11ShaderResourceView* const*>(slots.resources.data());
                        mDeviceContext->VSSetShaderResources(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_SAMPLER: {
                        UINT                       numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11SamplerState* const* ppResources  = reinterpret_cast<ID3D11SamplerState* const*>(slots.resources.data());
                        mDeviceContext->VSSetSamplers(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;
                }
            }
        }
        if (!IsNull(pipelineState.HS)) {
        }
        if (!IsNull(pipelineState.DS)) {
        }
        if (!IsNull(pipelineState.GS)) {
        }
        if (!IsNull(pipelineState.PS)) {
            for (size_t i = 0; i < state.PS.size(); ++i) {
                const grfx::dx11::CommandBuffer::ResourceSlotArray& slots = state.PS[i];
                switch (slots.descriptorType) {
                    default: {
                        PPX_ASSERT_MSG(false, "unknown descriptor type");
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_CBV: {
                        UINT                 numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11Buffer* const* ppResources  = reinterpret_cast<ID3D11Buffer* const*>(slots.resources.data());
                        mDeviceContext->PSSetConstantBuffers(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_SRV: {
                        UINT                             numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11ShaderResourceView* const* ppResources  = reinterpret_cast<ID3D11ShaderResourceView* const*>(slots.resources.data());
                        mDeviceContext->PSSetShaderResources(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_SAMPLER: {
                        UINT                       numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11SamplerState* const* ppResources  = reinterpret_cast<ID3D11SamplerState* const*>(slots.resources.data());
                        mDeviceContext->PSSetSamplers(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;
                }
            }
        }
    }
}

void Queue::ResetComputeState()
{
    mComputePipelineStateIndex   = kInvalidStateIndex;
    mComputeDescriptorStateIndex = kInvalidStateIndex;
}

void Queue::SetComputeState(const grfx::dx11::CommandBuffer::ActionCmd& cmd, const grfx::dx11::CommandBuffer* pCmdBuf)
{
    if (IndexChanged(cmd.computePipleineStateIndex, mComputePipelineStateIndex)) {
        mComputePipelineStateIndex                                   = cmd.computePipleineStateIndex;
        const grfx::dx11::CommandBuffer::ComputePipelineState& state = pCmdBuf->mComputePipelineState.At(mComputePipelineStateIndex);

        if (!IsNull(state.CS)) {
            mDeviceContext->CSSetShader(state.CS, nullptr, 0);
        }
    }

    if (IndexChanged(cmd.computeDescriptorstateIndex, mComputeDescriptorStateIndex)) {
        mComputeDescriptorStateIndex                                   = cmd.computeDescriptorstateIndex;
        const grfx::dx11::CommandBuffer::ComputeDescriptorState& state = pCmdBuf->mComputeDescriptorState.At(mComputeDescriptorStateIndex);

        PPX_ASSERT_MSG(mComputePipelineStateIndex != kInvalidStateIndex, "no pipeline bound");
        const grfx::dx11::CommandBuffer::ComputePipelineState& pipelineState = pCmdBuf->mComputePipelineState.At(mComputePipelineStateIndex);

        if (!IsNull(pipelineState.CS)) {
            for (size_t i = 0; i < state.CS.size(); ++i) {
                const grfx::dx11::CommandBuffer::ResourceSlotArray& slots = state.CS[i];
                switch (slots.descriptorType) {
                    default: {
                        PPX_ASSERT_MSG(false, "unknown descriptor type");
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_CBV: {
                        UINT                 numBuffers        = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11Buffer* const* ppConstantBuffers = reinterpret_cast<ID3D11Buffer* const*>(slots.resources.data());
                        mDeviceContext->CSSetConstantBuffers(
                            slots.startSlot,
                            numBuffers,
                            ppConstantBuffers);
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_SRV: {
                        UINT                             numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11ShaderResourceView* const* ppViews      = reinterpret_cast<ID3D11ShaderResourceView* const*>(slots.resources.data());
                        mDeviceContext->CSSetShaderResources(
                            slots.startSlot,
                            numResources,
                            ppViews);
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_UAV: {
                        UINT                              numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11UnorderedAccessView* const* ppViews      = reinterpret_cast<ID3D11UnorderedAccessView* const*>(slots.resources.data());
                        mDeviceContext->CSSetUnorderedAccessViews(
                            slots.startSlot,
                            numResources,
                            ppViews,
                            nullptr);

                        // Store the resource at bound slot so it can be unbound later
                        // if/when there is an image layout transition.
                        //
                        for (UINT i = 0; i < numResources; ++i) {
                            UINT slot = slots.startSlot + i;
                            ppViews[i]->GetResource(reinterpret_cast<ID3D11Resource**>(&mBoundUAVs[slot].pResource));
                        }
                    } break;

                    case grfx::D3D_DESCRIPTOR_TYPE_SAMPLER: {
                        UINT                       numResources = static_cast<UINT>(CountU32(slots.resources));
                        ID3D11SamplerState* const* ppResources  = reinterpret_cast<ID3D11SamplerState* const*>(slots.resources.data());
                        mDeviceContext->CSSetSamplers(
                            slots.startSlot,
                            numResources,
                            ppResources);
                    } break;
                }
            }
        }
    }
}

Result Queue::Submit(const grfx::SubmitInfo* pSubmitInfo)
{
    for (uint32_t cmdBufIndex = 0; cmdBufIndex < pSubmitInfo->commandBufferCount; ++cmdBufIndex) {
        const dx11::CommandBuffer* pCmdBuf = ToApi(pSubmitInfo->ppCommandBuffers[cmdBufIndex]);

        const std::vector<dx11::CommandBuffer::ActionCmd>& actionCmds = pCmdBuf->mActionCmds;

        const size_t numCmds = actionCmds.size();
        for (size_t cmdIndex = 0; cmdIndex < numCmds; ++cmdIndex) {
            ResetGraphicsState();
            ResetComputeState();

            const dx11::CommandBuffer::ActionCmd& cmd = actionCmds[cmdIndex];
            switch (cmd.cmd) {
                default: {
                    PPX_ASSERT_MSG(false, "unknown command");
                } break;

                case CMD_BEGIN_RENDER_PASS: {
                    const dx11::CommandBuffer::RenderPassArgs& args = cmd.args.renderPass;

                    mDeviceContext->OMSetRenderTargets(
                        static_cast<UINT>(args.rtvs.numViews),
                        args.rtvs.views.data(),
                        args.dsv.pView);

                    for (UINT i = 0; i < args.rtvs.numViews; ++i) {
                        if (args.rtvs.loadOps[i] == grfx::ATTACHMENT_LOAD_OP_CLEAR) {
                            mDeviceContext->ClearRenderTargetView(
                                args.rtvs.views[i],
                                args.rtvs.clearValues[i].rgba.data());
                        }
                    }

                    if (!IsNull(args.dsv.pView)) {
                        // clang-format off
                        UINT clearFlags = 0;
                        if (args.dsv.depthLoadOp == grfx::ATTACHMENT_LOAD_OP_CLEAR)   clearFlags |= D3D11_CLEAR_DEPTH;
                        if (args.dsv.stencilLoadOp == grfx::ATTACHMENT_LOAD_OP_CLEAR) clearFlags |= D3D11_CLEAR_STENCIL;
                        // clang-format on
                        if (clearFlags != 0) {
                            mDeviceContext->ClearDepthStencilView(
                                args.dsv.pView,
                                clearFlags,
                                args.dsv.clearValue.depth,
                                args.dsv.clearValue.stencil);
                        }
                    }
                } break;

                case CMD_TRANSITION_IMAGE_LAYOUT: {
                    const dx11::CommandBuffer::TransitionArgs& args = cmd.args.transition;
                    for (size_t i = 0; i < mBoundUAVs.size(); ++i) {
                        if (args.resource == mBoundUAVs[i].pResource) {
                            UINT                       slot        = static_cast<UINT>(i);
                            ID3D11UnorderedAccessView* nullUAVs[1] = {nullptr};
                            mDeviceContext->CSSetUnorderedAccessViews(slot, 1, nullUAVs, nullptr);
                        }
                    }
                } break;

                case CMD_DRAW: {
                    SetGraphicsState(cmd, pCmdBuf);

                    const dx11::CommandBuffer::DrawArgs& args = cmd.args.draw;

                    mDeviceContext->DrawInstanced(
                        args.vertexCountPerInstance,
                        args.instanceCount,
                        args.startVertexLocation,
                        args.startInstanceLocation);
                } break;

                case CMD_DRAW_INDEXED: {
                    SetGraphicsState(cmd, pCmdBuf);

                    const dx11::CommandBuffer::DrawIndexedArgs& args = cmd.args.drawIndexed;

                    mDeviceContext->DrawIndexedInstanced(
                        args.indexCountPerInstance,
                        args.instanceCount,
                        args.startIndexLocation,
                        args.baseVertexLocation,
                        args.startInstanceLocation);

                } break;

                case CMD_DISPATCH: {
                    SetComputeState(cmd, pCmdBuf);

                    const dx11::CommandBuffer::DispatchArgs& args = cmd.args.dispatch;

                    mDeviceContext->Dispatch(
                        args.threadGroupCountX,
                        args.threadGroupCountY,
                        args.threadGroupCountZ);

                } break;

                case CMD_COPY_BUFFER_TO_BUFFER: {
                    const dx11::CommandBuffer::CopyBufferToBufferArgs& args = cmd.args.copyBufferToBuffer;

                    ID3D11Resource* pSrcResource = ToApi(args.pSrcBuffer)->GetDxBuffer();
                    ID3D11Resource* pDstResource = ToApi(args.pDstBuffer)->GetDxBuffer();

                    D3D11_BOX srcBox = {};
                    srcBox.left      = static_cast<UINT>(args.copyInfo.srcBuffer.offset);
                    srcBox.top       = 0;
                    srcBox.front     = 0;
                    srcBox.right     = static_cast<UINT>(args.copyInfo.srcBuffer.offset + args.copyInfo.size);
                    srcBox.bottom    = 1;
                    srcBox.back      = 1;

                    mDeviceContext->CopySubresourceRegion1(
                        pDstResource,
                        0,
                        static_cast<UINT>(args.copyInfo.dstBuffer.offset),
                        0,
                        0,
                        pSrcResource,
                        0,
                        &srcBox,
                        D3D11_COPY_DISCARD);

                } break;

                case CMD_COPY_BUFFER_TO_IMAGE: {
                    const dx11::CommandBuffer::CopyBufferToImageArgs& args = cmd.args.copyBufferToImage;

                    char*   pMappedAddress = nullptr;
                    HRESULT hr             = args.pSrcBuffer->MapMemory(0, reinterpret_cast<void**>(&pMappedAddress));
                    if (FAILED(hr)) {
                        PPX_ASSERT_MSG(false, "could not map memory");
                    }

                    ID3D11Resource* pDstResource  = ToApi(args.pDstImage)->GetDxResource();
                    uint32_t        mipLevelCount = args.pDstImage->GetMipLevelCount();

                    for (uint32_t i = 0; i < args.copyInfo.dstImage.arrayLayerCount; ++i) {
                        uint32_t arrayLayer       = args.copyInfo.dstImage.arrayLayer + i;
                        UINT     subresourceIndex = static_cast<UINT>((args.copyInfo.dstImage.arrayLayer * mipLevelCount) + args.copyInfo.dstImage.mipLevel);

                        D3D11_BOX dstBox = {};
                        dstBox.left      = args.copyInfo.dstImage.x;
                        dstBox.top       = args.copyInfo.dstImage.y;
                        dstBox.front     = args.copyInfo.dstImage.z;
                        dstBox.right     = args.copyInfo.dstImage.x + args.copyInfo.dstImage.width;
                        dstBox.bottom    = args.copyInfo.dstImage.y + args.copyInfo.dstImage.height;
                        dstBox.back      = args.copyInfo.dstImage.z + args.copyInfo.dstImage.depth;

                        const char* pSrcData = pMappedAddress + args.copyInfo.srcBuffer.footprintOffset;

                        UINT srcRowPitch   = static_cast<UINT>(args.copyInfo.srcBuffer.imageRowStride);
                        UINT srcDepthPitch = static_cast<UINT>(args.copyInfo.srcBuffer.imageRowStride * args.copyInfo.srcBuffer.imageHeight);

                        D3D11_COPY_FLAGS copyFlags = D3D11_COPY_DISCARD;
                        if (args.pDstImage->GetType() == grfx::IMAGE_TYPE_CUBE) {
                            copyFlags = D3D11_COPY_NO_OVERWRITE;
                        }

                        mDeviceContext->UpdateSubresource1(
                            pDstResource,
                            subresourceIndex,
                            &dstBox,
                            pSrcData,
                            srcRowPitch,
                            srcDepthPitch,
                            copyFlags);
                    }

                    args.pSrcBuffer->UnmapMemory();
                } break;

                case CMD_IM_GUI_RENDER: {
                    void (*pRenderFn)(void) = cmd.args.imGuiRender.pRenderFn;
                    pRenderFn();
                } break;
            }
        }
    }

    return ppx::SUCCESS;
}

Result Queue::GetTimestampFrequency(uint64_t* pFrequency) const
{
    return ppx::ERROR_FAILED;
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
