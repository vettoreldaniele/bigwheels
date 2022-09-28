#ifndef ppx_grfx_sync_h
#define ppx_grfx_sync_h

#include "ppx/grfx/grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct FenceCreateInfo
//!
//!
struct FenceCreateInfo
{
    bool signaled = false;
};

//! @class Fence
//!
//!
class Fence
    : public grfx::DeviceObject<grfx::FenceCreateInfo>
{
public:
    Fence() {}
    virtual ~Fence() {}

    virtual Result Wait(uint64_t timeout = UINT64_MAX) = 0;
    virtual Result Reset()                             = 0;

    Result WaitAndReset(uint64_t timeout = UINT64_MAX);

protected:
    virtual Result CreateApiObjects(const grfx::FenceCreateInfo* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                                        = 0;
    friend class grfx::Device;
};

// -------------------------------------------------------------------------------------------------

//! @struct SemaphoreCreateInfo
//!
//!
struct SemaphoreCreateInfo
{
};

//! @class Semaphore
//!
//!
class Semaphore
    : public grfx::DeviceObject<grfx::SemaphoreCreateInfo>
{
public:
    Semaphore() {}
    virtual ~Semaphore() {}

protected:
    virtual Result CreateApiObjects(const grfx::SemaphoreCreateInfo* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                                            = 0;
    friend class grfx::Device;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_sync_h
