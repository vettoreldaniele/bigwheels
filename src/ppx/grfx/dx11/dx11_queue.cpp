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

    PPX_ASSERT_MSG(QUERY_FRAME_DELAY >= 1, "invalid frequency query delay");
    for (uint32_t i = 0; i < MAX_QUERIES_IN_FLIGHT; ++i) {
        D3D11_QUERY_DESC queryDesc = {};
        queryDesc.Query            = D3D11_QUERY_TIMESTAMP_DISJOINT;
        HRESULT hr                 = ToApi(GetDevice())->GetDxDevice()->CreateQuery(&queryDesc, &mFrequencyQuery[i]);
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D11Device::CreateQuery failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    mReadFrequencyQuery  = 0;
    mWriteFrequencyQuery = 0;

    return ppx::SUCCESS;
}

void Queue::DestroyApiObjects()
{
}

Result Queue::WaitIdle()
{
    return ppx::SUCCESS;
}

Result Queue::UpdateTimestampFrequency()
{
    ID3D11DeviceContext* ctx = mDeviceContext.Get();
    if (mWriteFrequencyQuery <= QUERY_FRAME_DELAY) {
        mFrequency = 0;
        return ppx::SUCCESS;
    }
    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT queryData = {};

    HRESULT queryResult = S_FALSE;
    while (queryResult == S_FALSE) {
        queryResult = GetDxDeviceContext()->GetData(mFrequencyQuery[mReadFrequencyQuery % MAX_QUERIES_IN_FLIGHT], &queryData, sizeof(queryData), 0);

        if (queryResult != S_OK) {
            return ppx::ERROR_FAILED;
        }
    }

    mFrequency = queryData.Frequency;
    return ppx::SUCCESS;
}

Result Queue::Submit(const grfx::SubmitInfo* pSubmitInfo)
{
    ID3D11DeviceContext* ctx = mDeviceContext.Get();
    if (mWriteFrequencyQuery > QUERY_FRAME_DELAY) {
        mReadFrequencyQuery++;
    }

    ctx->Begin(mFrequencyQuery[mWriteFrequencyQuery % MAX_QUERIES_IN_FLIGHT]);

    for (uint32_t cmdBufIndex = 0; cmdBufIndex < pSubmitInfo->commandBufferCount; ++cmdBufIndex) {
        const dx11::CommandBuffer* pCmdBuf = ToApi(pSubmitInfo->ppCommandBuffers[cmdBufIndex]);
        const dx11::CommandList&   cmdList = pCmdBuf->GetCommandList();
        cmdList.Execute(mDeviceContext.Get());
    }

    ctx->End(mFrequencyQuery[mWriteFrequencyQuery % MAX_QUERIES_IN_FLIGHT]);
    mWriteFrequencyQuery++;

    UpdateTimestampFrequency();

    return ppx::SUCCESS;
}

Result Queue::GetTimestampFrequency(uint64_t* pFrequency) const
{
    *pFrequency = mFrequency;
    return ppx::SUCCESS;
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
