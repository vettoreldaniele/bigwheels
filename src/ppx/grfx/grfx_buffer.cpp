#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {

Result Buffer::Create(const grfx::BufferCreateInfo* pCreateInfo)
{
#if 0
    // Constant/uniform buffers need to be at least PPX_CONSTANT_BUFFER_ALIGNMENT in size
    if (pCreateInfo->usageFlags.bits.uniformBuffer && (pCreateInfo->size < PPX_CONSTANT_BUFFER_ALIGNMENT)) {
        PPX_ASSERT_MSG(false, "constant/uniform buffer sizes must be at least PPX_CONSTANT_BUFFER_ALIGNMENT (" << PPX_CONSTANT_BUFFER_ALIGNMENT << ")");
        return ppx::ERROR_GRFX_MINIMUM_BUFFER_SIZE_NOT_MET;
    }

    // Storage/structured buffers need to be at least PPX_STORAGE_BUFFER_ALIGNMENT in size
    if (pCreateInfo->usageFlags.bits.uniformBuffer && (pCreateInfo->size < PPX_STUCTURED_BUFFER_ALIGNMENT)) {
        PPX_ASSERT_MSG(false, "storage/structured buffer sizes must be at least PPX_STUCTURED_BUFFER_ALIGNMENT (" << PPX_STUCTURED_BUFFER_ALIGNMENT << ")");
        return ppx::ERROR_GRFX_MINIMUM_BUFFER_SIZE_NOT_MET;
    }
#endif
    Result ppxres = grfx::DeviceObject<grfx::BufferCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

Result Buffer::CopyFromSource(uint32_t dataSize, const void* pData)
{
    if (dataSize > GetSize()) {
        return ppx::ERROR_LIMIT_EXCEEDED;
    }

    // Map
    void*  pBufferAddress = nullptr;
    Result ppxres         = MapMemory(0, &pBufferAddress);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Copy
    std::memcpy(pBufferAddress,pData, dataSize);

    // Unmap
    UnmapMemory();

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
