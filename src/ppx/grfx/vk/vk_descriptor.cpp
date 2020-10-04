#include "ppx/grfx/vk/vk_descriptor.h"

namespace ppx {
namespace grfx {
namespace vk {

// -------------------------------------------------------------------------------------------------
// DescriptorPool
// -------------------------------------------------------------------------------------------------
Result DescriptorPool::CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo)
{
    VkDescriptorPoolCreateInfo vkci = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    //vkci.flags                      = ;
    //vkci.maxSets                    = ;
    //vkci.poolSizeCount              = ;
    //vkci.pPoolSizes                 = ;

    return ppx::ERROR_FAILED;
}

void DescriptorPool::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// DescriptorSet
// -------------------------------------------------------------------------------------------------
Result DescriptorSet::CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void DescriptorSet::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// DescriptorSetLayout
// -------------------------------------------------------------------------------------------------
Result DescriptorSetLayout::CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void DescriptorSetLayout::DestroyApiObjects()
{
}

} // namespace vk
} // namespace grfx
} // namespace ppx
