#ifndef ppx_render_base_buffer_h
#define ppx_render_base_buffer_h

#include "ppx/000_config.h"

namespace ppx {
namespace render {

class BaseBuffer
{
public:
    BaseBuffer() {}
    virtual ~BaseBuffer() {}

protected:
    ObjPtr<Buffer> mBuffer;
};

} // namespace render
} // namespace ppx

#endif // ppx_render_base_buffer_h
