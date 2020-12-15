#ifndef ppx_grfx_dx_queue_h
#define ppx_grfx_dx_queue_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_queue.h"

namespace ppx {
namespace grfx {
namespace dx {

class Queue
    : public grfx::Queue
{
public:
    Queue() {}
    virtual ~Queue() {}

    typename D3D12CommandQueuePtr::InterfaceType* GetDxQueue() const { return mCommandQueue.Get(); }

    virtual Result WaitIdle() override;

    virtual Result Submit(const grfx::SubmitInfo* pSubmitInfo) override;

    virtual Result GetTimestampFrequency(uint64_t* pFrequency) const override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::QueueCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12CommandQueuePtr            mCommandQueue;
    grfx::FencePtr                  mWaitIdleFence;
    std::vector<ID3D12CommandList*> mListBuffer;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_queue_h
