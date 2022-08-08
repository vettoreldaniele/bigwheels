#include "ppx/grfx/grfx_sync.h"

namespace ppx {
namespace grfx {

Result Fence::WaitAndReset(uint64_t timeout)
{
    Result ppxres = Wait(timeout);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = Reset();
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
