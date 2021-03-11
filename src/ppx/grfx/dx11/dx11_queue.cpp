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

Result Queue::Submit(const grfx::SubmitInfo* pSubmitInfo)
{
    for (uint32_t cmdBufIndex = 0; cmdBufIndex < pSubmitInfo->commandBufferCount; ++cmdBufIndex) {
        const dx11::CommandBuffer* pCmd = ToApi(pSubmitInfo->ppCommandBuffers[cmdBufIndex]);

        const std::vector<dx11::CommandBuffer::ActionCmd>& actionCmds = pCmd->mActionCmds;

        const size_t numCmds = actionCmds.size();
        for (size_t cmdIndex = 0; cmdIndex < numCmds; ++cmdIndex) {
            const dx11::CommandBuffer::ActionCmd& cmd = actionCmds[cmdIndex];

            switch (cmd.cmd) {
                default: {
                    PPX_ASSERT_MSG(false, "unknown command");
                } break;

                case CMD_BEGIN_RENDER_PASS: {
                    const dx11::CommandBuffer::BeginRenderPassArgs& args = cmd.args.beginRenderPass;

                    mDeviceContext->OMSetRenderTargets(
                        args.NumViews,
                        args.ppRenderTargetViews,
                        args.pDepthStencilView);

                    for (UINT i = 0; i < args.NumViews; ++i) {
                        if (!IsNull(args.RTVClearOp[i].pRenderTargetView)) {
                            mDeviceContext->ClearRenderTargetView(
                                args.RTVClearOp[i].pRenderTargetView,
                                args.RTVClearOp[i].ColorRGBA);
                        }
                    }
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
