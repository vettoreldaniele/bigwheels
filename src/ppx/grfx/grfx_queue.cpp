#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_scope.h"

namespace ppx {
namespace grfx {

Result Queue::CreateCommandBuffer(
    grfx::CommandBuffer** ppCommandBuffer,
    uint32_t              resourceDescriptorCount,
    uint32_t              samplerDescriptorCount)
{
    std::lock_guard<std::mutex> lock(mCommandSetMutex);

    CommandSet set = {};

    grfx::CommandPoolCreateInfo ci = {};
    ci.pQueue                      = this;

    Result ppxres = GetDevice()->CreateCommandPool(&ci, &set.commandPool);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = GetDevice()->AllocateCommandBuffer(set.commandPool, &set.commandBuffer, resourceDescriptorCount, samplerDescriptorCount);
    if (Failed(ppxres)) {
        GetDevice()->DestroyCommandPool(set.commandPool);
        return ppxres;
    }

    *ppCommandBuffer = set.commandBuffer;

    mCommandSets.push_back(set);

    return ppx::SUCCESS;
}

void Queue::DestroyCommandBuffer(const grfx::CommandBuffer* pCommandBuffer)
{
    std::lock_guard<std::mutex> lock(mCommandSetMutex);

    auto it = std::find_if(
        std::begin(mCommandSets),
        std::end(mCommandSets),
        [pCommandBuffer](const CommandSet& elem) -> bool {
            bool isSame = (elem.commandBuffer.Get() == pCommandBuffer);
            return isSame; });
    if (it == std::end(mCommandSets)) {
        return;
    }

    CommandSet& set = *it;

    GetDevice()->FreeCommandBuffer(set.commandBuffer);
    GetDevice()->DestroyCommandPool(set.commandPool);

    RemoveElementIf(
        mCommandSets,
        [set](const CommandSet& elem) -> bool {
            bool isSamePool = (elem.commandPool == set.commandPool);
            bool isSameBuffer = (elem.commandBuffer == set.commandBuffer); 
            bool isSame = isSamePool && isSameBuffer; 
            return isSame; });
}

Result Queue::CopyBufferToBuffer(
    const grfx::BufferToBufferCopyInfo* pCopyInfo,
    grfx::Buffer*                       pSrcBuffer,
    grfx::Buffer*                       pDstBuffer,
    grfx::ResourceState                 stateBefore,
    grfx::ResourceState                 stateAfter)
{
    grfx::ScopeDestroyer SCOPED_DESTROYER(GetDevice());

    // Create command buffer
    grfx::CommandBufferPtr cmd;
    Result                 ppxres = CreateCommandBuffer(&cmd, 0, 0);
    if (Failed(ppxres)) {
        return ppxres;
    }
    SCOPED_DESTROYER.AddObject(this, cmd);

    // Build command buffer
    {
        ppxres = cmd->Begin();
        if (Failed(ppxres)) {
            return ppxres;
        }

        cmd->BufferResourceBarrier(pDstBuffer, stateBefore, grfx::RESOURCE_STATE_COPY_DST);
        cmd->CopyBufferToBuffer(pCopyInfo, pSrcBuffer, pDstBuffer);
        cmd->BufferResourceBarrier(pDstBuffer, grfx::RESOURCE_STATE_COPY_DST, stateAfter);

        ppxres = cmd->End();
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Submit command buffer
    grfx::SubmitInfo submit;
    submit.commandBufferCount = 1;
    submit.ppCommandBuffers   = &cmd;
    //
    ppxres = Submit(&submit);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Wait work completion
    ppxres = WaitIdle();
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

Result Queue::CopySourceToBuffer(
    const uint32_t      sourceDataSize,
    const void*         pSourceData,
    grfx::Buffer*       pDstBuffer,
    grfx::ResourceState stateBefore,
    grfx::ResourceState stateAfter)
{
    if ((sourceDataSize == 0) || IsNull(pSourceData)) {
        return ppx::ERROR_BAD_DATA_SOURCE;
    }
    if (IsNull(pDstBuffer)) {
        return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
    }

    bool validSourceDataSize = (sourceDataSize <= pDstBuffer->GetSize());
    PPX_ASSERT_MSG(validSourceDataSize, "Source data size exceeds destination buffer size");
    if (!validSourceDataSize) {
        return ppx::ERROR_BAD_DATA_SOURCE;
    }

    grfx::ScopeDestroyer SCOPED_DESTROYER(GetDevice());

    // Create temporary staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        ppx::grfx::BufferCreateInfo createInfo = {};
        createInfo.size                        = sourceDataSize;
        createInfo.usageFlags.bits.transferSrc = true;
        createInfo.memoryUsage                 = ppx::grfx::MEMORY_USAGE_CPU_ONLY;
        Result ppxres                          = GetDevice()->CreateBuffer(&createInfo, &stagingBuffer);
        if (Failed(ppxres)) {
            return ppxres;
        }

        SCOPED_DESTROYER.AddObject(stagingBuffer);
    }

    Result ppxres = stagingBuffer->CopyFromSource(sourceDataSize, pSourceData);
    if (Failed(ppxres)) {
        return ppxres;
    }

    grfx::BufferToBufferCopyInfo copyInfo = {};
    copyInfo.size                         = sourceDataSize;
    copyInfo.srcBuffer.offset             = 0;
    copyInfo.dstBuffer.offset             = 0;

    ppxres = CopyBufferToBuffer(&copyInfo, stagingBuffer, pDstBuffer, stateBefore, stateAfter);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

Result Queue::CopyBufferToImage(
    const grfx::BufferToImageCopyInfo* pCopyInfo,
    grfx::Buffer*                      pSrcBuffer,
    grfx::Image*                       pDstImage,
    uint32_t                           mipLevel,
    uint32_t                           mipLevelCount,
    uint32_t                           arrayLayer,
    uint32_t                           arrayLayerCount,
    grfx::ResourceState                stateBefore,
    grfx::ResourceState                stateAfter)
{
    grfx::ScopeDestroyer SCOPED_DESTROYER(GetDevice());

    // Create command buffer
    grfx::CommandBufferPtr cmd;
    Result                 ppxres = CreateCommandBuffer(&cmd, 0, 0);
    if (Failed(ppxres)) {
        return ppxres;
    }
    SCOPED_DESTROYER.AddObject(this, cmd);

    // Build command buffer
    {
        ppxres = cmd->Begin();
        if (Failed(ppxres)) {
            return ppxres;
        }

        cmd->TransitionImageLayout(pDstImage, PPX_ALL_SUBRESOURCES, stateBefore, grfx::RESOURCE_STATE_COPY_DST);
        cmd->CopyBufferToImage(pCopyInfo, pSrcBuffer, pDstImage);
        cmd->TransitionImageLayout(pDstImage, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_COPY_DST, stateAfter);

        ppxres = cmd->End();
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Submit command buffer
    grfx::SubmitInfo submit;
    submit.commandBufferCount = 1;
    submit.ppCommandBuffers   = &cmd;
    //
    ppxres = Submit(&submit);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Wait work completion
    ppxres = WaitIdle();
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
