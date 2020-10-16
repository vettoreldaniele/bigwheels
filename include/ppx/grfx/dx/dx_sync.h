#ifndef ppx_grfx_dx_sync_h
#define ppx_grfx_dx_sync_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_sync.h"

namespace ppx {
namespace grfx {
namespace dx {

class Fence
    : public grfx::Fence
{
public:
    Fence() {}
    virtual ~Fence() {}

    typename D3D12FencePtr::InterfaceType* GetDxFence() const { return mFence.Get(); }

    UINT64 GetNextSignalValue();
    UINT64 GetWaitForValue() const;

    virtual Result Wait(uint64_t timeout = UINT64_MAX) override;
    virtual Result Reset() override;

protected:
    virtual Result CreateApiObjects(const grfx::FenceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12FencePtr mFence;
    HANDLE        mFenceEventHandle = nullptr;
    UINT64        mValue = 0;
};

// -------------------------------------------------------------------------------------------------

class Semaphore
    : public grfx::Semaphore
{
public:
    Semaphore() {}
    virtual ~Semaphore() {}

    typename D3D12FencePtr::InterfaceType* GetDxFence() const { return mFence.Get(); }

    UINT64 GetNextSignalValue();
    UINT64 GetWaitForValue() const;

protected:
    virtual Result CreateApiObjects(const grfx::SemaphoreCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12FencePtr mFence;
    UINT64        mValue = 0;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_sync_h
