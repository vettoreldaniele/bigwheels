#include "ppx/grfx/dx11/dx11_descriptor.h"
#include "ppx/grfx/dx11/dx11_buffer.h"
#include "ppx/grfx/dx11/dx11_device.h"
#include "ppx/grfx/dx11/dx11_image.h"

namespace ppx {
namespace grfx {
namespace dx11 {

// -------------------------------------------------------------------------------------------------
// DescriptorPool
// -------------------------------------------------------------------------------------------------
void DescriptorPool::UpdateRemainingCount()
{
    mRemainingCountCBV     = mTotalCountCBV - mAllocatedCountCBV;
    mRemainingCountSRV     = mTotalCountSRV - mAllocatedCountSRV;
    mRemainingCountUAV     = mTotalCountUAV - mAllocatedCountUAV;
    mRemainingCountSampler = mTotalCountSampler - mAllocatedCountSampler;
}

Result DescriptorPool::CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo)
{
    mTotalCountCBV     = pCreateInfo->uniformBuffer;
    mTotalCountSRV     = pCreateInfo->sampledImage + pCreateInfo->uniformTexelBuffer + pCreateInfo->structuredBuffer;
    mTotalCountUAV     = pCreateInfo->storageBuffer + pCreateInfo->storageTexelBuffer + pCreateInfo->storageImage;
    mTotalCountSampler = pCreateInfo->sampler;

    UpdateRemainingCount();

    return ppx::SUCCESS;
}

Result DescriptorPool::AllocateSet(
    uint32_t bindingCountCBV,
    uint32_t bindingCountSRV,
    uint32_t bindingCountUAV,
    uint32_t bindingCountSampler)
{
    bool hasAvailableCBV     = (bindingCountCBV <= mRemainingCountCBV);
    bool hasAvailableSRV     = (bindingCountSRV <= mRemainingCountSRV);
    bool hasAvailableUAV     = (bindingCountUAV <= mRemainingCountUAV);
    bool hasAvailableSampler = (bindingCountSampler <= mRemainingCountSampler);
    bool hasAvailable        = (hasAvailableCBV && hasAvailableSRV && hasAvailableUAV && hasAvailableSampler);
    if (!hasAvailable) {
        return ppx::ERROR_GRFX_DESCRIPTOR_COUNT_EXCEEDED;
    }

    mAllocatedCountCBV += bindingCountCBV;
    mAllocatedCountSRV += bindingCountSRV;
    mAllocatedCountUAV += bindingCountUAV;
    mAllocatedCountSampler += bindingCountSampler;

    UpdateRemainingCount();

    return ppx::SUCCESS;
}

void DescriptorPool::FreeSet(
    uint32_t bindingCountCBV,
    uint32_t bindingCountSRV,
    uint32_t bindingCountUAV,
    uint32_t bindingCountSampler)
{
    if (bindingCountCBV <= mAllocatedCountCBV) {
        mAllocatedCountCBV -= bindingCountCBV;
    }
    else {
        PPX_ASSERT_MSG(false, "attempting to free descriptor bindings more than allocated (CBV)");
    }

    if (bindingCountSRV <= mAllocatedCountSRV) {
        mAllocatedCountSRV -= bindingCountSRV;
    }
    else {
        PPX_ASSERT_MSG(false, "attempting to free descriptor bindings more than allocated (SRV)");
    }

    if (bindingCountUAV <= mAllocatedCountUAV) {
        mAllocatedCountUAV -= bindingCountUAV;
    }
    else {
        PPX_ASSERT_MSG(false, "attempting to free descriptor bindings more than allocated (UAV)");
    }

    if (bindingCountSampler <= mAllocatedCountSampler) {
        mAllocatedCountSampler -= bindingCountSampler;
    }
    else {
        PPX_ASSERT_MSG(false, "attempting to free descriptor bindings more than allocated (Sampler)");
    }

    UpdateRemainingCount();
}

void DescriptorPool::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// DescriptorSet
// -------------------------------------------------------------------------------------------------
Result DescriptorSet::CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo)
{
    auto pApiLayout = ToApi(pCreateInfo->pLayout);
    auto pApiPool   = ToApi(pCreateInfo->pPool);

    uint32_t countCBV     = pApiLayout->GetBindingCountCBV();
    uint32_t countSRV     = pApiLayout->GetBindingCountSRV();
    uint32_t countUAV     = pApiLayout->GetBindingCountUAV();
    uint32_t countSampler = pApiLayout->GetBindingCountSampler();

    Result ppxres = pApiPool->AllocateSet(countCBV, countSRV, countUAV, countSampler);
    if (Failed(ppxres)) {
        return ppxres;
    }

    auto         bindings     = pApiLayout->GetBindings();
    const size_t bindingCount = bindings.size();
    for (size_t i = 0; i < bindingCount; ++i) {
        const grfx::DescriptorBinding& binding = bindings[i];

        switch (binding.type) {
            default: {
                // This shouldn't happen unless there's a supported descriptor type that isn't handled below.
                PPX_ASSERT_MSG(false, "unknown descriptor type: " << ToString(binding.type) << "(" << binding.type << ")");
                return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
            } break;

            // CBV
            case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                DescriptorResourceBinding resourceBinding = {binding.binding, grfx::D3D_DESCRIPTOR_TYPE_CBV, binding.shaderVisiblity};
                resourceBinding.resources.resize(binding.arrayCount);
                mResourceBindings.push_back(resourceBinding);
            } break;

            // SRV
            case grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER: {
                DescriptorResourceBinding resourceBinding = {binding.binding, grfx::D3D_DESCRIPTOR_TYPE_SRV, binding.shaderVisiblity};
                resourceBinding.resources.resize(binding.arrayCount);
                mResourceBindings.push_back(resourceBinding);
            } break;

            // UAV
            case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE: {
                DescriptorResourceBinding resourceBinding = {binding.binding, grfx::D3D_DESCRIPTOR_TYPE_UAV, binding.shaderVisiblity};
                resourceBinding.resources.resize(binding.arrayCount);
                mResourceBindings.push_back(resourceBinding);
            } break;

            // SAMPLER
            case grfx::DESCRIPTOR_TYPE_SAMPLER: {
                DescriptorResourceBinding resourceBinding = {binding.binding, grfx::D3D_DESCRIPTOR_TYPE_SAMPLER, binding.shaderVisiblity};
                resourceBinding.resources.resize(binding.arrayCount);
                mResourceBindings.push_back(resourceBinding);
            } break;
        }
    }

    PPX_ASSERT_MSG((mResourceBindings.size() == bindings.size()), "resource bindings count doesn't match descriptor binding count");

    return ppx::SUCCESS;
}

void DescriptorSet::DestroyApiObjects()
{
    auto     pApiLayout   = ToApi(mCreateInfo.pLayout);
    uint32_t countCBV     = pApiLayout->GetBindingCountCBV();
    uint32_t countSRV     = pApiLayout->GetBindingCountSRV();
    uint32_t countUAV     = pApiLayout->GetBindingCountUAV();
    uint32_t countSampler = pApiLayout->GetBindingCountSampler();

    auto pApiPool = ToApi(mCreateInfo.pPool);
    pApiPool->FreeSet(countCBV, countSRV, countUAV, countSampler);
}

Result DescriptorSet::UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites)
{
    for (uint32_t writeIndex = 0; writeIndex < writeCount; ++writeIndex) {
        const grfx::WriteDescriptor& write = pWrites[writeIndex];

        // Find resource binding
        auto it = FindIf(
            mResourceBindings,
            [&write](const DescriptorResourceBinding& elem) -> bool {
                        bool isMatch = (elem.binding == write.binding);
                        return isMatch; });
        if (it == std::end(mResourceBindings)) {
            PPX_ASSERT_MSG(false, "binding " << write.binding << " is not in set");
            return ppx::ERROR_GRFX_BINDING_NOT_IN_SET;
        }
        // Make sure the write's array index in bounds
        const uint32_t arrayCount = CountU32(it->resources);
        if (write.arrayIndex >= arrayCount) {
            PPX_ASSERT_MSG(false, "write array index (" << write.arrayIndex << ") exceeds array count (" << arrayCount << ")");
            return ppx::ERROR_OUT_OF_RANGE;
        }

        switch (write.type) {
            default: {
                // This shouldn't happen unless there's a supported descriptor type that isn't handled below.
                PPX_ASSERT_MSG(false, "unknown descriptor type: " << ToString(write.type) << "(" << write.type << ")");
                return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
            } break;

            // CBV
            case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                it->resources[write.arrayIndex] = ToApi(write.pBuffer)->GetDxBuffer();
            } break;

            // SRV
            case grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: {
                it->resources[write.arrayIndex] = static_cast<const dx11::SampledImageView*>(write.pImageView)->GetDxShaderResourceView();
            } break;

            case grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER: {
                typename D3D11ShaderResourceViewPtr::InterfaceType* pSRV = nullptr;
                Result ppxres = ToApi(GetDevice())->GetStructuredBufferSRV(write.pBuffer, write.structuredElementCount, &pSRV);
                if (Failed(ppxres)) {
                    return ppxres;
                }
                it->resources[write.arrayIndex] = pSRV;
            } break;

            // UAV
            case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER: {
                PPX_ASSERT_MSG(false, "not implemented");
            } break;

            case grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE: {
                it->resources[write.arrayIndex] = static_cast<const dx11::StorageImageView*>(write.pImageView)->GetDxUnorderedAccessView();
            } break;

            // SAMPLER
            case grfx::DESCRIPTOR_TYPE_SAMPLER: {
                it->resources[write.arrayIndex] = ToApi(write.pSampler)->GetDxSamplerState();
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
    const size_t bindingCount = pCreateInfo->bindings.size();
    for (size_t i = 0; i < bindingCount; ++i) {
        const grfx::DescriptorBinding& binding = pCreateInfo->bindings[i];

        switch (binding.type) {
            default: {
                // This shouldn't happen unless there's a supported descriptor type that isn't handled below.
                PPX_ASSERT_MSG(false, "unknown descriptor type: " << ToString(binding.type) << "(" << binding.type << ")");
                return ppx::ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE;
            } break;

            // CBV
            case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                mBindingCountCBV += binding.arrayCount;
            } break;

            // SRV
            case grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER: {
                mBindingCountSRV += binding.arrayCount;
            } break;

            // UAV
            case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            case grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE: {
                mBindingCountUAV += binding.arrayCount;
            } break;

            // SAMPLER
            case grfx::DESCRIPTOR_TYPE_SAMPLER: {
                mBindingCountSampler += binding.arrayCount;
            } break;
        }
    }
    return ppx::SUCCESS;
}

void DescriptorSetLayout::DestroyApiObjects()
{
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
