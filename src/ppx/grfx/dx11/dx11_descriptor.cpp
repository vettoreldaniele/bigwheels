#include "ppx/grfx/dx11/dx11_descriptor.h"

namespace ppx {
namespace grfx {
namespace dx11 {

// -------------------------------------------------------------------------------------------------
// DescriptorPool
// -------------------------------------------------------------------------------------------------
Result DescriptorPool::CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void DescriptorPool::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// DescriptorSet
// -------------------------------------------------------------------------------------------------
Result DescriptorSet::CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void DescriptorSet::DestroyApiObjects()
{
}

Result DescriptorSet::UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites)
{
    return ppx::ERROR_FAILED;;
}

// -------------------------------------------------------------------------------------------------
// DescriptorSetLayout
// -------------------------------------------------------------------------------------------------
Result DescriptorSetLayout::CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void DescriptorSetLayout::DestroyApiObjects()
{
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
