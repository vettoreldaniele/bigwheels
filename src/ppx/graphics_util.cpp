#include "ppx/graphics_util.h"
#include "ppx/bitmap.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_scope.h"

namespace ppx {

Result CreateTextureFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    grfx::Image**                ppImage,
    const grfx::ImageUsageFlags& additionalImageUsage)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppImage);

    // Load bitmap
    Bitmap bitmap;
    Result ppxres = Bitmap::LoadFile(path, &bitmap);
    if (Failed(ppxres)) {
        return ppxres;
    }

    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        uint64_t bitmapFootprintSize = bitmap.GetFootprintSize();

        grfx::BufferCreateInfo ci      = {};
        ci.size                        = bitmapFootprintSize;
        ci.usageFlags.bits.transferSrc = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;

        ppxres = pQueue->GetDevice()->CreateBuffer(&ci, &stagingBuffer);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(stagingBuffer);

        // Map and copy to staging buffer
        void* pBufferAddress = nullptr;
        ppxres               = stagingBuffer->MapMemory(0, &pBufferAddress);
        if (Failed(ppxres)) {
            return ppxres;
        }
        std::memcpy(pBufferAddress, bitmap.GetData(), bitmapFootprintSize);
        stagingBuffer->UnmapMemory();
    }

    // Create target image
    grfx::ImagePtr targetImage;
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

        ci.usageFlags.flags |= additionalImageUsage.flags;

        ppxres = pQueue->GetDevice()->CreateImage(&ci, &targetImage);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetImage);
    }

    // Copy info
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

    // Copy to GPU image
    ppxres = pQueue->CopyBufferToImage(
        &copyInfo,
        stagingBuffer,
        targetImage,
        PPX_ALL_SUBRESOURCES,
        grfx::RESOURCE_STATE_UNDEFINED,
        grfx::RESOURCE_STATE_SHADER_RESOURCE);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Change ownership to reference so object doesn't get destroyed
    targetImage->SetOwnership(grfx::OWNERSHIP_REFERENCE);

    // Assign output
    *ppImage = targetImage;

    return ppx::SUCCESS;
}

Result CreateModelFromGeometry(
    grfx::Queue*    pQueue,
    const Geometry* pGeometry,
    grfx::Model**   ppModel)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pGeometry);
    PPX_ASSERT_NULL_ARG(ppModel);

    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        uint32_t biggestBufferSize = pGeometry->GetBiggestBufferSize();

        grfx::BufferCreateInfo ci      = {};
        ci.size                        = biggestBufferSize;
        ci.usageFlags.bits.transferSrc = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;

        Result ppxres = pQueue->GetDevice()->CreateBuffer(&ci, &stagingBuffer);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(stagingBuffer);
    }

    // Create target model
    grfx::ModelPtr targetModel;
    {
        grfx::ModelCreateInfo ci = grfx::ModelCreateInfo(*pGeometry);

        Result ppxres = pQueue->GetDevice()->CreateModel(&ci, &targetModel);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetModel);
    }

    // Copy geometry data to model
    {
        // Copy info
        grfx::BufferToBufferCopyInfo copyInfo = {};

        // Index buffer
        if (pGeometry->GetIndexType() != grfx::INDEX_TYPE_UNDEFINED) {
            const Geometry::Buffer* pGeoBuffer = pGeometry->GetIndexBuffer();
            PPX_ASSERT_NULL_ARG(pGeoBuffer);

            uint32_t geoBufferSize = pGeoBuffer->GetSize();

            Result ppxres = stagingBuffer->CopyFromSource(geoBufferSize, pGeoBuffer->GetData());
            if (Failed(ppxres)) {
                return ppxres;
            }

            copyInfo.size = geoBufferSize;

            // Copy to GPU buffer
            ppxres = pQueue->CopyBufferToBuffer(&copyInfo, stagingBuffer, targetModel->GetIndexBuffer());
            if (Failed(ppxres)) {
                return ppxres;
            }
        }

        // Vertex buffers
        uint32_t vertexBufferCount = pGeometry->GetVertexBufferCount();
        for (uint32_t i = 0; i < vertexBufferCount; ++i) {
            const Geometry::Buffer* pGeoBuffer = pGeometry->GetVertxBuffer(i);
            PPX_ASSERT_NULL_ARG(pGeoBuffer);

            uint32_t geoBufferSize = pGeoBuffer->GetSize();

            Result ppxres = stagingBuffer->CopyFromSource(geoBufferSize, pGeoBuffer->GetData());
            if (Failed(ppxres)) {
                return ppxres;
            }

            copyInfo.size = geoBufferSize;

            grfx::BufferPtr targetBuffer = targetModel->GetVertexBuffer(i);

            // Copy to GPU buffer
            ppxres = pQueue->CopyBufferToBuffer(&copyInfo, stagingBuffer, targetBuffer);
            if (Failed(ppxres)) {
                return ppxres;
            }
        }
    }

    // Change ownership to reference so object doesn't get destroyed
    targetModel->SetOwnership(grfx::OWNERSHIP_REFERENCE);

    // Assign output
    *ppModel = targetModel;

    return ppx::SUCCESS;
}

} // namespace ppx
