#ifndef ppx_grfx_dx12_descriptor_helper_h
#define ppx_grfx_dx12_descriptor_helper_h

// *** Graphics API Note ***
//
// In D3D12 it isn't explicity required to allocate unique CPU handles for
// descriptors ahead of time. Applications are free to reuse heaps at will
// and recycle handle offsets. But because we are aligning  D3D12 to Vulkan,
// certain descriptor types, such as RTV and DSV, need an explicit handle
// due to VkImageView being a concrete and allocated object in Vulkan.
//

#include "ppx/grfx/dx12/dx12_config.h"

namespace ppx {
namespace grfx {
namespace dx12 {

class Device;

// -------------------------------------------------------------------------------------------------
// Descriptor Handle Manager Helpers
// -------------------------------------------------------------------------------------------------
const UINT   MAX_DESCRIPTOR_HANDLE_HEAP_SIZE = 256;
const SIZE_T INVALID_D3D12_DESCRIPTOR_HANDLE = static_cast<SIZE_T>(~0);

struct DescriptorHandle
{
    UINT                        offset = UINT_MAX;
    D3D12_CPU_DESCRIPTOR_HANDLE handle = {dx12::INVALID_D3D12_DESCRIPTOR_HANDLE};

    DescriptorHandle()
    {
        Reset();
    }

    operator bool() const
    {
        bool result = (offset != UINT_MAX) && (handle.ptr != 0);
        return result;
    }

    void Reset()
    {
        offset     = UINT_MAX;
        handle.ptr = dx12::INVALID_D3D12_DESCRIPTOR_HANDLE;
    }
};

//! @class DescriptorHandleAllocator
//!
//!
class DescriptorHandleAllocator
{
public:
    DescriptorHandleAllocator();
    ~DescriptorHandleAllocator();

    Result Create(dx12::Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type);
    void   Destroy();

    Result AllocateHandle(DescriptorHandle* ppHandle);
    void   FreeHandle(const DescriptorHandle& handle);
    bool   HasHandle(const DescriptorHandle& handle) const;
    bool   HasAvailableHandle() const;

private:
    UINT FirstAvailableIndex() const;

private:
    D3D12DescriptorHeapPtr        mHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE   mHeapStart     = {};
    UINT                          mIncrementSize = 0;
    std::vector<DescriptorHandle> mHandles;
};

//! @class DescriptorHandleManager
//!
//!
class DescriptorHandleManager
{
public:
    DescriptorHandleManager();
    ~DescriptorHandleManager();

    Result Create(dx12::Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type);
    void   Destroy();

    Result AllocateHandle(DescriptorHandle* pHandle);
    void   FreeHandle(const DescriptorHandle& handle);
    bool   HasHandle(const DescriptorHandle& handle) const;

private:
    dx12::Device*                           mDevice = nullptr;
    D3D12_DESCRIPTOR_HEAP_TYPE              mType   = InvalidValue<D3D12_DESCRIPTOR_HEAP_TYPE>();
    std::vector<DescriptorHandleAllocator*> mAllocators;
};

} // namespace dx12
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx12_descriptor_helper_h
