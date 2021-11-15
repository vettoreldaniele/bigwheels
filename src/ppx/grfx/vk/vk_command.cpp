#include "ppx/grfx/vk/vk_command.h"
#include "ppx/grfx/vk/vk_buffer.h"
#include "ppx/grfx/vk/vk_descriptor.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_image.h"
#include "ppx/grfx/vk/vk_query.h"
#include "ppx/grfx/vk/vk_queue.h"
#include "ppx/grfx/vk/vk_pipeline.h"
#include "ppx/grfx/vk/vk_render_pass.h"

#include "ppx/grfx/vk/vk_profiler_fn_wrapper.h"

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

    VkResult vkres = vk::AllocateCommandBuffers(
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
        vk::FreeCommandBuffers(
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

    VkResult vkres = vk::BeginCommandBuffer(mCommandBuffer, &vkbi);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkBeginCommandBuffer failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

Result CommandBuffer::End()
{
    VkResult vkres = vk::EndCommandBuffer(mCommandBuffer);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG(false, "vkEndCommandBuffer failed: " << ToString(vkres));
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void CommandBuffer::BeginRenderPassImpl(const grfx::RenderPassBeginInfo* pBeginInfo)
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

    vk::CmdBeginRenderPass(mCommandBuffer, &vkbi, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::EndRenderPassImpl()
{
    vk::CmdEndRenderPass(mCommandBuffer);
}

void CommandBuffer::TransitionImageLayout(
    const grfx::Image*  pImage,
    uint32_t            mipLevel,
    uint32_t            mipLevelCount,
    uint32_t            arrayLayer,
    uint32_t            arrayLayerCount,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState,
    const grfx::Queue*  pSrcQueue,
    const grfx::Queue*  pDstQueue)
{
    PPX_ASSERT_NULL_ARG(pImage);

    if ((!IsNull(pSrcQueue) && IsNull(pDstQueue)) || (IsNull(pSrcQueue) && !IsNull(pDstQueue))) {
        PPX_ASSERT_MSG(false, "queue family transfer requires both pSrcQueue and pDstQueue to be NOT NULL");
    }

    if (beforeState == afterState) {
        return;
    }

    if (mipLevelCount == PPX_REMAINING_MIP_LEVELS) {
        mipLevelCount = pImage->GetMipLevelCount();
    }

    if (arrayLayerCount == PPX_REMAINING_ARRAY_LAYERS) {
        arrayLayerCount = pImage->GetArrayLayerCount();
    }

    const vk::Image* pApiImage = ToApi(pImage);

    VkPipelineStageFlags srcStageMask        = InvalidValue<VkPipelineStageFlags>();
    VkPipelineStageFlags dstStageMask        = InvalidValue<VkPipelineStageFlags>();
    VkAccessFlags        srcAccessMask       = InvalidValue<VkAccessFlags>();
    VkAccessFlags        dstAccessMask       = InvalidValue<VkAccessFlags>();
    VkImageLayout        oldLayout           = InvalidValue<VkImageLayout>();
    VkImageLayout        newLayout           = InvalidValue<VkImageLayout>();
    uint32_t             srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    uint32_t             dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    VkDependencyFlags    dependencyFlags     = 0;

    if (!IsNull(pSrcQueue)) {
        srcQueueFamilyIndex = ToApi(pSrcQueue)->GetQueueFamilyIndex();
    }

    if (!IsNull(pDstQueue)) {
        dstQueueFamilyIndex = ToApi(pDstQueue)->GetQueueFamilyIndex();
    }

    if (srcQueueFamilyIndex == dstQueueFamilyIndex) {
        srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }

    vk::Device* pDevice = ToApi(GetDevice());

    Result ppxres = ToVkBarrierSrc(
        beforeState,
        pDevice->GetDeviceFeatures().geometryShader,
        pDevice->GetDeviceFeatures().tessellationShader,
        srcStageMask,
        srcAccessMask,
        oldLayout);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "couldn't get src barrier data");

    ppxres = ToVkBarrierDst(
        afterState,
        pDevice->GetDeviceFeatures().geometryShader,
        pDevice->GetDeviceFeatures().tessellationShader,
        dstStageMask,
        dstAccessMask,
        newLayout);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "couldn't get dst barrier data");

    VkImageMemoryBarrier barrier            = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.srcAccessMask                   = srcAccessMask;
    barrier.dstAccessMask                   = dstAccessMask;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = srcQueueFamilyIndex;
    barrier.dstQueueFamilyIndex             = dstQueueFamilyIndex;
    barrier.image                           = pApiImage->GetVkImage();
    barrier.subresourceRange.aspectMask     = pApiImage->GetVkImageAspectFlags();
    barrier.subresourceRange.baseMipLevel   = mipLevel;
    barrier.subresourceRange.levelCount     = mipLevelCount;
    barrier.subresourceRange.baseArrayLayer = arrayLayer;
    barrier.subresourceRange.layerCount     = arrayLayerCount;

    vk::CmdPipelineBarrier(
        mCommandBuffer,  // commandBuffer
        srcStageMask,    // srcStageMask
        dstStageMask,    // dstStageMask
        dependencyFlags, // dependencyFlags
        0,               // memoryBarrierCount
        nullptr,         // pMemoryBarriers
        0,               // bufferMemoryBarrierCount
        nullptr,         // pBufferMemoryBarriers
        1,               // imageMemoryBarrierCount
        &barrier);       // pImageMemoryBarriers);
}

void CommandBuffer::BufferResourceBarrier(
    const grfx::Buffer* pBuffer,
    grfx::ResourceState beforeState,
    grfx::ResourceState afterState,
    const grfx::Queue*  pSrcQueue,
    const grfx::Queue*  pDstQueue)
{
    PPX_ASSERT_NULL_ARG(pBuffer);

    if ((!IsNull(pSrcQueue) && IsNull(pDstQueue)) || (IsNull(pSrcQueue) && !IsNull(pDstQueue))) {
        PPX_ASSERT_MSG(false, "queue family transfer requires both pSrcQueue and pDstQueue to be NOT NULL");
    }

    if (beforeState == afterState) {
        return;
    }

    VkPipelineStageFlags srcStageMask        = InvalidValue<VkPipelineStageFlags>();
    VkPipelineStageFlags dstStageMask        = InvalidValue<VkPipelineStageFlags>();
    VkAccessFlags        srcAccessMask       = InvalidValue<VkAccessFlags>();
    VkAccessFlags        dstAccessMask       = InvalidValue<VkAccessFlags>();
    VkImageLayout        oldLayout           = InvalidValue<VkImageLayout>();
    VkImageLayout        newLayout           = InvalidValue<VkImageLayout>();
    uint32_t             srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    uint32_t             dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    VkDependencyFlags    dependencyFlags     = 0;

    if (!IsNull(pSrcQueue)) {
        srcQueueFamilyIndex = ToApi(pSrcQueue)->GetQueueFamilyIndex();
    }

    if (!IsNull(pDstQueue)) {
        dstQueueFamilyIndex = ToApi(pDstQueue)->GetQueueFamilyIndex();
    }

    if (srcQueueFamilyIndex == dstQueueFamilyIndex) {
        srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }

    vk::Device* pDevice = ToApi(GetDevice());

    Result ppxres = ToVkBarrierSrc(
        beforeState,
        pDevice->GetDeviceFeatures().geometryShader,
        pDevice->GetDeviceFeatures().tessellationShader,
        srcStageMask,
        srcAccessMask,
        oldLayout);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "couldn't get src barrier data");

    ppxres = ToVkBarrierDst(
        afterState,
        pDevice->GetDeviceFeatures().geometryShader,
        pDevice->GetDeviceFeatures().tessellationShader,
        dstStageMask,
        dstAccessMask,
        newLayout);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "couldn't get dst barrier data");

    VkBufferMemoryBarrier barrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
    barrier.srcAccessMask         = srcAccessMask;
    barrier.dstAccessMask         = dstAccessMask;
    barrier.srcQueueFamilyIndex   = srcQueueFamilyIndex;
    barrier.dstQueueFamilyIndex   = dstQueueFamilyIndex;
    barrier.buffer                = ToApi(pBuffer)->GetVkBuffer();
    barrier.offset                = static_cast<VkDeviceSize>(0);
    barrier.size                  = static_cast<VkDeviceSize>(pBuffer->GetSize());

    vkCmdPipelineBarrier(
        mCommandBuffer,  // commandBuffer
        srcStageMask,    // srcStageMask
        dstStageMask,    // dstStageMask
        dependencyFlags, // dependencyFlags
        0,               // memoryBarrierCount
        nullptr,         // pMemoryBarriers
        1,               // bufferMemoryBarrierCount
        &barrier,        // pBufferMemoryBarriers
        0,               // imageMemoryBarrierCount
        nullptr);        // pImageMemoryBarriers);
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

void CommandBuffer::BindDescriptorSets(
    VkPipelineBindPoint               bindPoint,
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    PPX_ASSERT_NULL_ARG(pInterface);

    // D3D12 needs the pipeline interface (root signature) bound even if there
    // aren't any descriptor sets. Since Vulkan doesn't require this, we'll
    // just treat it as a NOOP if setCount is zero.
    //
    if (setCount == 0) {
        return;
    }

    // Get set numbers
    const std::vector<uint32_t>& setNumbers = pInterface->GetSetNumbers();

    // setCount cannot exceed the number of sets in the pipeline interface
    uint32_t setNumberCount = CountU32(setNumbers);
    if (setCount > setNumberCount) {
        PPX_ASSERT_MSG(false, "setCount exceeds the number of sets in pipeline interface");
    }

    if (setCount > 0) {
        // Get Vulkan handles
        VkDescriptorSet vkSets[PPX_MAX_BOUND_DESCRIPTOR_SETS] = {VK_NULL_HANDLE};
        for (uint32_t i = 0; i < setCount; ++i) {
            vkSets[i] = ToApi(ppSets[i])->GetVkDescriptorSet();
        }

        // If we have consecutive set numbers we can bind just once...
        if (pInterface->HasConsecutiveSetNumbers()) {
            uint32_t firstSet = setNumbers[0];

            vk::CmdBindDescriptorSets(
                mCommandBuffer,                           // commandBuffer
                bindPoint,                                // pipelineBindPoint
                ToApi(pInterface)->GetVkPipelineLayout(), // layout
                firstSet,                                 // firstSet
                setCount,                                 // descriptorSetCount
                vkSets,                                   // pDescriptorSets
                0,                                        // dynamicOffsetCount
                nullptr);                                 // pDynamicOffsets
        }
        // ...otherwise we get to bind a bunch of times
        else {
            for (uint32_t i = 0; i < setCount; ++i) {
                uint32_t firstSet = setNumbers[i];

                vk::CmdBindDescriptorSets(
                    mCommandBuffer,                           // commandBuffer
                    bindPoint,                                // pipelineBindPoint
                    ToApi(pInterface)->GetVkPipelineLayout(), // layout
                    firstSet,                                 // firstSet
                    1,                                        // descriptorSetCount
                    &vkSets[i],                               // pDescriptorSets
                    0,                                        // dynamicOffsetCount
                    nullptr);                                 // pDynamicOffsets
            }
        }
    }
    else {
        vk::CmdBindDescriptorSets(
            mCommandBuffer,                           // commandBuffer
            bindPoint,                                // pipelineBindPoint
            ToApi(pInterface)->GetVkPipelineLayout(), // layout
            0,                                        // firstSet
            0,                                        // descriptorSetCount
            nullptr,                                  // pDescriptorSets
            0,                                        // dynamicOffsetCount
            nullptr);                                 // pDynamicOffsets
    }
}

void CommandBuffer::BindGraphicsDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pInterface, setCount, ppSets);
}

void CommandBuffer::BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline)
{
    PPX_ASSERT_NULL_ARG(pPipeline);

    vk::CmdBindPipeline(
        mCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        ToApi(pPipeline)->GetVkPipeline());
}

void CommandBuffer::BindComputeDescriptorSets(
    const grfx::PipelineInterface*    pInterface,
    uint32_t                          setCount,
    const grfx::DescriptorSet* const* ppSets)
{
    BindDescriptorSets(VK_PIPELINE_BIND_POINT_COMPUTE, pInterface, setCount, ppSets);
}

void CommandBuffer::BindComputePipeline(const grfx::ComputePipeline* pPipeline)
{
    PPX_ASSERT_NULL_ARG(pPipeline);

    vk::CmdBindPipeline(
        mCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        ToApi(pPipeline)->GetVkPipeline());
}

void CommandBuffer::BindIndexBuffer(const grfx::IndexBufferView* pView)
{
    PPX_ASSERT_NULL_ARG(pView);
    PPX_ASSERT_NULL_ARG(pView->pBuffer);

    vk::CmdBindIndexBuffer(
        mCommandBuffer,
        ToApi(pView->pBuffer)->GetVkBuffer(),
        static_cast<VkDeviceSize>(pView->offset),
        ToVkIndexType(pView->indexType));
}

void CommandBuffer::BindVertexBuffers(uint32_t viewCount, const grfx::VertexBufferView* pViews)
{
    PPX_ASSERT_NULL_ARG(pViews);
    PPX_ASSERT_MSG(viewCount < PPX_MAX_VERTEX_BINDINGS, "viewCount exceeds PPX_MAX_VERTEX_ATTRIBUTES");

    VkBuffer     buffers[PPX_MAX_RENDER_TARGETS] = {VK_NULL_HANDLE};
    VkDeviceSize offsets[PPX_MAX_RENDER_TARGETS] = {0};

    for (uint32_t i = 0; i < viewCount; ++i) {
        buffers[i] = ToApi(pViews[i].pBuffer)->GetVkBuffer();
        offsets[i] = static_cast<VkDeviceSize>(pViews[i].offset);
    }

    vk::CmdBindVertexBuffers(
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
    vk::CmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::Dispatch(
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    vk::CmdDispatch(mCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::CopyBufferToBuffer(
    const grfx::BufferToBufferCopyInfo* pCopyInfo,
    grfx::Buffer*                       pSrcBuffer,
    grfx::Buffer*                       pDstBuffer)
{
    VkBufferCopy region = {};
    region.srcOffset    = static_cast<VkDeviceSize>(pCopyInfo->srcBuffer.offset);
    region.dstOffset    = static_cast<VkDeviceSize>(pCopyInfo->dstBuffer.offset);
    region.size         = static_cast<VkDeviceSize>(pCopyInfo->size);

    vkCmdCopyBuffer(
        mCommandBuffer,
        ToApi(pSrcBuffer)->GetVkBuffer(),
        ToApi(pDstBuffer)->GetVkBuffer(),
        1,
        &region);
}

void CommandBuffer::CopyBufferToImage(
    const grfx::BufferToImageCopyInfo* pCopyInfo,
    grfx::Buffer*                      pSrcBuffer,
    grfx::Image*                       pDstImage)
{
    PPX_ASSERT_NULL_ARG(pCopyInfo);
    PPX_ASSERT_NULL_ARG(pSrcBuffer);
    PPX_ASSERT_NULL_ARG(pDstImage);

    VkBufferImageCopy region               = {};
    region.bufferOffset                    = static_cast<VkDeviceSize>(pCopyInfo->srcBuffer.footprintOffset);
    region.bufferRowLength                 = pCopyInfo->srcBuffer.imageWidth;
    region.bufferImageHeight               = pCopyInfo->srcBuffer.imageHeight;
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

void CommandBuffer::CopyImageToBuffer(
    const grfx::ImageToBufferCopyInfo* pCopyInfo,
    grfx::Image*                       pSrcImage,
    grfx::Buffer*                      pDstBuffer)
{
    PPX_ASSERT_MSG(false, "not implemented");
}

void CommandBuffer::BeginQuery(
    const grfx::Query* pQuery,
    uint32_t           queryIndex)
{
    PPX_ASSERT_NULL_ARG(pQuery);
    PPX_ASSERT_MSG(queryIndex <= pQuery->GetCount(), "invalid query index");

    VkQueryControlFlags flags = 0;
    if (pQuery->GetType() == grfx::QUERY_TYPE_OCCLUSION) {
        flags = VK_QUERY_CONTROL_PRECISE_BIT;
    }

    vkCmdBeginQuery(
        mCommandBuffer,
        ToApi(pQuery)->GetVkQueryPool(),
        queryIndex,
        flags);
}

void CommandBuffer::EndQuery(
    const grfx::Query* pQuery,
    uint32_t           queryIndex)
{
    PPX_ASSERT_NULL_ARG(pQuery);
    PPX_ASSERT_MSG(queryIndex <= pQuery->GetCount(), "invalid query index");

    vkCmdEndQuery(
        mCommandBuffer,
        ToApi(pQuery)->GetVkQueryPool(),
        queryIndex);
}

void CommandBuffer::WriteTimestamp(
    const grfx::Query*  pQuery,
    grfx::PipelineStage pipelineStage,
    uint32_t            queryIndex)
{
    PPX_ASSERT_MSG(queryIndex <= pQuery->GetCount(), "invalid query index");
    vkCmdWriteTimestamp(
        mCommandBuffer,
        ToVkPipelineStage(pipelineStage),
        ToApi(pQuery)->GetVkQueryPool(),
        queryIndex);
}

void CommandBuffer::ResolveQueryData(
    grfx::Query* pQuery,
    uint32_t     startIndex,
    uint32_t     numQueries)
{
    PPX_ASSERT_MSG((startIndex + numQueries) <= pQuery->GetCount(), "invalid query index/number");
    const VkQueryResultFlags flags = VK_QUERY_RESULT_WAIT_BIT | VK_QUERY_RESULT_64_BIT;
    vkCmdCopyQueryPoolResults(mCommandBuffer, ToApi(pQuery)->GetVkQueryPool(), startIndex, numQueries, ToApi(pQuery)->GetReadBackBuffer(), 0, ToApi(pQuery)->GetQueryTypeSize(), flags);
}

// -------------------------------------------------------------------------------------------------
// CommandPool
// -------------------------------------------------------------------------------------------------
Result CommandPool::CreateApiObjects(const grfx::CommandPoolCreateInfo* pCreateInfo)
{
    VkCommandPoolCreateInfo vkci = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    vkci.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkci.queueFamilyIndex        = ToApi(pCreateInfo->pQueue)->GetQueueFamilyIndex();

    VkResult vkres = vk::CreateCommandPool(
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
