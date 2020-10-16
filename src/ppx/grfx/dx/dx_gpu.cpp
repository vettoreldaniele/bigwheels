#include "ppx/grfx/dx/dx_gpu.h"

#define PPX_MAX_DX12_GRAPHICS_QUEUES 1
#define PPX_MAX_DX12_COMPUTE_QUEUES  2
#define PPX_MAX_DX12_COPY_QUEUES     2

namespace ppx {
namespace grfx {
namespace dx {

Result Gpu::CreateApiObjects(const grfx::internal::GpuCreateInfo* pCreateInfo)
{
    ComPtr<IDXGIAdapter> adapter = static_cast<IDXGIAdapter*>(pCreateInfo->pApiObject);
    HRESULT              hr      = adapter.As(&mGpu);
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }

    DXGI_ADAPTER_DESC3 adapterDesc = {};
    hr                             = mGpu->GetDesc3(&adapterDesc);
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }

    // Name - hack convert name from wchar to char
    for (size_t i = 0;; ++i) {
        if (adapterDesc.Description[i] == 0) {
            break;
        }
        char c = static_cast<char>(adapterDesc.Description[i]);
        mDeviceName.push_back(c);
    }

    // Vendor
    mDeviceVendorId = static_cast<grfx::VendorId>(adapterDesc.VendorId);

    return ppx::SUCCESS;
}

void Gpu::DestroyApiObjects()
{
    if (mGpu) {
        mGpu.Reset();
    }
}

uint32_t Gpu::GetGraphicsQueueCount() const
{
    return PPX_MAX_DX12_GRAPHICS_QUEUES;
}

uint32_t Gpu::GetComputeQueueCount() const
{
    return PPX_MAX_DX12_COMPUTE_QUEUES;
}

uint32_t Gpu::GetTransferQueueCount() const
{
    return PPX_MAX_DX12_COPY_QUEUES;
}

} // namespace dx
} // namespace grfx
} // namespace ppx