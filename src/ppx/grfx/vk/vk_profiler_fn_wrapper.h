#ifndef PPX_GRFX_VK_PROFILER_FN_WRAPPHER_H
#define PPX_GRFX_VK_PROFILER_FN_WRAPPHER_H

#include "ppx/grfx/vk//000_vk_config_platform.h"

namespace ppx {
namespace grfx {
namespace vk {

void RegisterProfilerFunctions();

#if defined(PPX_ENABLE_PROFILE_GRFX_API_FUNCTIONS)

VkResult CreateBuffer(
    VkDevice                     device,
    const VkBufferCreateInfo*    pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer*                    pBuffer);

void UpdateDescriptorSets(
    VkDevice                    device,
    uint32_t                    descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t                    descriptorCopyCount,
    const VkCopyDescriptorSet*  pDescriptorCopies);

VkResult QueuePresent(
    VkQueue                 queue,
    const VkPresentInfoKHR* pPresentInfo);

VkResult QueueSubmit(
    VkQueue             queue,
    uint32_t            submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence             fence);

VkResult BeginCommandBuffer(
    VkCommandBuffer                 commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo);

VkResult EndCommandBuffer(
    VkCommandBuffer commandBuffer);

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
    const VkImageMemoryBarrier*  pImageMemoryBarriers);

void CmdBeginRenderPass(
    VkCommandBuffer              commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents            contents);

void CmdEndRenderPass(
    VkCommandBuffer commandBuffer);

void CmdBindDescriptorSets(
    VkCommandBuffer        commandBuffer,
    VkPipelineBindPoint    pipelineBindPoint,
    VkPipelineLayout       layout,
    uint32_t               firstSet,
    uint32_t               descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t               dynamicOffsetCount,
    const uint32_t*        pDynamicOffsets);

void CmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer        buffer,
    VkDeviceSize    offset,
    VkIndexType     indexType);

void CmdBindPipeline(
    VkCommandBuffer     commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline          pipeline);

void CmdBindVertexBuffers(
    VkCommandBuffer     commandBuffer,
    uint32_t            firstBinding,
    uint32_t            bindingCount,
    const VkBuffer*     pBuffers,
    const VkDeviceSize* pOffsets);

void CmdDispatch(
    VkCommandBuffer commandBuffer,
    uint32_t        groupCountX,
    uint32_t        groupCountY,
    uint32_t        groupCountZ);

void CmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t        vertexCount,
    uint32_t        instanceCount,
    uint32_t        firstVertex,
    uint32_t        firstInstance);

void CmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t        indexCount,
    uint32_t        instanceCount,
    uint32_t        firstIndex,
    int32_t         vertexOffset,
    uint32_t        firstInstance);

#else

inline VkResult CreateBuffer(
    VkDevice                     device,
    const VkBufferCreateInfo*    pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer*                    pBuffer)
{
    return vkCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

inline void UpdateDescriptorSets(
    VkDevice                    device,
    uint32_t                    descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t                    descriptorCopyCount,
    const VkCopyDescriptorSet*  pDescriptorCopies)
{
    vkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

inline VkResult QueuePresent(
    VkQueue                 queue,
    const VkPresentInfoKHR* pPresentInfo)
{
    return vkQueuePresentKHR(queue, pPresentInfo);
}

inline VkResult QueueSubmit(
    VkQueue             queue,
    uint32_t            submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence             fence)
{
    return vkQueueSubmit(queue, submitCount, pSubmits, fence);
}

inline VkResult BeginCommandBuffer(
    VkCommandBuffer                 commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo)
{
    return vkBeginCommandBuffer(commandBuffer, pBeginInfo);
}

inline VkResult EndCommandBuffer(
    VkCommandBuffer commandBuffer)
{
    return vkEndCommandBuffer(commandBuffer);
}

inline void CmdPipelineBarrier(
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
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

inline void CmdBeginRenderPass(
    VkCommandBuffer              commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents            contents)
{
    vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

inline void CmdEndRenderPass(
    VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
}

inline void CmdBindDescriptorSets(
    VkCommandBuffer        commandBuffer,
    VkPipelineBindPoint    pipelineBindPoint,
    VkPipelineLayout       layout,
    uint32_t               firstSet,
    uint32_t               descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t               dynamicOffsetCount,
    const uint32_t*        pDynamicOffsets)
{
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

inline void CmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer        buffer,
    VkDeviceSize    offset,
    VkIndexType     indexType)
{
    vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

inline void CmdBindPipeline(
    VkCommandBuffer     commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline          pipeline)
{
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

inline void CmdBindVertexBuffers(
    VkCommandBuffer     commandBuffer,
    uint32_t            firstBinding,
    uint32_t            bindingCount,
    const VkBuffer*     pBuffers,
    const VkDeviceSize* pOffsets)
{
    vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

inline void CmdDispatch(
    VkCommandBuffer commandBuffer,
    uint32_t        groupCountX,
    uint32_t        groupCountY,
    uint32_t        groupCountZ)
{
    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

inline void CmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t        vertexCount,
    uint32_t        instanceCount,
    uint32_t        firstVertex,
    uint32_t        firstInstance)
{
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

inline void CmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t        indexCount,
    uint32_t        instanceCount,
    uint32_t        firstIndex,
    int32_t         vertexOffset,
    uint32_t        firstInstance)
{
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

#endif // defined(PPX_ENABLE_PROFILE_GRFX_FUNCTIONS)

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // PPX_GRFX_VK_PROFILER_FN_WRAPPHER_H
