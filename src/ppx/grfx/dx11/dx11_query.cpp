#include "ppx/grfx/dx11/dx11_query.h"
#include "ppx/grfx/dx11/dx11_device.h"

namespace ppx {
namespace grfx {
namespace dx11 {

Result QueryPool::CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo)
{
    mQueryType = ToD3D11QueryType(pCreateInfo->type);
    mQueryPool.resize(pCreateInfo->count);

    D3D11DevicePtr device = ToApi(GetDevice())->GetDxDevice();

    for (UINT i = 0; i < pCreateInfo->count; ++i) {
        D3D11_QUERY_DESC queryDesc = {};
        queryDesc.Query            = mQueryType;
        HRESULT hr                 = device->CreateQuery(&queryDesc, &mQueryPool[i]);
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D11Device::CreateQuery failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    return ppx::SUCCESS;
}

ID3D11Query* QueryPool::GetQuery(uint32_t queryIndex) const
{
    PPX_ASSERT_MSG(queryIndex < mQueryPool.size(), "invalid query index");

    return mQueryPool[queryIndex];
}

void QueryPool::DestroyApiObjects()
{
}

void QueryPool::Reset(uint32_t firstQuery, uint32_t queryCount)
{
    (void)firstQuery;
    (void)queryCount;
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
