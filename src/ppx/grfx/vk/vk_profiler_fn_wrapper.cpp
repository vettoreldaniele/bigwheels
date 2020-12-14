#include "ppx/grfx/vk/vk_profiler_fn_wrapper.h"
#include "ppx/profiler.h"

namespace ppx {
namespace grfx {
namespace vk {

static ProfilerEventToken s_vkCreateBuffer          = 0;
static ProfilerEventToken s_vkUpdateDescriptorSets  = 0;
static ProfilerEventToken s_vkQueuePresent          = 0;
static ProfilerEventToken s_vkQueueSubmit           = 0;
static ProfilerEventToken s_vkCmdPipelineBarrier    = 0;
static ProfilerEventToken s_vkBeginCommandBuffer    = 0;
static ProfilerEventToken s_vkEndCommandBuffer      = 0;
static ProfilerEventToken s_vkCmdBeginRenderPass    = 0;
static ProfilerEventToken s_vkCmdEndRenderPass      = 0;
static ProfilerEventToken s_vkCmdBindDescriptorSets = 0;
static ProfilerEventToken s_vkCmdBindIndexBuffer    = 0;
static ProfilerEventToken s_vkCmdBindPipeline       = 0;
static ProfilerEventToken s_vkCmdBindVertexBuffers  = 0;
static ProfilerEventToken s_vkCmdDispatch           = 0;
static ProfilerEventToken s_vkCmdDraw               = 0;
static ProfilerEventToken s_vkCmdDrawIndexed        = 0;

void RegisterProfilerFunctions()
{
    Result ppxres = ppx::ERROR_FAILED;

#define REGISTER_EVENT_PARAMS(VKFN) #VKFN, &s_##VKFN

    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCreateBuffer)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkUpdateDescriptorSets)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkQueuePresent)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkQueueSubmit)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdPipelineBarrier)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkBeginCommandBuffer)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkEndCommandBuffer)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindDescriptorSets)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindIndexBuffer)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindPipeline)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdBindVertexBuffers)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdDispatch)));
    PPX_CHECKED_CALL(Profiler::RegisterGrfxApiFnEvent(REGISTER_EVENT_PARAMS(vkCmdDraw)));
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

VkResult QueuePresent(
    VkQueue                 queue,
    const VkPresentInfoKHR* pPresentInfo)
{
    ProfilerScopedEventSample eventSample(s_vkQueuePresent);
    return vkQueuePresentKHR(queue, pPresentInfo);
}

VkResult QueueSubmit(
    VkQueue             queue,
    uint32_t            submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence             fence)
{
    ProfilerScopedEventSample eventSample(s_vkQueueSubmit);
    return vkQueueSubmit(queue, submitCount, pSubmits, fence);
}

VkResult BeginCommandBuffer(
    VkCommandBuffer                 commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo)
{
    ProfilerScopedEventSample eventSample(s_vkBeginCommandBuffer);
    return vkBeginCommandBuffer(commandBuffer, pBeginInfo);
}

VkResult EndCommandBuffer(
    VkCommandBuffer commandBuffer)
{
    ProfilerScopedEventSample eventSample(s_vkEndCommandBuffer);
    return vkEndCommandBuffer(commandBuffer);
}

void CmdPipelineBarrier(
    VkCommandBuffer              commandBuffer,
    VkPipelineStageFlags         srcStageMask,
    VkPipelineStageFlags         dstStageMask,
    VkDependencyFlags            dependencyFlags,
    uint32_t                     memoryBarrierCount,
    const VkMemoryBarrier*       pMemoryBarriers,
    uint32_t                     bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t                     imageMemoryBarrierCount,
    const VkImageMemoryBarrier*  pImageMemoryBarriers)
{
    ProfilerScopedEventSample eventSample(s_vkCmdPipelineBarrier);
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

void CmdBeginRenderPass(
    VkCommandBuffer              commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents            contents)
{
    ProfilerScopedEventSample eventSample(s_vkUpdateDescriptorSets);
    vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

void CmdEndRenderPass(
    VkCommandBuffer commandBuffer)
{
    ProfilerScopedEventSample eventSample(s_vkUpdateDescriptorSets);
    vkCmdEndRenderPass(commandBuffer);
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

void CmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t        vertexCount,
    uint32_t        instanceCount,
    uint32_t        firstVertex,
    uint32_t        firstInstance)
{
    ProfilerScopedEventSample eventSample(s_vkCmdDraw);
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
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
