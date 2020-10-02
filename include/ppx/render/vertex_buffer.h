#ifndef ppx_render_vertex_buffer_h
#define ppx_render_vertex_buffer_h

#include "ppx/render/base_buffer.h"

namespace ppx {
namespace render {

struct VertexBufferCreateInfo
{
    uint64_t size = 0;
};

class VertexBuffer
    : public BaseBuffer
{
public:
};

} // namespace render
} // namespace ppx

#endif // ppx_render_vertex_buffer_h
