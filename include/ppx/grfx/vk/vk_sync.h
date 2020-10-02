#ifndef ppx_grfx_vk_sync_h
#define ppx_grfx_vk_sync_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_sync.h"

namespace ppx {
namespace grfx {
namespace vk {

//! @class Fence
//!
//!
class Fence
    : public grfx::Fence
{
public:
    Fence() {}
    virtual ~Fence() {}

    VkFencePtr GetVkFence() const { return mFence; }

    virtual Result Wait(uint64_t timeout = UINT64_MAX) override;
    virtual Result Reset() override;

protected:
    virtual Result CreateApiObjects(const grfx::FenceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkFencePtr mFence;
};

// -------------------------------------------------------------------------------------------------

//! @class Semaphore
//!
//!
class Semaphore
    : public grfx::Semaphore
{
public:
    Semaphore() {}
    virtual ~Semaphore() {}

    VkSemaphorePtr GetVkSemaphore() const { return mSemaphore; }

protected:
    virtual Result CreateApiObjects(const grfx::SemaphoreCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkSemaphorePtr mSemaphore;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_sync_h
