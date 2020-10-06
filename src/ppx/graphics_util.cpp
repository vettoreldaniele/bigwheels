#include "ppx/graphics_util.h"
#include "ppx/bitmap.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"

namespace ppx {

struct ScopeKeeper
{
    grfx::QueuePtr                      queue;
    std::vector<grfx::BufferPtr>        buffers;
    std::vector<grfx::CommandBufferPtr> commandBuffers;
    std::vector<grfx::ImagePtr>         images;

    ScopeKeeper(const grfx::QueuePtr& queue_)
        : queue(queue_) {}

    ~ScopeKeeper()
    {
        if (!queue) {
            Result ppxres = queue->WaitIdle();
            if (ppxres == ppx::SUCCESS) {
                PPX_ASSERT_MSG(false, "ScopeKeeper queue wait idle failed");
                return;
            }
        }

        for (auto& elem : commandBuffers) {
            queue->DestroyCommandBuffer(elem);
        }

        for (auto& elem : buffers) {
            queue->GetDevice()->DestroyBuffer(elem);
        }

        for (auto& elem : images) {
            queue->GetDevice()->DestroyImage(elem);
        }
    }
};

Result CreateTextureFromFile(grfx::Queue* pQueue, const fs::path& path, grfx::Image** ppImage)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppImage);

    // Load bitmap
    Bitmap bitmap;
    Result ppxres = Bitmap::LoadFile(path, &bitmap);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ScopeKeeper keeper(pQueue);

    // Create staging buffer
    grfx::BufferPtr buffer;
    {
        uint64_t bitmapFootprintSize = bitmap.GetFootprintSize();

        grfx::BufferCreateInfo ci      = {};
        ci.size                        = bitmapFootprintSize;
        ci.usageFlags.bits.transferSrc = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;

        ppxres = pQueue->GetDevice()->CreateBuffer(&ci, &buffer);
        if (Failed(ppxres)) {
            return ppxres;
        }
        keeper.buffers.push_back(buffer);

        // Map and copy to staging buffer
        void* pBufferAddress = nullptr;
        ppxres               = buffer->MapMemory(0, &pBufferAddress);
        if (Failed(ppxres)) {
            return ppxres;
        }
        std::memcpy(pBufferAddress, bitmap.GetData(), bitmapFootprintSize);
        buffer->UnmapMemory();
    }

    // Create image
    grfx::ImagePtr image;
    {
        grfx::ImageCreateInfo ci       = {};
        ci.type                        = grfx::IMAGE_TYPE_2D;
        ci.width                       = bitmap.GetWidth();
        ci.height                      = bitmap.GetHeight();
        ci.depth                       = 1;
        ci.format                      = grfx::FORMAT_R8G8B8A8_UNORM;
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = 1;
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.sampled     = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_GPU_ONLY;

        ppxres = pQueue->GetDevice()->CreateImage(&ci, &image);
        if (Failed(ppxres)) {
            return ppxres;
        }
        keeper.images.push_back(image);
    }

    grfx::CommandBufferPtr cmdBuf;
    ppxres = pQueue->CreateCommandBuffer(&cmdBuf);
    if (Failed(ppxres)) {
        return ppxres;
    }
    keeper.commandBuffers.push_back(cmdBuf);

    ppxres = cmdBuf->Begin();
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Build command
    {
        cmdBuf->TransitionImageLayout(image, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_UNDEFINED, grfx::RESOURCE_STATE_COPY_DST);

        grfx::BufferToImageCopyInfo copyInfo = {};
        copyInfo.srcBuffer.offset            = 0;
        copyInfo.srcBuffer.footprintWidth    = bitmap.GetWidth();
        copyInfo.srcBuffer.footprintHeight   = bitmap.GetHeight();
        copyInfo.dstImage.mipLevel           = 0;
        copyInfo.dstImage.arrayLayer         = 0;
        copyInfo.dstImage.arrayLayerCount    = 1;
        copyInfo.dstImage.x                  = 0;
        copyInfo.dstImage.y                  = 0;
        copyInfo.dstImage.z                  = 0;
        copyInfo.dstImage.width              = bitmap.GetWidth();
        copyInfo.dstImage.height             = bitmap.GetHeight();
        copyInfo.dstImage.depth              = 1;

        cmdBuf->CopyBufferToImage(&copyInfo, buffer, image);

        cmdBuf->TransitionImageLayout(image, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    ppxres = cmdBuf->End();
    if (Failed(ppxres)) {
        return ppxres;
    }

    grfx::SubmitInfo submit;
    submit.commandBufferCount = 1;
    submit.ppCommandBuffers   = &cmdBuf;

    ppxres = pQueue->Submit(&submit);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = pQueue->WaitIdle();
    if (Failed(ppxres)) {
        return ppxres;
    }

    keeper.images.clear();
    *ppImage = image;

    return ppx::SUCCESS;
}

} // namespace ppx
