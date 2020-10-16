#include "ppx/grfx/dx/dx_queue.h"
#include "ppx/grfx/dx/dx_command.h"
#include "ppx/grfx/dx/dx_device.h"
#include "ppx/grfx/dx/dx_sync.h"

namespace ppx {
namespace grfx {
namespace dx {

Result Queue::CreateApiObjects(const grfx::internal::QueueCreateInfo* pCreateInfo)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo->pApiObject);
    if (IsNull(pCreateInfo->pApiObject)) {
        return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
    }

    ComPtr<ID3D12CommandQueue> queue = static_cast<ID3D12CommandQueue*>(pCreateInfo->pApiObject);

    HRESULT hr = queue.As(&mCommandQueue);
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "failed casting to ID3DCommandQueue");
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(D3D12CommandQueue, mCommandQueue.Get());

    grfx::FenceCreateInfo fenceCreateInfo = {};
    Result                ppxres          = GetDevice()->CreateFence(&fenceCreateInfo, &mWaitIdleFence);
    if (Failed(ppxres)) {
        PPX_ASSERT_MSG(false, "wait for idle fence create failed");
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Queue::DestroyApiObjects()
{
    WaitIdle();

    if (mWaitIdleFence) {
        mWaitIdleFence.Reset();
    }

    if (mCommandQueue) {
        mCommandQueue.Reset();
    }
}

Result Queue::WaitIdle()
{
    dx::Fence* pFence = ToApi(mWaitIdleFence.Get());
    UINT64     value  = pFence->GetNextSignalValue();
    mCommandQueue->Signal(pFence->GetDxFence(), value);
    Result ppxres = pFence->Wait();
    if (Failed(ppxres)) {
        return ppxres;
    }
    return ppx::SUCCESS;
}

Result Queue::Submit(const grfx::SubmitInfo* pSubmitInfo)
{
    if (mListBuffer.size() < pSubmitInfo->commandBufferCount) {
        mListBuffer.resize(pSubmitInfo->commandBufferCount);
    }

    for (uint32_t i = 0; i < pSubmitInfo->commandBufferCount; ++i) {
        mListBuffer[i] = ToApi(pSubmitInfo->ppCommandBuffers[i])->GetDxCommandList();
    }

    for (uint32_t i = 0; i < pSubmitInfo->waitSemaphoreCount; ++i) {
        ID3D12Fence* pDxFence = ToApi(pSubmitInfo->ppWaitSemaphores[i])->GetDxFence();
        UINT64       value    = ToApi(pSubmitInfo->ppWaitSemaphores[i])->GetWaitForValue();
        HRESULT      hr       = mCommandQueue->Wait(pDxFence, value);
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12CommandQueue::Wait failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    mCommandQueue->ExecuteCommandLists(
        static_cast<UINT>(pSubmitInfo->commandBufferCount),
        mListBuffer.data());

    for (uint32_t i = 0; i < pSubmitInfo->signalSemaphoreCount; ++i) {
        ID3D12Fence* pDxFence = ToApi(pSubmitInfo->ppSignalSemaphores[i])->GetDxFence();
        UINT64       value    = ToApi(pSubmitInfo->ppSignalSemaphores[i])->GetNextSignalValue();
        HRESULT      hr       = mCommandQueue->Signal(pDxFence, value);
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12CommandQueue::Signal failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    return ppx::SUCCESS;
}

} // namespace dx
} // namespace grfx
} // namespace ppx
