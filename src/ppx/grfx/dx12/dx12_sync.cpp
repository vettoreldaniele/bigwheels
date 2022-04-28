#include "ppx/grfx/dx12/dx12_sync.h"
#include "ppx/grfx/dx12/dx12_device.h"

namespace ppx {
namespace grfx {
namespace dx12 {

// -------------------------------------------------------------------------------------------------
// Fence
// -------------------------------------------------------------------------------------------------
Result Fence::CreateApiObjects(const grfx::FenceCreateInfo* pCreateInfo)
{
    D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE;

    HRESULT hr = ToApi(GetDevice())->GetDxDevice()->CreateFence(mValue, flags, IID_PPV_ARGS(&mFence));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateFence(fence) failed");
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(D3D12Fence(Fence), mFence.Get());

    mFenceEventHandle = CreateEventEx(NULL, NULL, false, EVENT_ALL_ACCESS);
    if (mFenceEventHandle == INVALID_HANDLE_VALUE) {
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void Fence::DestroyApiObjects()
{
#if defined(PPX_DXIIVK)
#else
    CloseHandle(mFenceEventHandle);
#endif

    if (mFence) {
        mFence.Reset();
    }
}

UINT64 Fence::GetNextSignalValue()
{
    mValue += 1;
    return mValue;
}

UINT64 Fence::GetWaitForValue() const
{
    return mValue;
}

Result Fence::Wait(uint64_t timeout)
{
    UINT64 completedValue = mFence->GetCompletedValue();
    if (completedValue < GetWaitForValue()) {
        mFence->SetEventOnCompletion(mValue, mFenceEventHandle);

        DWORD dwMillis = (timeout == UINT64_MAX) ? INFINITE : static_cast<DWORD>(timeout / 1000000ULL);
        WaitForSingleObjectEx(mFenceEventHandle, dwMillis, false);
    }
    return ppx::SUCCESS;
}

Result Fence::Reset()
{
    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------
// Semaphore
// -------------------------------------------------------------------------------------------------
Result Semaphore::CreateApiObjects(const grfx::SemaphoreCreateInfo* pCreateInfo)
{
    D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE;

    HRESULT hr = ToApi(GetDevice())->GetDxDevice()->CreateFence(mValue, flags, IID_PPV_ARGS(&mFence));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateFence(fence) failed");
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(D3D12Fence(Semaphore), mFence.Get());

    return ppx::SUCCESS;
}

void Semaphore::DestroyApiObjects()
{
    if (mFence) {
        mFence.Reset();
    }
}

UINT64 Semaphore::GetNextSignalValue()
{
    mValue += 1;
    return mValue;
}

UINT64 Semaphore::GetWaitForValue() const
{
    return mValue;
}

} // namespace dx12
} // namespace grfx
} // namespace ppx
