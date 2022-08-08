#include "ppx/grfx/dx11/dx11_sync.h"

namespace ppx {
namespace grfx {
namespace dx11 {

// -------------------------------------------------------------------------------------------------
// Fence
// -------------------------------------------------------------------------------------------------
Result Fence::CreateApiObjects(const grfx::FenceCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void Fence::DestroyApiObjects()
{
}

Result Fence::Wait(uint64_t timeout)
{
    return ppx::SUCCESS;
}

Result Fence::Reset()
{
    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------
// Semaphore
// -------------------------------------------------------------------------------------------------
Result Semaphore::CreateApiObjects(const grfx::SemaphoreCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void Semaphore::DestroyApiObjects()
{
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
