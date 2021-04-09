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

Result Queue::Submit(const grfx::SubmitInfo* pSubmitInfo)
{
    for (uint32_t cmdBufIndex = 0; cmdBufIndex < pSubmitInfo->commandBufferCount; ++cmdBufIndex) {
        const dx11::CommandBuffer* pCmdBuf = ToApi(pSubmitInfo->ppCommandBuffers[cmdBufIndex]);
        const dx11::CommandList& cmdList = pCmdBuf->GetCommandList();
        cmdList.Execute(mDeviceContext.Get());
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
