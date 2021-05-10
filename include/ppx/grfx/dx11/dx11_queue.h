#ifndef ppx_grfx_dx11_queue_h
#define ppx_grfx_dx11_queue_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/dx11/dx11_command.h"
#include "ppx/grfx/grfx_queue.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class Queue
    : public grfx::Queue
{
public:
    Queue() {}
    virtual ~Queue() {}

    typename D3D11DeviceContextPtr::InterfaceType* GetDxDeviceContext() const { return mDeviceContext.Get(); }

    virtual Result WaitIdle() override;

    virtual Result Submit(const grfx::SubmitInfo* pSubmitInfo) override;

    virtual Result GetTimestampFrequency(uint64_t* pFrequency) const override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::QueueCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D11DeviceContextPtr mDeviceContext;

    static const int QUERY_FRAME_DELAY = 3, MAX_QUERIES_IN_FLIGHT = QUERY_FRAME_DELAY + 1;
    ID3D11Query*     mFrequencyQuery[MAX_QUERIES_IN_FLIGHT];
    uint32_t         mReadFrequencyQuery, mWriteFrequencyQuery;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx11_queue_h
