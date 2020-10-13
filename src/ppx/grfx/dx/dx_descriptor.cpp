#include "ppx/grfx/dx/dx_descriptor.h"
#include "ppx/grfx/dx/dx_device.h"
#include "ppx/grfx/dx/dx_buffer.h"

// *** Graphics API Note ***
//
// D3D12 doen't have the following descriptor types:
//   DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
//   DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
//   DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
//   DESCRIPTOR_TYPE_INPUT_ATTACHMENT
//

namespace ppx {
namespace grfx {
namespace dx {

const uint32_t END_OF_HEAP_BINDING_ID = UINT32_MAX;

// -------------------------------------------------------------------------------------------------
// DescriptorPool
// -------------------------------------------------------------------------------------------------
Result DescriptorPool::CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo)
{
    bool hasCombinedImageSampler = (pCreateInfo->combinedImageSampler > 0);
    bool hasUniformBufferDynamic = (pCreateInfo->uniformBufferDynamic > 0);
    bool hasStorageBufferDynamic = (pCreateInfo->storageBufferDynamic > 0);
    bool hasInputAtachment       = (pCreateInfo->inputAttachment > 0);
    bool hasUnsupported          = hasCombinedImageSampler || hasUniformBufferDynamic || hasStorageBufferDynamic || hasInputAtachment;
    if (hasUnsupported) {
        return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
    }

    // Get counts
    mHeapSizeCBVSRVUAV = pCreateInfo->sampledImage +
                         pCreateInfo->storageImage +
                         pCreateInfo->uniformTexelBuffer +
                         pCreateInfo->storageTexelBuffer +
                         pCreateInfo->uniformBuffer +
                         pCreateInfo->storageBuffer;
    mHeapSizeSampler = pCreateInfo->sampler;

    // Get D3D12 device
    D3D12DevicePtr device = ToApi(GetDevice())->GetDxDevice();

    // Allocate CBVSRVUAV heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors             = static_cast<UINT>(mHeapSizeCBVSRVUAV);
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask                   = 0;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapCBVSRVUAV));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(CPU CBVSRVUAV) failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Allocate Sampler heap
    {
        // CPU
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors             = static_cast<UINT>(mHeapSizeCBVSRVUAV);
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask                   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapSampler));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(CPU Sampler) failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    mNumAvailableCBVSRVUAV = mHeapSizeCBVSRVUAV;
    mNumAvailableSampler   = mHeapSizeSampler;

    // Add end of heap markers in the allocation trackers
    mAllocationsCBVSRVUAV.push_back(DescriptorPool::Allocation{nullptr, END_OF_HEAP_BINDING_ID, mHeapSizeCBVSRVUAV, 0});
    mAllocationsSampler.push_back(DescriptorPool::Allocation{nullptr, END_OF_HEAP_BINDING_ID, mHeapSizeSampler, 0});

    return ppx::SUCCESS;
}

void DescriptorPool::DestroyApiObjects()
{
    mHeapSizeCBVSRVUAV     = 0;
    mHeapSizeSampler       = 0;
    mNumAvailableCBVSRVUAV = 0;
    mNumAvailableSampler   = 0;

    if (mHeapCBVSRVUAV) {
        mHeapCBVSRVUAV.Reset();
    }
    if (mHeapSampler) {
        mHeapSampler.Reset();
    }
}

// Expects that 'occupants' is sorted by offsets
//
static Result FindFirstAvailableHeapOffset(
    const uint32_t                                 rangeCount,
    const std::vector<DescriptorPool::Allocation>& occupants,
    uint32_t&                                      heapOffset)
{
    size_t allocationCount = occupants.size();
    if (allocationCount == 0) {
        // This should never happen since there is always
        // at least one occupant - the end of the heap.
        //
        PPX_ASSERT_MSG(false, "unexpected empty occupants");
        return ppx::ERROR_FAILED;
    }

    if (allocationCount > 1) {
        for (size_t j = 1; j < allocationCount; ++j) {
            size_t      i       = j - 1;
            const auto& prev    = occupants[i];
            const auto& next    = occupants[j];
            uint32_t    prevEnd = prev.offset + prev.count;
            uint32_t    nextBeg = next.offset;
            uint32_t    diff    = nextBeg - prevEnd;
            if (diff >= rangeCount) {
                heapOffset = prevEnd;
                return ppx::SUCCESS;
            }
        }
    }
    else {
        heapOffset = 0;
        return ppx::SUCCESS;
    }
    return ppx::ERROR_ALLOCATION_FAILED;
}

static void SortByOffset(std::vector<dx::DescriptorPool::Allocation>& container)
{
    std::sort(
        std::begin(container),
        std::end(container),
        [](const DescriptorPool::Allocation& a, const DescriptorPool::Allocation& b) -> bool {
            bool isLess = a.offset < b.offset;
            return isLess; });
}

static Result AllocateRanges(
    dx::DescriptorSet*                                       pSet,
    const std::vector<DescriptorSetLayout::DescriptorRange>& ranges,
    const uint32_t                                           heapSize,
    const D3D12_CPU_DESCRIPTOR_HANDLE&                       heapStart,
    UINT                                                     handleIncrementSize,
    std::vector<DescriptorPool::Allocation>&                 allocations,
    std::vector<DescriptorSet::HeapOffset>&                  heapOffsets)
{
    // We'll use a list of occupants to determine if
    // there's enough space for a given range. We'll
    // start out by populating this list with the current
    // allocations. Then we'll grow it using the incoming
    // ranges.
    //
    // If all the ranges can fit into the available space,
    // then we're good.
    //
    // If any range fails to fit then we bail on the
    // entire thing.
    //
    std::vector<DescriptorPool::Allocation> occupants;
    for (auto& elem : allocations) {
        occupants.push_back(elem);
    }
    // Sort occupants
    SortByOffset(occupants);

    // Iterage the ranges and detrmine if all the ranges
    // can fit in he available space.
    //
    // NOTE: This is basic implementation and makes no
    //       attempt at best fit. It just looks for the
    //       first available offset that can fit the range.
    //
    for (auto& range : ranges) {
        // Find available
        uint32_t heapOffset = UINT32_MAX;
        Result   ppxres     = FindFirstAvailableHeapOffset(range.count, occupants, heapOffset);
        if (Failed(ppxres)) {
            return ppxres;
        }

        // Paranoid check
        if (heapOffset >= heapSize) {
            // This should never happen...
            PPX_ASSERT_MSG(false, "heap offset exceeds heap size");
            return ppx::ERROR_FAILED;
        }

        // Fill out occupant
        DescriptorPool::Allocation occupant = {};
        occupant.pSet                       = nullptr;
        occupant.binding                    = range.binding;
        occupant.offset                     = heapOffset;
        occupant.count                      = range.count;
        // Store occupant
        occupants.push_back(occupant);
        // Sort occupants
        SortByOffset(occupants);
    }

    // If we got here than all the ranges fit into the available space.
    //
    // Iterate through the occupants and anything with a null pSet means
    // it's new allocation for the target set.
    //
    for (const auto& occupant : occupants) {
        // Skip anything that already has a set assignment or is the end of heap entry
        if (!IsNull(occupant.pSet) || (occupant.binding == END_OF_HEAP_BINDING_ID)) {
            continue;
        }

        // Copy and store the allocation for the pool
        DescriptorPool::Allocation allocation = occupant;
        allocation.pSet                       = pSet;
        allocations.push_back(allocation);

        DescriptorSet::HeapOffset heapOffset = {};
        heapOffset.binding                   = occupant.binding;
        heapOffset.offset                    = occupant.offset;
        heapOffset.descriptorHandle.ptr      = heapStart.ptr + static_cast<SIZE_T>(handleIncrementSize * occupant.offset);
        heapOffsets.push_back(heapOffset);
    }
    // Sort allocations
    SortByOffset(allocations);

    return ppx::SUCCESS;
}

Result DescriptorPool::AllocateDescriptorSet(const dx::DescriptorSetLayout* pLayout, dx::DescriptorSet* pSet)
{
    // Early out if there's not enough space available
    bool hasEnoughCBVSRVUAV = (pLayout->GetCountCBVSRVUAV() <= mNumAvailableCBVSRVUAV);
    bool hasEnoughSampler   = (pLayout->GetCountSampler() <= mNumAvailableSampler);
    if (!(hasEnoughCBVSRVUAV && hasEnoughSampler)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }

    // Target offsets
    auto& heapOffsets = pSet->GetHeapOffsets();

    // CBVSRVUAV
    {
        auto& ranges = pLayout->GetRangesCBVSRVUAV();
        if (!ranges.empty()) {
            D3D12_CPU_DESCRIPTOR_HANDLE heapStart           = mHeapCBVSRVUAV->GetCPUDescriptorHandleForHeapStart();
            UINT                        handleIncrementSize = ToApi(GetDevice())->GetHandleIncrementSizeCBVSRVUAV();

            Result ppxres = AllocateRanges(
                pSet,
                ranges,
                mHeapSizeCBVSRVUAV,
                heapStart,
                handleIncrementSize,
                mAllocationsCBVSRVUAV,
                heapOffsets);
            if (Failed(ppxres)) {
                return ppxres;
            }
        }
    }

    //// Sampler
    //{
    //    auto& ranges = pLayout->GetRangesSampler();
    //    if (!ranges.empty()) {
    //        Result ppxres = AllocateRanges(pSet, ranges, mHeapSizeSampler, mAllocationsSampler, heapOffsets);
    //        if (Failed(ppxres)) {
    //            return ppxres;
    //        }
    //    }
    //}

    return ppx::SUCCESS;
}

void DescriptorPool::FreeDescriptorSet(const dx::DescriptorSet* pSet)
{
    // Bail if pSet wasn't allocated from this pool
    dx::DescriptorPool* pPool = ToApi(pSet->GetPool());
    if (pPool != this) {
        return;
    }

    // Remove all CBVSRVUAV allocations that match pSet
    mAllocationsCBVSRVUAV.erase(
        std::remove_if(
            std::begin(mAllocationsCBVSRVUAV),
            std::end(mAllocationsCBVSRVUAV),
            [pSet](const Allocation& elem) -> bool {
                bool isMatch = (elem.pSet == pSet);
                return isMatch; }),
        std::end(mAllocationsCBVSRVUAV));

    // Remove all Sampler allocations that match pSet
    mAllocationsSampler.erase(
        std::remove_if(
            std::begin(mAllocationsSampler),
            std::end(mAllocationsSampler),
            [pSet](const Allocation& elem) -> bool {
                bool isMatch = (elem.pSet == pSet);
                return isMatch; }),
        std::end(mAllocationsSampler));
}

// -------------------------------------------------------------------------------------------------
// DescriptorSet
// -------------------------------------------------------------------------------------------------
Result DescriptorSet::CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo)
{
    D3D12DevicePtr device = ToApi(GetDevice())->GetDxDevice();

    // Heap sizes
    mHeapSizeCBVSRVUAV = static_cast<UINT>(ToApi(pCreateInfo->pLayout)->GetCountCBVSRVUAV());
    mHeapSizeSampler   = static_cast<UINT>(ToApi(pCreateInfo->pLayout)->GetCountSampler());

    // Allocate CBVSRVUAV heap
    if (mHeapSizeCBVSRVUAV > 0) {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors             = mHeapSizeCBVSRVUAV;
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask                   = 0;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapCBVSRVUAV));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(CBVSRVUAV) failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Allocate Sampler heap
    if (mHeapSizeSampler > 0) {
        // CPU
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors             = mHeapSizeSampler;
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask                   = 0;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapSampler));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(Sampler) failed");
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Build CBVSRVUAV offsets
    if (mHeapSizeCBVSRVUAV > 0) {
        UINT                        incrementSize = ToApi(GetDevice())->GetHandleIncrementSizeCBVSRVUAV();
        D3D12_CPU_DESCRIPTOR_HANDLE heapStart     = mHeapCBVSRVUAV->GetCPUDescriptorHandleForHeapStart();
        UINT                        offset        = 0;

        auto& ranges = ToApi(pCreateInfo->pLayout)->GetRangesCBVSRVUAV();
        for (size_t i = 0; i < ranges.size(); ++i) {
            const dx::DescriptorSetLayout::DescriptorRange& range = ranges[i];

            // Fill out heap offset
            HeapOffset heapOffset           = {};
            heapOffset.binding              = range.binding;
            heapOffset.offset               = offset;
            heapOffset.descriptorHandle.ptr = heapStart.ptr + static_cast<SIZE_T>(offset * incrementSize);

            // Store heap offset
            mHeapOffsets.push_back(heapOffset);

            // Increment offset by the range count
            offset += static_cast<UINT>(range.count);
        }
    }

    return ppx::SUCCESS;
}

void DescriptorSet::DestroyApiObjects()
{
    mHeapOffsets.clear();

    if (mHeapCBVSRVUAV) {
        mHeapCBVSRVUAV.Reset();
    }

    if (mHeapSampler) {
        mHeapSampler.Reset();
    }
}

Result DescriptorSet::UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites)
{
    // Check descriptor types
    for (uint32_t writeIndex = 0; writeIndex < writeCount; ++writeIndex) {
        const grfx::WriteDescriptor& srcWrite               = pWrites[writeIndex];
        bool                         isCombinedImageSampler = (srcWrite.type == grfx::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        bool                         isUniformBufferDynamic = (srcWrite.type == grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
        bool                         isStorageBufferDynamic = (srcWrite.type == grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
        bool                         isInputAtachment       = (srcWrite.type == grfx::DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
        bool                         isUnsupported          = isCombinedImageSampler || isUniformBufferDynamic || isStorageBufferDynamic || isInputAtachment;
        if (isUnsupported) {
            return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
        }
    }

    UINT           handleIncSizeCBVSRVUAV = ToApi(GetDevice())->GetHandleIncrementSizeCBVSRVUAV();
    UINT           handleIncSizeSampler   = ToApi(GetDevice())->GetHandleIncrementSizeSampler();
    D3D12DevicePtr device                 = ToApi(GetDevice())->GetDxDevice();

    for (uint32_t writeIndex = 0; writeIndex < writeCount; ++writeIndex) {
        const grfx::WriteDescriptor& srcWrite = pWrites[writeIndex];

        // Find heap offset
        auto it = FindIf(mHeapOffsets, [srcWrite](const HeapOffset& elem) -> bool { return elem.binding == srcWrite.binding; });
        if (it == std::end(mHeapOffsets)) {
            PPX_ASSERT_MSG(false, "attempted to update binding " << srcWrite.binding << " at array index " << srcWrite.arrayIndex << " but binding is not in set");
            return ppx::ERROR_GRFX_BINDING_NOT_IN_SET;
        }
        const HeapOffset& heapOffset = *it;

        switch (srcWrite.type) {
            default: {
                // This shouldn't happen unless there's a supported descriptor
                // type that isn't handled below.
                PPX_ASSERT_MSG(false, "unknown descriptor type: " << ToString(srcWrite.type) << "(" << srcWrite.type << ")");
                return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
            } break;

            case grfx::DESCRIPTOR_TYPE_SAMPLER: {
            } break;

            case grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: {
            } break;

            case grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER: {
            } break;

            case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
                desc.BufferLocation                  = ToApi(srcWrite.pBuffer)->GetDxResource()->GetGPUVirtualAddress();
                desc.SizeInBytes                     = static_cast<UINT>(srcWrite.pBuffer->GetSize());

                SIZE_T                      ptr    = heapOffset.descriptorHandle.ptr + static_cast<SIZE_T>(handleIncSizeCBVSRVUAV * srcWrite.arrayIndex);
                D3D12_CPU_DESCRIPTOR_HANDLE handle = D3D12_CPU_DESCRIPTOR_HANDLE{ptr};

                device->CreateConstantBufferView(&desc, handle);
            } break;
        }
    }

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------
// DescriptorSetLayout
// -------------------------------------------------------------------------------------------------
Result DescriptorSetLayout::CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo)
{
    // Check descriptor types
    for (size_t i = 0; i < pCreateInfo->bindings.size(); ++i) {
        const grfx::DescriptorBinding& binding = pCreateInfo->bindings[i];

        bool isCombinedImageSampler = (binding.type == grfx::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        bool isUniformBufferDynamic = (binding.type == grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
        bool isStorageBufferDynamic = (binding.type == grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
        bool isInputAtachment       = (binding.type == grfx::DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
        bool isUnsupported          = isCombinedImageSampler || isUniformBufferDynamic || isStorageBufferDynamic || isInputAtachment;
        if (isUnsupported) {
            return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
        }
    }

    // Build descriptor ranges
    for (size_t i = 0; i < pCreateInfo->bindings.size(); ++i) {
        const grfx::DescriptorBinding& binding = pCreateInfo->bindings[i];

        mCountCBVSRVUAV += (binding.type == grfx::DESCRIPTOR_TYPE_SAMPLER) ? 0 : binding.arrayCount;
        mCountSampler += (binding.type == grfx::DESCRIPTOR_TYPE_SAMPLER) ? binding.arrayCount : 0;

        std::vector<DescriptorRange>* pRanges = (binding.type == grfx::DESCRIPTOR_TYPE_SAMPLER) ? &mRangesSampler : &mRangesCBVSRVUAV;

        DescriptorRange info = {};
        info.binding         = binding.binding;
        info.count           = binding.arrayCount;

        pRanges->push_back(info);
    }

    return ppx::SUCCESS;
}

void DescriptorSetLayout::DestroyApiObjects()
{
    mRangesCBVSRVUAV.clear();
    mRangesSampler.clear();
}

} // namespace dx
} // namespace grfx
} // namespace ppx
