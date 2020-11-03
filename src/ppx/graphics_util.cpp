#include "ppx/graphics_util.h"
#include "ppx/bitmap.h"
#include "ppx/timer.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_scope.h"

namespace ppx {

grfx::Format ToGrfxFormat(Bitmap::Format value)
{
    // clang-format off
    switch (value) {
        default: break;
        case Bitmap::FORMAT_R_UINT8     : return grfx::FORMAT_R8_UNORM; break;
        case Bitmap::FORMAT_RG_UINT8    : return grfx::FORMAT_R8G8_UNORM; break;
        case Bitmap::FORMAT_RGB_UINT8   : return grfx::FORMAT_R8G8B8_UNORM; break;
        case Bitmap::FORMAT_RGBA_UINT8  : return grfx::FORMAT_R8G8B8A8_UNORM; break;
        case Bitmap::FORMAT_R_UINT16    : return grfx::FORMAT_R16_UNORM; break;
        case Bitmap::FORMAT_RG_UINT16   : return grfx::FORMAT_R16G16_UNORM; break;
        case Bitmap::FORMAT_RGB_UINT16  : return grfx::FORMAT_R16G16B16_UNORM; break;
        case Bitmap::FORMAT_RGBA_UINT16 : return grfx::FORMAT_R16G16B16A16_UNORM; break;
        //case Bitmap::FORMAT_R_UINT32    : return grfx::FORMAT_R32_UNORM; break;
        //case Bitmap::FORMAT_RG_UINT32   : return grfx::FORMAT_R32G32_UNORM; break;
        //case Bitmap::FORMAT_RGB_UINT32  : return grfx::FORMAT_R32G32B32_UNORM; break;
        //case Bitmap::FORMAT_RGBA_UINT32 : return grfx::FORMAT_R32G32B32A32_UNORM; break;
        case Bitmap::FORMAT_R_FLOAT     : return grfx::FORMAT_R32_FLOAT; break;
        case Bitmap::FORMAT_RG_FLOAT    : return grfx::FORMAT_R32G32_FLOAT; break;
        case Bitmap::FORMAT_RGB_FLOAT   : return grfx::FORMAT_R32G32B32_FLOAT; break;
        case Bitmap::FORMAT_RGBA_FLOAT  : return grfx::FORMAT_R32G32B32A32_FLOAT; break;
    }
    // clang-format on
    return grfx::FORMAT_UNDEFINED;
}

Result CreateTextureFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    grfx::Image**                ppImage,
    const grfx::ImageUsageFlags& additionalImageUsage)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppImage);

    Timer timer;
    PPX_ASSERT_MSG(timer.Start() == ppx::TIMER_RESULT_SUCCESS, "timer start failed");
    double fnStartTime = timer.SecondsSinceStart(); 

    // Load bitmap
    Bitmap bitmap;
    Result ppxres = Bitmap::LoadFile(path, &bitmap);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Row stride alignment to handle DX's requirement
    uint32_t rowStrideAlignement = grfx::IsDx(pQueue->GetDevice()->GetApi()) ? PPX_D3D12_TEXTURE_DATA_PITCH_ALIGNMENT : 1;
    uint32_t alignedRowStride    = RoundUp<uint32_t>(bitmap.GetRowStride(), rowStrideAlignement);

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        uint64_t bitmapFootprintSize = bitmap.GetFootprintSize(rowStrideAlignement);

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

        const char*    pSrc         = bitmap.GetData();
        char*          pDst         = static_cast<char*>(pBufferAddress);
        const uint32_t srcRowStride = bitmap.GetRowStride();
        const uint32_t dstRowStride = alignedRowStride;
        for (uint32_t y = 0; y < bitmap.GetHeight(); ++y) {
            memcpy(pDst, pSrc, srcRowStride);
            pSrc += srcRowStride;
            pDst += dstRowStride;
        }

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
        ci.format                      = ToGrfxFormat(bitmap.GetFormat());
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
    copyInfo.srcBuffer.imageWidth        = bitmap.GetWidth();
    copyInfo.srcBuffer.imageHeight       = bitmap.GetHeight();
    copyInfo.srcBuffer.imageRowStride    = rowStrideAlignement;
    copyInfo.srcBuffer.footprintOffset   = 0;
    copyInfo.srcBuffer.footprintWidth    = bitmap.GetWidth();
    copyInfo.srcBuffer.footprintHeight   = bitmap.GetHeight();
    copyInfo.srcBuffer.footprintDepth    = 1;
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

    double fnEndTime = timer.SecondsSinceStart();
    float  fnElapsed = static_cast<float>(fnEndTime - fnStartTime);
    PPX_LOG_INFO("Created image from image file: " << path << " (" << FloatString(fnElapsed) << " seconds)");

    return ppx::SUCCESS;
}

Result CreateTextureFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    grfx::Texture**              ppTexture,
    const grfx::ImageUsageFlags& additionalImageUsage)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppTexture);

    Timer timer;
    PPX_ASSERT_MSG(timer.Start() == ppx::TIMER_RESULT_SUCCESS, "timer start failed");
    double fnStartTime = timer.SecondsSinceStart();

    // Load bitmap
    Bitmap bitmap;
    Result ppxres = Bitmap::LoadFile(path, &bitmap);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Row stride alignment to handle DX's requirement
    uint32_t rowStrideAlignement = grfx::IsDx(pQueue->GetDevice()->GetApi()) ? PPX_D3D12_TEXTURE_DATA_PITCH_ALIGNMENT : 1;
    uint32_t alignedRowStride    = RoundUp<uint32_t>(bitmap.GetRowStride(), rowStrideAlignement);

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        uint64_t bitmapFootprintSize = bitmap.GetFootprintSize(rowStrideAlignement);

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

        const char*    pSrc         = bitmap.GetData();
        char*          pDst         = static_cast<char*>(pBufferAddress);
        const uint32_t srcRowStride = bitmap.GetRowStride();
        const uint32_t dstRowStride = alignedRowStride;
        for (uint32_t y = 0; y < bitmap.GetHeight(); ++y) {
            memcpy(pDst, pSrc, srcRowStride);
            pSrc += srcRowStride;
            pDst += dstRowStride;
        }

        stagingBuffer->UnmapMemory();
    }

    // Create target texture
    grfx::TexturePtr targetTexture;
    {
        grfx::TextureCreateInfo ci     = {};
        ci.pImage                      = nullptr;
        ci.imageType                   = grfx::IMAGE_TYPE_2D;
        ci.width                       = bitmap.GetWidth();
        ci.height                      = bitmap.GetHeight();
        ci.depth                       = 1;
        ci.imageFormat                 = ToGrfxFormat(bitmap.GetFormat());
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = 1;
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.sampled     = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_GPU_ONLY;
        ci.initialState                = grfx::RESOURCE_STATE_GENERAL;
        ci.RTVClearValue               = {0, 0, 0, 0};
        ci.DSVClearValue               = {1.0f, 0xFF};
        ci.sampledImageViewType        = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
        ci.sampledImageViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.renderTargetViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.depthStencilViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.storageImageViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.ownership                   = grfx::OWNERSHIP_REFERENCE;

        ci.usageFlags.flags |= additionalImageUsage.flags;

        ppxres = pQueue->GetDevice()->CreateTexture(&ci, &targetTexture);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetTexture);
    }

    // Copy info
    grfx::BufferToImageCopyInfo copyInfo = {};
    copyInfo.srcBuffer.imageWidth        = bitmap.GetWidth();
    copyInfo.srcBuffer.imageHeight       = bitmap.GetHeight();
    copyInfo.srcBuffer.imageRowStride    = alignedRowStride;
    copyInfo.srcBuffer.footprintOffset   = 0;
    copyInfo.srcBuffer.footprintWidth    = bitmap.GetWidth();
    copyInfo.srcBuffer.footprintHeight   = bitmap.GetHeight();
    copyInfo.srcBuffer.footprintDepth    = 1;
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
        targetTexture->GetImage(),
        PPX_ALL_SUBRESOURCES,
        grfx::RESOURCE_STATE_UNDEFINED,
        grfx::RESOURCE_STATE_SHADER_RESOURCE);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Change ownership to reference so object doesn't get destroyed
    targetTexture->SetOwnership(grfx::OWNERSHIP_REFERENCE);

    // Assign output
    *ppTexture = targetTexture;

    double fnEndTime = timer.SecondsSinceStart();
    float  fnElapsed = static_cast<float>(fnEndTime - fnStartTime);
    PPX_LOG_INFO("Created texture from image file: " << path << " (" << FloatString(fnElapsed) << " seconds)");

    return ppx::SUCCESS;
}

Result CreateTexture1x1(
    grfx::Queue*                 pQueue,
    const float4&                color,
    grfx::Texture**              ppTexture,
    const grfx::ImageUsageFlags& additionalImageUsage)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppTexture);

    Result ppxres = ppx::SUCCESS;

    // Create bitmap
    Bitmap bitmap = Bitmap(1, 1, Bitmap::FORMAT_RGBA_UINT8);
    bitmap.Fill(color.r, color.g, color.b, color.a);

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Row stride alignment to handle DX's requirement
    uint32_t rowStrideAlignement = grfx::IsDx(pQueue->GetDevice()->GetApi()) ? PPX_D3D12_TEXTURE_DATA_PITCH_ALIGNMENT : 1;
    uint32_t alignedRowStride    = RoundUp<uint32_t>(bitmap.GetRowStride(), rowStrideAlignement);

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        uint64_t bitmapFootprintSize = bitmap.GetFootprintSize(rowStrideAlignement);

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

        const char*    pSrc         = bitmap.GetData();
        char*          pDst         = static_cast<char*>(pBufferAddress);
        const uint32_t srcRowStride = bitmap.GetRowStride();
        const uint32_t dstRowStride = alignedRowStride;
        for (uint32_t y = 0; y < bitmap.GetHeight(); ++y) {
            memcpy(pDst, pSrc, srcRowStride);
            pSrc += srcRowStride;
            pDst += dstRowStride;
        }

        stagingBuffer->UnmapMemory();
    }

    // Create target texture
    grfx::TexturePtr targetTexture;
    {
        grfx::TextureCreateInfo ci     = {};
        ci.pImage                      = nullptr;
        ci.imageType                   = grfx::IMAGE_TYPE_2D;
        ci.width                       = bitmap.GetWidth();
        ci.height                      = bitmap.GetHeight();
        ci.depth                       = 1;
        ci.imageFormat                 = ToGrfxFormat(bitmap.GetFormat());
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = 1;
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.sampled     = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_GPU_ONLY;
        ci.initialState                = grfx::RESOURCE_STATE_GENERAL;
        ci.RTVClearValue               = {0, 0, 0, 0};
        ci.DSVClearValue               = {1.0f, 0xFF};
        ci.sampledImageViewType        = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
        ci.sampledImageViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.renderTargetViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.depthStencilViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.storageImageViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.ownership                   = grfx::OWNERSHIP_REFERENCE;

        ci.usageFlags.flags |= additionalImageUsage.flags;

        ppxres = pQueue->GetDevice()->CreateTexture(&ci, &targetTexture);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetTexture);
    }

    // Copy info
    grfx::BufferToImageCopyInfo copyInfo = {};
    copyInfo.srcBuffer.imageWidth        = bitmap.GetWidth();
    copyInfo.srcBuffer.imageHeight       = bitmap.GetHeight();
    copyInfo.srcBuffer.imageRowStride    = alignedRowStride;
    copyInfo.srcBuffer.footprintOffset   = 0;
    copyInfo.srcBuffer.footprintWidth    = bitmap.GetWidth();
    copyInfo.srcBuffer.footprintHeight   = bitmap.GetHeight();
    copyInfo.srcBuffer.footprintDepth    = 1;
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
        targetTexture->GetImage(),
        PPX_ALL_SUBRESOURCES,
        grfx::RESOURCE_STATE_UNDEFINED,
        grfx::RESOURCE_STATE_SHADER_RESOURCE);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Change ownership to reference so object doesn't get destroyed
    targetTexture->SetOwnership(grfx::OWNERSHIP_REFERENCE);

    // Assign output
    *ppTexture = targetTexture;

    return ppx::SUCCESS;
}

struct SubImage
{
    uint32_t width        = 0;
    uint32_t height       = 0;
    uint32_t bufferOffset = 0;
};

SubImage CalcSubimageCrossHorizontalLeft(uint32_t subImageIndex, uint32_t imageWidth, uint32_t imageHeight, grfx::Format format)
{
    uint32_t cellPixelsX = imageWidth / 4;
    uint32_t cellPixelsY = imageHeight / 3;
    uint32_t cellX       = 0;
    uint32_t cellY       = 0;
    switch (subImageIndex) {
        default: break;

        case 0: {
            cellX = 1;
            cellY = 0;
        } break;

        case 1: {
            cellX = 0;
            cellY = 1;
        } break;

        case 2: {
            cellX = 1;
            cellY = 1;
        } break;

        case 3: {
            cellX = 2;
            cellY = 1;
        } break;

        case 4: {
            cellX = 3;
            cellY = 1;
        } break;

        case 5: {
            cellX = 1;
            cellY = 2;

        } break;
    }

    uint32_t pixelStride  = grfx::FormatSize(format);
    uint32_t pixelOffsetX = cellX * cellPixelsX * pixelStride;
    uint32_t pixelOffsetY = cellY * cellPixelsY * imageWidth * pixelStride;

    SubImage subImage     = {};
    subImage.width        = cellPixelsX;
    subImage.height       = cellPixelsY;
    subImage.bufferOffset = pixelOffsetX + pixelOffsetY;

    return subImage;
}

Result CreateCubeMapFromFile(
    grfx::Queue*                  pQueue,
    const fs::path&               path,
    const ppx::CubeMapCreateInfo* pCreateInfo,
    grfx::Image**                 ppImage,
    const grfx::ImageUsageFlags&  additionalImageUsage)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppImage);

    // Load bitmap
    Bitmap bitmap;
    Result ppxres = Bitmap::LoadFile(path, &bitmap);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Scoped destroy
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

    // Target format
    grfx::Format targetFormat = grfx::FORMAT_R8G8B8A8_UNORM;

    // Calculate subImage to use for target image dimensions
    SubImage tmpSubImage = CalcSubimageCrossHorizontalLeft(0, bitmap.GetWidth(), bitmap.GetHeight(), targetFormat);

    // Create target image
    grfx::ImagePtr targetImage;
    {
        grfx::ImageCreateInfo ci       = {};
        ci.type                        = grfx::IMAGE_TYPE_CUBE;
        ci.width                       = tmpSubImage.width;
        ci.height                      = tmpSubImage.height;
        ci.depth                       = 1;
        ci.format                      = targetFormat;
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = 1;
        ci.arrayLayerCount             = 6;
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

    // Copy to GPU image
    //
    // @TODO: pack copies
    //
    {
        uint32_t faces[6] = {
            pCreateInfo->posX,
            pCreateInfo->negX,
            pCreateInfo->posY,
            pCreateInfo->negY,
            pCreateInfo->posZ,
            pCreateInfo->negZ,
        };

        // Vulkan doesn't seem to mind transtioning a resource from
        // UNDEFINED to SHADER_READ but D3D12 does...so we'll save
        // the before state after the first transition so it matches.
        //
        grfx::ResourceState beforeState = grfx::RESOURCE_STATE_UNDEFINED;
        for (uint32_t arrayLayer = 0; arrayLayer < 6; ++arrayLayer) {
            uint32_t subImageIndex = faces[arrayLayer];
            SubImage subImage      = CalcSubimageCrossHorizontalLeft(subImageIndex, bitmap.GetWidth(), bitmap.GetHeight(), targetFormat);

            // Copy info
            grfx::BufferToImageCopyInfo copyInfo = {};
            copyInfo.srcBuffer.imageWidth        = bitmap.GetWidth();
            copyInfo.srcBuffer.imageHeight       = bitmap.GetHeight();
            copyInfo.srcBuffer.imageRowStride    = bitmap.GetRowStride();
            copyInfo.srcBuffer.footprintOffset   = subImage.bufferOffset;
            copyInfo.srcBuffer.footprintWidth    = subImage.width;
            copyInfo.srcBuffer.footprintHeight   = subImage.height;
            copyInfo.srcBuffer.footprintDepth    = 1;
            copyInfo.dstImage.mipLevel           = 0;
            copyInfo.dstImage.arrayLayer         = arrayLayer;
            copyInfo.dstImage.arrayLayerCount    = 1;
            copyInfo.dstImage.x                  = 0;
            copyInfo.dstImage.y                  = 0;
            copyInfo.dstImage.z                  = 0;
            copyInfo.dstImage.width              = subImage.width;
            copyInfo.dstImage.height             = subImage.width;
            copyInfo.dstImage.depth              = 1;

            ppxres = pQueue->CopyBufferToImage(
                &copyInfo,
                stagingBuffer,
                targetImage,
                PPX_ALL_SUBRESOURCES,
                beforeState,
                grfx::RESOURCE_STATE_SHADER_RESOURCE);
            if (Failed(ppxres)) {
                return ppxres;
            }

            beforeState = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        }
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
