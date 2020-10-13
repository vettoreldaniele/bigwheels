#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {

Result Buffer::Create(const grfx::BufferCreateInfo* pCreateInfo)
{
    // Constant/uniform buffers need to be at least PPX_CONSTANT_BUFFER_ALIGNMENT in size
    if (pCreateInfo->usageFlags.bits.uniformBuffer && (pCreateInfo->size < PPX_CONSTANT_BUFFER_ALIGNMENT)) {
        PPX_ASSERT_MSG(false, "constant/uniform buffers must be at least PPX_CONSTANT_BUFFER_ALIGNMENT (" << PPX_CONSTANT_BUFFER_ALIGNMENT << ") in size");
        return ppx::ERROR_GRFX_MINIMUM_BUFFER_SIZE_NOT_MET;
    }

    // Storage/structured buffers need to be at least PPX_STORAGE_BUFFER_ALIGNMENT in size
    if (pCreateInfo->usageFlags.bits.uniformBuffer && (pCreateInfo->size < PPX_STUCTURED_BUFFER_ALIGNMENT)) {
        PPX_ASSERT_MSG(false, "constant/uniform buffers must be at leastPPX_STUCTURED_BUFFER_ALIGNMENT (" << PPX_STUCTURED_BUFFER_ALIGNMENT << ") in size");
        return ppx::ERROR_GRFX_MINIMUM_BUFFER_SIZE_NOT_MET;
    }

    Result ppxres = grfx::DeviceObject<grfx::BufferCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
