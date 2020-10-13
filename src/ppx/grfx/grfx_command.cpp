#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {

void CommandBuffer::BindIndexBuffer(const grfx::Buffer* pBuffer, grfx::IndexType indexType, uint64_t offset)
{
    grfx::IndexBufferView view = {};
    view.pBuffer               = pBuffer;
    view.indexType             = indexType;
    view.offset                = offset;

    BindIndexBuffer(&view);
}

void CommandBuffer::BindVertexBuffers(
    uint32_t                   bufferCount,
    const grfx::Buffer* const* ppBuffers,
    const uint32_t*            pStrides,
    const uint64_t*            pOffsets)
{
    PPX_ASSERT_MSG(bufferCount < PPX_MAX_VERTEX_BINDINGS, "bufferCount exceeds PPX_MAX_VERTEX_ATTRIBUTES");

    grfx::VertexBufferView views[PPX_MAX_VERTEX_BINDINGS] = {};
    for (uint32_t i = 0; i < bufferCount; ++i) {
        views[i].pBuffer    = ppBuffers[i];
        views[i].stride = pStrides[i];
        if (!IsNull(pOffsets)) {
            views[i].offset = pOffsets[i];
        }
    }

    BindVertexBuffers(bufferCount, views);
}

} // namespace grfx
} // namespace ppx
