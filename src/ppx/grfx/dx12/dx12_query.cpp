#include "ppx/grfx/dx12/dx12_query.h"
#include "ppx/grfx/dx12/dx12_device.h"

namespace ppx {
namespace grfx {
namespace dx {

Result QueryPool::CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo)
{
    D3D12_QUERY_HEAP_DESC desc = {};
    desc.Type                  = ToD3D12QueryHeapType(pCreateInfo->type);
    desc.Count                 = static_cast<UINT>(pCreateInfo->count);

    D3D12DevicePtr device = ToApi(GetDevice())->GetDxDevice();

    HRESULT hr = device->CreateQueryHeap(&desc, IID_PPV_ARGS(&mHeap));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateQueryHeap failed");
        return ppx::ERROR_API_FAILURE;
    }

    mQueryType = ToD3D12QueryType(pCreateInfo->type);

    return ppx::SUCCESS;
}

void QueryPool::DestroyApiObjects()
{
    if (mHeap) {
        mHeap.Reset();
    }
}

void QueryPool::Reset(uint32_t firstQuery, uint32_t queryCount)
{
    (void)firstQuery;
    (void)queryCount;
}

} // namespace dx
} // namespace grfx
} // namespace ppx
