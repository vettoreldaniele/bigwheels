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
    uint32_t               size       = 0;
    grfx::BufferUsageFlags usageFlags = 0;
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
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_buffer_h
