#ifndef ppx_grfx_dx_descriptor_h
#define ppx_grfx_dx_descriptor_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_descriptor.h"

// *** Graphics API Note ***
//
// To keep things simple, aliasing of descriptor binding ranges within
// descriptor set layouts are curently not permitted.
//
// D3D12 limits sampler heap size to 2048
//   See: https://docs.microsoft.com/en-us/windows/win32/direct3d12/hardware-support?redirectedfrom=MSDN
//
// Shader visible vs non-shader visible heaps:
//    See: https://docs.microsoft.com/en-us/windows/win32/direct3d12/non-shader-visible-descriptor-heaps
//
//

namespace ppx {
namespace grfx {
namespace dx {

// -------------------------------------------------------------------------------------------------

class DescriptorPool
    : public grfx::DescriptorPool
{
public:
    struct Allocation
    {
        const grfx::DescriptorSet* pSet    = nullptr;
        uint32_t                   binding = UINT32_MAX;
        uint32_t                   offset  = 0;
        uint32_t                   count   = 0;
    };

    DescriptorPool() {}
    virtual ~DescriptorPool() {}

    Result AllocateDescriptorSet(const dx::DescriptorSetLayout* pLayout, dx::DescriptorSet* pSet);
    void   FreeDescriptorSet(const dx::DescriptorSet* pSet);

protected:
    virtual Result CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    uint32_t                mHeapSizeCBVSRVUAV = 0;
    uint32_t                mHeapSizeSampler   = 0;
    D3D12DescriptorHeapPtr  mHeapCBVSRVUAV;
    D3D12DescriptorHeapPtr  mHeapSampler;
    uint32_t                mNumAvailableCBVSRVUAV = 0;
    uint32_t                mNumAvailableSampler   = 0;
    std::vector<Allocation> mAllocationsCBVSRVUAV;
    std::vector<Allocation> mAllocationsSampler;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSet
    : public grfx::DescriptorSet
{
public:
    struct HeapOffset
    {
        UINT                        binding          = UINT32_MAX;
        UINT                        offset           = UINT32_MAX;
        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = {};
    };

    DescriptorSet() {}
    virtual ~DescriptorSet() {}

    std::vector<HeapOffset>& GetHeapOffsets() { return mHeapOffsets; }

    D3D12DescriptorHeapPtr  GetHeapCBVSRVUAV() const { return mHeapCBVSRVUAV; }
    D3D12DescriptorHeapPtr  GetHeapSampler() const { return mHeapSampler; }

    virtual Result UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites) override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    UINT                    mHeapSizeCBVSRVUAV = 0;
    UINT                    mHeapSizeSampler   = 0;
    D3D12DescriptorHeapPtr  mHeapCBVSRVUAV;
    D3D12DescriptorHeapPtr  mHeapSampler;
    std::vector<HeapOffset> mHeapOffsets;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSetLayout
    : public grfx::DescriptorSetLayout
{
public:
    struct DescriptorRange
    {
        uint32_t binding = UINT32_MAX;
        uint32_t count   = 0;
    };

    DescriptorSetLayout() {}
    virtual ~DescriptorSetLayout() {}

    uint32_t GetCountCBVSRVUAV() const { return mCountCBVSRVUAV; }
    uint32_t GetCountSampler() const { return mCountSampler; }

    const std::vector<DescriptorRange>& GetRangesCBVSRVUAV() const { return mRangesCBVSRVUAV; }
    const std::vector<DescriptorRange>& GetRangesSampler() const { return mRangesSampler; }

protected:
    virtual Result CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    uint32_t                     mCountCBVSRVUAV = 0;
    uint32_t                     mCountSampler   = 0;
    std::vector<DescriptorRange> mRangesCBVSRVUAV;
    std::vector<DescriptorRange> mRangesSampler;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_descriptor_h
