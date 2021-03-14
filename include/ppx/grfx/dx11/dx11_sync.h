#ifndef ppx_grfx_dx11_sync_h
#define ppx_grfx_dx11_sync_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_sync.h"

namespace ppx {
namespace grfx {
namespace dx11{

class Fence
    : public grfx::Fence
{
public:
    Fence() {}
    virtual ~Fence() {}

    virtual Result Wait(uint64_t timeout = UINT64_MAX) override;
    virtual Result Reset() override;

protected:
    virtual Result CreateApiObjects(const grfx::FenceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
};

// -------------------------------------------------------------------------------------------------

class Semaphore
    : public grfx::Semaphore
{
public:
    Semaphore() {}
    virtual ~Semaphore() {}

    UINT64 GetNextSignalValue();
    UINT64 GetWaitForValue() const;

protected:
    virtual Result CreateApiObjects(const grfx::SemaphoreCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_sync_h
