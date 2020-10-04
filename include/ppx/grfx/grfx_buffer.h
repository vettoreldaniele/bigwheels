#ifndef ppx_grfx_buffer_h
#define ppx_grfx_buffer_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct BufferCreateInfo
//!
//!
struct BufferCreateInfo
{
    uint32_t               size        = 0;
    grfx::BufferUsageFlags usageFlags  = 0;
    grfx::MemoryUsage      memoryUsage = grfx::MEMORY_USAGE_GPU_ONLY;
};

//! @class Buffer
//!
//!
class Buffer
    : public grfx::DeviceObject<grfx::BufferCreateInfo>
{
public:
    Buffer() {}
    virtual ~Buffer() {}

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) = 0;
    virtual void   UnmapMemory()                                      = 0;
};

// -------------------------------------------------------------------------------------------------

struct IndexBufferView
{
    const grfx::Buffer* pBuffer   = nullptr;
    grfx::IndexType     indexType = grfx::INDEX_TYPE_UINT16;
    uint64_t            offset    = 0;

    IndexBufferView() {}

    IndexBufferView(const grfx::Buffer* pBuffer_, grfx::IndexType indexType_, uint64_t offset_ = 0)
        : pBuffer(pBuffer_), indexType(indexType_), offset(offset_) {}
};

// -------------------------------------------------------------------------------------------------

struct VertexBufferView
{
    const grfx::Buffer* pBuffer = nullptr;
    uint64_t            offset  = 0;

    VertexBufferView() {}

    VertexBufferView(const grfx::Buffer* pBuffer_, uint64_t offset_ = 0)
        : pBuffer(pBuffer_), offset(offset_) {}
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_buffer_h
