#ifndef ppx_grfx_dx_buffer_h
#define ppx_grfx_dx_buffer_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {
namespace dx {

class Buffer
    : public grfx::Buffer
{
public:
    Buffer() {}
    virtual ~Buffer() {}

    D3D12ResourcePtr GetDxResource() const { return mResource; }

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) override;
    virtual void   UnmapMemory() override;

protected:
    virtual Result CreateApiObjects(const grfx::BufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12ResourcePtr            mResource;
    D3D12_HEAP_TYPE             mHeapType;
    ObjPtr<D3D12MA::Allocation> mAllocation;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_buffer_h
