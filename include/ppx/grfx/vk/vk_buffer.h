#ifndef ppx_grfx_vk_buffer_h
#define ppx_grfx_vk_buffer_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {
namespace vk {

class Buffer
    : public grfx::Buffer
{
public:
    Buffer() {}
    virtual ~Buffer() {}

protected:
    virtual Result CreateApiObjects(const grfx::BufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkBufferPtr mBuffer;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_buffer_h
