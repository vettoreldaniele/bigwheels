#ifndef ppx_grfx_vk_buffer_h
#define ppx_grfx_vk_buffer_h

#include "ppx/grfx/vk/vk_config.h"
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

    VkBufferPtr GetVkBuffer() const { return mBuffer; }

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) override;
    virtual void   UnmapMemory() override;

protected:
    virtual Result CreateApiObjects(const grfx::BufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkBufferPtr       mBuffer;
    VmaAllocationPtr  mAllocation;
    VmaAllocationInfo mAllocationInfo = {};
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_buffer_h
