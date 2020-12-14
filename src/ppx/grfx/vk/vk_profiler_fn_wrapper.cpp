#include "ppx/grfx/vk/vk_profiler_fn_wrapper.h"
#include "ppx/profiler.h"

namespace ppx {
namespace grfx {
namespace vk {

static ProfilerEventToken s_vkCreateBuffer          = 0;
static ProfilerEventToken s_vkUpdateDescriptorSets  = 0;
static ProfilerEventToken s_vkCmdBindDescriptorSets = 0;
static ProfilerEventToken s_vkCmdBindIndexBuffer    = 0;
static ProfilerEventToken s_vkCmdBindPipeline       = 0;
static ProfilerEventToken s_vkCmdBindVertexBuffers  = 0;
static ProfilerEventToken s_vkCmdDispatch           = 0;
static ProfilerEventToken s_vkCmdDrawIndexed        = 0;

void RegisterProfilerFunctions()
{
    Result ppxres = ppx::ERROR_FAILED;

#define REGISTER_EVENT_PARAMS(VKFN) #VKFN, &s_##VKFN

    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCreateBuffer)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkUpdateDescriptorSets)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindDescriptorSets)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindIndexBuffer)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindPipeline)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindVertexBuffers)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdDispatch)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdDrawIndexed)));

#undef REGISTER_EVENT_PARAMS
}

#if defined(PPX_ENABLE_PROFILE_GRFX_API_FUNCTIONS)

VkResult CreateBuffer(
    VkDevice                     device,
    const VkBufferCreateInfo*    pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer*                    pBuffer)
{
    ProfilerScopedEventSample eventSample(s_vkCreateBuffer);
    return vkCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

void UpdateDescriptorSets(
    VkDevice                    device,
    uint32_t                    descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t                    descriptorCopyCount,
    const VkCopyDescriptorSet*  pDescriptorCopies)
{
    ProfilerScopedEventSample eventSample(s_vkUpdateDescriptorSets);
    vkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

void CmdBindDescriptorSets(
    VkCommandBuffer        commandBuffer,
    VkPipelineBindPoint    pipelineBindPoint,
    VkPipelineLayout       layout,
    uint32_t               firstSet,
    uint32_t               descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t               dynamicOffsetCount,
    const uint32_t*        pDynamicOffsets)
{
    ProfilerScopedEventSample eventSample(s_vkCmdBindDescriptorSets);
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

void CmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer        buffer,
    VkDeviceSize    offset,
    VkIndexType     indexType)
{
    ProfilerScopedEventSample eventSample(s_vkCmdBindIndexBuffer);
    vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

void CmdBindPipeline(
    VkCommandBuffer     commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline          pipeline)
{
    ProfilerScopedEventSample eventSample(s_vkCmdBindVertexBuffers);
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void CmdBindVertexBuffers(
    VkCommandBuffer     commandBuffer,
    uint32_t            firstBinding,
    uint32_t            bindingCount,
    const VkBuffer*     pBuffers,
    const VkDeviceSize* pOffsets)
{
    ProfilerScopedEventSample eventSample(s_vkCmdBindPipeline);
    vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

void CmdDispatch(
    VkCommandBuffer commandBuffer,
    uint32_t        groupCountX,
    uint32_t        groupCountY,
    uint32_t        groupCountZ)
{
    ProfilerScopedEventSample eventSample(s_vkCmdDispatch);
    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void CmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t        indexCount,
    uint32_t        instanceCount,
    uint32_t        firstIndex,
    int32_t         vertexOffset,
    uint32_t        firstInstance)
{
    ProfilerScopedEventSample eventSample(s_vkCmdDrawIndexed);
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

#endif // defined(PPX_ENABLE_PROFILE_GRFX_FUNCTIONS)

} // namespace vk
} // namespace grfx
} // namespace ppx
