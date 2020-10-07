#include "ppx/grfx/vk/vk_command.h"
#include "ppx/grfx/vk/vk_buffer.h"
#include "ppx/grfx/vk/vk_descriptor.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_image.h"
#include "ppx/grfx/vk/vk_queue.h"
#include "ppx/grfx/vk/vk_pipeline.h"
#include "ppx/grfx/vk/vk_render_pass.h"

namespace ppx {
namespace grfx {
namespace vk {

// -------------------------------------------------------------------------------------------------
// CommandBuffer
// -------------------------------------------------------------------------------------------------
Result CommandBuffer::CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo)
{
    VkCommandBufferAllocateInfo vkai = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    vkai.commandPool                 = ToApi(pCreateInfo->pPool)->GetVkCommandPool();
    vkai.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkai.commandBufferCount          = 1;

    VkResult vkres = vkAllocateCommandBuffers(
        ToApi(GetDevice())->GetVkDevice(),
        &vkai,
        &mCommandBuffer);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkAllocateCommandBuffers failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void CommandBuffer::DestroyApiObjects()
{
    if (mCommandBuffer) {
        vkFreeCommandBuffers(
            ToApi(GetDevice())->GetVkDevice(),
            ToApi(mCreateInfo.pPool)->GetVkCommandPool(),
            1,
            mCommandBuffer);

        mCommandBuffer.Reset();
    }
}

Result CommandBuffer::Begin()
{
    VkCommandBufferBeginInfo vkbi = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

    VkResult vkres = vkBeginCommandBuffer(mCommandBuffer, &vkbi);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkBeginCommandBuffer failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

Result CommandBuffer::End()
{
    VkResult vkres = vkEndCommandBuffer(mCommandBuffer);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkEndCommandBuffer failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void CommandBuffer::BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo)
{
    VkRect2D rect = {};
    rect.offset   = {pBeginInfo->renderArea.x, pBeginInfo->renderArea.x};
    rect.extent   = {pBeginInfo->renderArea.width, pBeginInfo->renderArea.height};

    uint32_t     clearValueCount                         = 0;
    VkClearValue clearValues[PPX_MAX_RENDER_TARGETS + 1] = {};

    for (uint32_t i = 0; i < pBeginInfo->RTVClearCount; ++i) {
        clearValues[i].color = ToVkClearColorValue(pBeginInfo->RTVClearValues[i]);
        ++clearValueCount;
    }

    if (pBeginInfo->pRenderPass->GetDepthStencilView()) {
        uint32_t i                  = clearValueCount;
        clearValues[i].depthStencil = ToVkClearDepthStencilValue(pBeginInfo->DSVClearValue);
        ++clearValueCount;
    }

    VkRenderPassBeginInfo vkbi = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    vkbi.renderPass            = ToApi(pBeginInfo->pRenderPass)->GetVkRenderPass();
    vkbi.framebuffer           = ToApi(pBeginInfo->pRenderPass)->GetVkFramebuffer();
    vkbi.renderArea            = rect;
    vkbi.clearValueCount       = clearValueCount;
    vkbi.pClearValues          = clearValues;

    vkCmdBeginRenderPass(mCommandBuffer, &vkbi, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(mCommandBuffer);
}

void CommandBuffer::TransitionImageLayout(
    const grfx::Image*  pImage,
    uint32_t            mipLevel,
    uint32_t            mipLevelCount,
    uint32_t            arrayLayer,
    uint32_t            arrayLayerCount,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState)
{
    PPX_ASSERT_NULL_ARG(pImage);

    if (mipLevelCount == PPX_ALL_MIP_LEVELS) {
        mipLevelCount = pImage->GetMipLevelCount();
    }

    if (arrayLayerCount == PPX_ALL_ARRAY_LAYERS) {
        arrayLayerCount = pImage->GetArrayLayerCount();
    }

    const vk::Image* pApiImage = ToApi(pImage);

    VkPipelineStageFlags srcStageMask    = InvalidValue<VkPipelineStageFlags>();
    VkPipelineStageFlags dstStageMask    = InvalidValue<VkPipelineStageFlags>();
    VkAccessFlags        srcAccessMask   = InvalidValue<VkAccessFlags>();
    VkAccessFlags        dstAccessMask   = InvalidValue<VkAccessFlags>();
    VkImageLayout        oldLayout       = InvalidValue<VkImageLayout>();
    VkImageLayout        newLayout       = InvalidValue<VkImageLayout>();
    VkDependencyFlags    dependencyFlags = 0;

    Result ppxres = ToVkBarrierSrc(beforeState, srcStageMask, srcAccessMask, oldLayout);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "couldn't get src barrier data");

    ppxres = ToVkBarrierDst(afterState, dstStageMask, dstAccessMask, newLayout);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "couldn't get dst barrier data");

    VkImageMemoryBarrier barrier            = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.srcAccessMask                   = srcAccessMask;
    barrier.dstAccessMask                   = dstAccessMask;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = pApiImage->GetVkImage();
    barrier.subresourceRange.aspectMask     = pApiImage->GetVkImageAspectFlags();
    barrier.subresourceRange.baseMipLevel   = mipLevel;
    barrier.subresourceRange.levelCount     = mipLevelCount;
    barrier.subresourceRange.baseArrayLayer = arrayLayer;
    barrier.subresourceRange.layerCount     = arrayLayerCount;

    vkCmdPipelineBarrier(
        mCommandBuffer, // commandBuffer
        srcStageMask,   // srcStageMask
        dstStageMask,   // dstStageMask
        0,              // dependencyFlags
        0,              // memoryBarrierCount
        nullptr,        // pMemoryBarriers
        0,              // bufferMemoryBarrierCount
        nullptr,        // pBufferMemoryBarriers
        1,              // imageMemoryBarrierCount
        &barrier);      // pImageMemoryBarriers);
}

void CommandBuffer::SetViewports(uint32_t viewportCount, const grfx::Viewport* pViewports)
{
    VkViewport viewports[PPX_MAX_VIEWPORTS] = {};
    for (uint32_t i = 0; i < viewportCount; ++i) {
        // clang-format off
        viewports[i].x        =  pViewports[i].x;
        viewports[i].y        =  pViewports[i].height;
        viewports[i].width    =  pViewports[i].width;
        viewports[i].height   = -pViewports[i].height;
        viewports[i].minDepth =  pViewports[i].minDepth;
        viewports[i].maxDepth =  pViewports[i].maxDepth;
        // clang-format on
    }

    vkCmdSetViewport(
        mCommandBuffer,
        0,
        viewportCount,
        viewports);
}

void CommandBuffer::SetScissors(uint32_t scissorCount, const grfx::Rect* pScissors)
{
    vkCmdSetScissor(
        mCommandBuffer,
        0,
        scissorCount,
        reinterpret_cast<const VkRect2D*>(pScissors));
}

void CommandBuffer::BindGraphicsDescriptorSets(const grfx::PipelineInterface* pInterface, uint32_t setCount, const grfx::DescriptorSet* const* ppSets)
{
    PPX_ASSERT_NULL_ARG(pInterface);

    VkDescriptorSet vkSets[PPX_MAX_BOUND_DESCRIPTOR_SETS] = {VK_NULL_HANDLE};

    for (uint32_t i = 0; i < setCount; ++i) {
        vkSets[i] = ToApi(ppSets[i])->GetVkDescriptorSet();
    }

    vkCmdBindDescriptorSets(
        mCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        ToApi(pInterface)->GetVkPipelineLayout(),
        0,
        1,
        vkSets,
        0,
        nullptr);
}

void CommandBuffer::BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)
{
    PPX_ASSERT_NULL_ARG(pPipeline);

    vkCmdBindPipeline(
        mCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        ToApi(pPipeline)->GetVkPipeline());
}

void CommandBuffer::BindComputeDescriptorSets(const grfx::PipelineInterface* pInterface, uint32_t setCount, const grfx::DescriptorSet* const* ppSets)
{
    PPX_ASSERT_NULL_ARG(pInterface);

    VkDescriptorSet vkSets[PPX_MAX_BOUND_DESCRIPTOR_SETS] = {VK_NULL_HANDLE};

    for (uint32_t i = 0; i < setCount; ++i) {
        vkSets[i] = ToApi(ppSets[i])->GetVkDescriptorSet();
    }

    vkCmdBindDescriptorSets(
        mCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        ToApi(pInterface)->GetVkPipelineLayout(),
        0,
        1,
        vkSets,
        0,
        nullptr);
}

void CommandBuffer::BindComputePipeline(const grfx::ComputePipeline* pPipeline)
{
    PPX_ASSERT_NULL_ARG(pPipeline);

    vkCmdBindPipeline(
        mCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        ToApi(pPipeline)->GetVkPipeline());
}

void CommandBuffer::BindIndexBuffer(const grfx::IndexBufferView* pView)
{
    vkCmdBindIndexBuffer(
        mCommandBuffer,
        ToApi(pView->pBuffer)->GetVkBuffer(),
        static_cast<VkDeviceSize>(pView->offset),
        ToVkIndexType(pView->indexType));
}

void CommandBuffer::BindVertexBuffers(uint32_t viewCount, const grfx::VertexBufferView* pViews)
{
    PPX_ASSERT_MSG(viewCount < PPX_MAX_VERTEX_ATTRIBUTES, "viewCount exceeds PPX_MAX_VERTEX_ATTRIBUTES");

    VkBuffer     buffers[PPX_MAX_RENDER_TARGETS] = {VK_NULL_HANDLE};
    VkDeviceSize offsets[PPX_MAX_RENDER_TARGETS] = {0};

    for (uint32_t i = 0; i < viewCount; ++i) {
        buffers[i] = ToApi(pViews[i].pBuffer)->GetVkBuffer();
        offsets[i] = static_cast<VkDeviceSize>(pViews[i].offset);
    }

    vkCmdBindVertexBuffers(
        mCommandBuffer,
        0,
        viewCount,
        buffers,
        offsets);
}

void CommandBuffer::Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t  vertexOffset,
        uint32_t firstInstance)
{
    vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::Dispatch(
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ)
{
    vkCmdDispatch(mCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::CopyBufferToImage(
    const grfx::BufferToImageCopyInfo* pCopyInfo,
    const grfx::Buffer*                pSrcBuffer,
    const grfx::Image*                 pDstImage)
{
    PPX_ASSERT_NULL_ARG(pCopyInfo);
    PPX_ASSERT_NULL_ARG(pSrcBuffer);
    PPX_ASSERT_NULL_ARG(pDstImage);

    VkBufferImageCopy region               = {};
    region.bufferOffset                    = static_cast<VkDeviceSize>(pCopyInfo->srcBuffer.offset);
    region.bufferRowLength                 = pCopyInfo->srcBuffer.footprintWidth;
    region.bufferImageHeight               = pCopyInfo->srcBuffer.footprintHeight;
    region.imageSubresource.aspectMask     = ToApi(pDstImage)->GetVkImageAspectFlags();
    region.imageSubresource.mipLevel       = pCopyInfo->dstImage.mipLevel;
    region.imageSubresource.baseArrayLayer = pCopyInfo->dstImage.arrayLayer;
    region.imageSubresource.layerCount     = pCopyInfo->dstImage.arrayLayerCount;
    region.imageOffset.x                   = pCopyInfo->dstImage.x;
    region.imageOffset.y                   = pCopyInfo->dstImage.y;
    region.imageOffset.z                   = pCopyInfo->dstImage.z;
    region.imageExtent.width               = pCopyInfo->dstImage.width;
    region.imageExtent.height              = pCopyInfo->dstImage.height;
    region.imageExtent.depth               = pCopyInfo->dstImage.depth;

    vkCmdCopyBufferToImage(
        mCommandBuffer,
        ToApi(pSrcBuffer)->GetVkBuffer(),
        ToApi(pDstImage)->GetVkImage(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

// -------------------------------------------------------------------------------------------------
// CommandPool
// -------------------------------------------------------------------------------------------------
Result CommandPool::CreateApiObjects(const grfx::CommandPoolCreateInfo* pCreateInfo)
{
    VkCommandPoolCreateInfo vkci = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    vkci.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkci.queueFamilyIndex        = ToApi(pCreateInfo->pQueue)->GetQueueFamilyIndex();

    VkResult vkres = vkCreateCommandPool(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mCommandPool);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkCreateCommandPool failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void CommandPool::DestroyApiObjects()
{
    if (mCommandPool) {
        vkDestroyCommandPool(
            ToApi(GetDevice())->GetVkDevice(),
            mCommandPool,
            nullptr);

        mCommandPool.Reset();
    }
}

} // namespace vk
} // namespace grfx
} // namespace ppx
