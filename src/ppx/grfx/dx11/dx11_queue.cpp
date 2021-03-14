#include "ppx/grfx/dx11/dx11_queue.h"
#include "ppx/grfx/dx11/dx11_device.h"
#include "ppx/grfx/dx11/dx11_command.h"

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
    mViewportStateIndex         = kInvalidStateIndex;
    mScissorStateIndex          = kInvalidStateIndex;
    mIndexBufferStateIndex      = kInvalidStateIndex;
    mVertexBufferStateIndex     = kInvalidStateIndex;
    mGraphicsPipelineStateIndex = kInvalidStateIndex;
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
        mVertexBufferStateIndex = cmd.vertexBufferStateIndex;
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
        const grfx::dx11::CommandBuffer::GraphicsPipelineState& state = pCmdBuf->mGraphicsPipelineStack.At(mGraphicsPipelineStateIndex);

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

                    if (args.dsv.loadOp == grfx::ATTACHMENT_LOAD_OP_CLEAR) {
                        PPX_ASSERT_MSG(false, "not implemented");
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

                    //case CMD_SET_VIEWPORTS: {
                    //    const dx11::CommandBuffer::SetViewportsArgs& args = cmd.args.setViewportArgs;
                    //    mDeviceContext->RSSetViewports(args.NumViewports, args.pViewports);
                    //} break;
                    //
                    //case CMD_SET_SCISSORS: {
                    //    const dx11::CommandBuffer::SetScissorssArgs& args = cmd.args.setScissorssArgs;
                    //    mDeviceContext->RSSetScissorRects(args.NumRects, args.pRects);
                    //} break;

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
