#include "ppx/grfx/vk/vk_query.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_instance.h"

namespace ppx {
namespace grfx {
namespace vk {

Result QueryPool::CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo)
{
    VkQueryPoolCreateInfo vkci = {VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    vkci.flags                 = 0;
    vkci.queryType             = ToVkQueryType(pCreateInfo->type);
    vkci.queryCount            = pCreateInfo->count;
    vkci.pipelineStatistics    = 0;

    if (vkci.queryType == VK_QUERY_TYPE_PIPELINE_STATISTICS) {
        vkci.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT |
                                  VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;
    }

    VkResult vkres = vkCreateQueryPool(ToApi(GetDevice())->GetVkDevice(), &vkci, nullptr, &mQueryPool);
    if (vkres != VK_SUCCESS) {
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void QueryPool::DestroyApiObjects()
{
    if (mQueryPool) {
        vkDestroyQueryPool(
            ToApi(GetDevice())->GetVkDevice(),
            mQueryPool,
            nullptr);

        mQueryPool.Reset();
    }
}

void QueryPool::Reset(uint32_t firstQuery, uint32_t queryCount)
{
    if (GetDevice()->GetInstance()->GetApi() == grfx::API_VK_1_1) {
        ToApi(GetDevice())->ResetQueryPoolEXT(
            mQueryPool,
            firstQuery,
            queryCount);    
    }
    else 
    {
        // Assumes Vulkan 1.2
        vkResetQueryPool(
            ToApi(GetDevice())->GetVkDevice(),
            mQueryPool,
            firstQuery,
            queryCount);
    }
}

} // namespace vk
} // namespace grfx
} // namespace ppx
