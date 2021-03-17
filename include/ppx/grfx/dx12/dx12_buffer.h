#ifndef ppx_grfx_dx12_buffer_h
#define ppx_grfx_dx12_buffer_h

#include "ppx/grfx/dx12/000_dx12_config.h"
#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {
namespace dx12 {

class Buffer
    : public grfx::Buffer
{
public:
    Buffer() {}
    virtual ~Buffer() {}

    typename D3D12ResourcePtr::InterfaceType* GetDxResource() const { return mResource.Get(); }

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

} // namespace dx12
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx12_buffer_h