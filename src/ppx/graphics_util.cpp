#include "ppx/graphics_util.h"
#include "ppx/bitmap.h"
#include "ppx/mipmap.h"
#include "ppx/timer.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_util.h"
#include "ppx/grfx/grfx_scope.h"
#include "gli/gli.hpp"

namespace ppx {
namespace grfx_util {

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

grfx::Format ToGrfxFormat(gli::format value)
{
    // clang-format off
    switch (value) {
        case gli::FORMAT_RGB_DXT1_UNORM_BLOCK8          : return grfx::FORMAT_BC1_RGB_UNORM;
        case gli::FORMAT_RGB_DXT1_SRGB_BLOCK8           : return grfx::FORMAT_BC1_RGB_SRGB;
        case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8         : return grfx::FORMAT_BC1_RGBA_UNORM;
        case gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8          : return grfx::FORMAT_BC1_RGBA_SRGB;
        case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16        : return grfx::FORMAT_BC2_SRGB;
        case gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16         : return grfx::FORMAT_BC3_SRGB;
        case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16        : return grfx::FORMAT_BC3_UNORM;
        case gli::FORMAT_R_ATI1N_UNORM_BLOCK8           : return grfx::FORMAT_BC4_UNORM;
        case gli::FORMAT_R_ATI1N_SNORM_BLOCK8           : return grfx::FORMAT_BC4_SNORM;
        case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16         : return grfx::FORMAT_BC5_UNORM;
        case gli::FORMAT_RG_ATI2N_SNORM_BLOCK16         : return grfx::FORMAT_BC5_SNORM;
        case gli::FORMAT_RGB_BP_UFLOAT_BLOCK16          : return grfx::FORMAT_BC6H_UFLOAT;
        case gli::FORMAT_RGB_BP_SFLOAT_BLOCK16          : return grfx::FORMAT_BC6H_SFLOAT;
        case gli::FORMAT_RGBA_BP_UNORM_BLOCK16          : return grfx::FORMAT_BC7_UNORM;
        case gli::FORMAT_RGBA_BP_SRGB_BLOCK16           : return grfx::FORMAT_BC7_SRGB;
        default:
            return grfx::FORMAT_UNDEFINED;
    }
    // clang-format on
}

// -------------------------------------------------------------------------------------------------

Result CopyBitmapToImage(
    grfx::Queue*        pQueue,
    const Bitmap*       pBitmap,
    grfx::Image*        pImage,
    uint32_t            mipLevel,
    uint32_t            arrayLayer,
    grfx::ResourceState stateBefore,
    grfx::ResourceState stateAfter)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pBitmap);
    PPX_ASSERT_NULL_ARG(pImage);

    Result ppxres = ppx::ERROR_FAILED;

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Row stride alignment to handle DX's requirement
    uint32_t rowStrideAlignement = grfx::IsDx12(pQueue->GetDevice()->GetApi()) ? PPX_D3D12_TEXTURE_DATA_PITCH_ALIGNMENT : 1;
    uint32_t alignedRowStride    = RoundUp<uint32_t>(pBitmap->GetRowStride(), rowStrideAlignement);

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        uint64_t bitmapFootprintSize = pBitmap->GetFootprintSize(rowStrideAlignement);

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

        const char*    pSrc         = pBitmap->GetData();
        char*          pDst         = static_cast<char*>(pBufferAddress);
        const uint32_t srcRowStride = pBitmap->GetRowStride();
        const uint32_t dstRowStride = alignedRowStride;
        for (uint32_t y = 0; y < pBitmap->GetHeight(); ++y) {
            memcpy(pDst, pSrc, srcRowStride);
            pSrc += srcRowStride;
            pDst += dstRowStride;
        }

        stagingBuffer->UnmapMemory();
    }

    // Copy info
    grfx::BufferToImageCopyInfo copyInfo = {};
    copyInfo.srcBuffer.imageWidth        = pBitmap->GetWidth();
    copyInfo.srcBuffer.imageHeight       = pBitmap->GetHeight();
    copyInfo.srcBuffer.imageRowStride    = alignedRowStride;
    copyInfo.srcBuffer.footprintOffset   = 0;
    copyInfo.srcBuffer.footprintWidth    = pBitmap->GetWidth();
    copyInfo.srcBuffer.footprintHeight   = pBitmap->GetHeight();
    copyInfo.srcBuffer.footprintDepth    = 1;
    copyInfo.dstImage.mipLevel           = mipLevel;
    copyInfo.dstImage.arrayLayer         = arrayLayer;
    copyInfo.dstImage.arrayLayerCount    = 1;
    copyInfo.dstImage.x                  = 0;
    copyInfo.dstImage.y                  = 0;
    copyInfo.dstImage.z                  = 0;
    copyInfo.dstImage.width              = pBitmap->GetWidth();
    copyInfo.dstImage.height             = pBitmap->GetHeight();
    copyInfo.dstImage.depth              = 1;

    // Copy to GPU image
    ppxres = pQueue->CopyBufferToImage(
        &copyInfo,
        stagingBuffer,
        pImage,
        mipLevel,
        1,
        arrayLayer,
        1,
        stateBefore,
        stateAfter);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}
// -------------------------------------------------------------------------------------------------

Result CreateImageFromBitmap(
    grfx::Queue*        pQueue,
    const Bitmap*       pBitmap,
    grfx::Image**       ppImage,
    const ImageOptions& options)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pBitmap);
    PPX_ASSERT_NULL_ARG(ppImage);

    Result ppxres = ppx::ERROR_FAILED;

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Cap mip level count
    uint32_t maxMipLevelCount = Mipmap::CalculateLevelCount(pBitmap->GetWidth(), pBitmap->GetHeight());
    uint32_t mipLevelCount    = std::min<uint32_t>(options.mMipLevelCount, maxMipLevelCount);

    // Create target image
    grfx::ImagePtr targetImage;
    {
        grfx::ImageCreateInfo ci       = {};
        ci.type                        = grfx::IMAGE_TYPE_2D;
        ci.width                       = pBitmap->GetWidth();
        ci.height                      = pBitmap->GetHeight();
        ci.depth                       = 1;
        ci.format                      = ToGrfxFormat(pBitmap->GetFormat());
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = mipLevelCount;
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.sampled     = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_GPU_ONLY;
        ci.initialState                = grfx::RESOURCE_STATE_SHADER_RESOURCE;

        ci.usageFlags.flags |= options.mAdditionalUsage;

        ppxres = pQueue->GetDevice()->CreateImage(&ci, &targetImage);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetImage);
    }

    Mipmap mipmap = Mipmap(*pBitmap, mipLevelCount);
    if (!mipmap.IsOk()) {
        return ppx::ERROR_FAILED;
    }

    // Copy mips to image
    for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel) {
        const Bitmap* pMip = mipmap.GetMip(mipLevel);

        ppxres = CopyBitmapToImage(
            pQueue,
            pMip,
            targetImage,
            mipLevel,
            0,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_SHADER_RESOURCE);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Change ownership to reference so object doesn't get destroyed
    targetImage->SetOwnership(grfx::OWNERSHIP_REFERENCE);

    // Assign output
    *ppImage = targetImage;

    return ppx::SUCCESS;
}

bool IsDDSFile(const fs::path& path)
{
    return (std::strstr(path.c_str(), ".dds") != nullptr || std::strstr(path.c_str(), ".ktx") != nullptr);
}

Result CreateImageFromCompressedImage(
    grfx::Queue*        pQueue,
    const gli::texture& image,
    grfx::Image**       ppImage,
    const ImageOptions& options)
{
    Result ppxres;

    PPX_LOG_INFO("Target type: " << grfx::ToString(image.target()) << "\n");
    PPX_LOG_INFO("Format: " << grfx::ToString(image.format()) << "\n");
    PPX_LOG_INFO("Swizzles: " << image.swizzles()[0] << ", " << image.swizzles()[1] << ", " << image.swizzles()[2] << ", " << image.swizzles()[3] << "\n");
    PPX_LOG_INFO("Layer information:\n"
                 << "\tBase layer: " << image.base_layer() << "\n"
                 << "\tMax layer: " << image.max_layer() << "\n"
                 << "\t# of layers: " << image.layers() << "\n");
    PPX_LOG_INFO("Face information:\n"
                 << "\tBase face: " << image.base_face() << "\n"
                 << "\tMax face: " << image.max_face() << "\n"
                 << "\t# of faces: " << image.faces() << "\n");
    PPX_LOG_INFO("Level information:\n"
                 << "\tBase level: " << image.base_level() << "\n"
                 << "\tMax level: " << image.max_level() << "\n"
                 << "\t# of levels: " << image.levels() << "\n");
    PPX_LOG_INFO("Image extents by level:\n");
    for (gli::texture::size_type level = 0; level < image.levels(); level++) {
        PPX_LOG_INFO("\textent(level == " << level << "): [" << image.extent(level)[0] << ", " << image.extent(level)[1] << ", " << image.extent(level)[2] << "]\n");
    }
    PPX_LOG_INFO("Total image size (bytes): " << image.size() << "\n");
    PPX_LOG_INFO("Image size by level:\n");
    for (gli::texture::size_type i = 0; i < image.levels(); i++) {
        PPX_LOG_INFO("\tsize(level == " << i << "): " << image.size(i) << "\n");
    }
    PPX_LOG_INFO("Image data pointer: " << image.data() << "\n");

    PPX_ASSERT_MSG((image.target() == gli::TARGET_2D), "Expecting a 2D DDS image.");

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    grfx::Format format      = ToGrfxFormat(image.format());
    uint32_t     imageWidth  = static_cast<uint32_t>(image.extent(0)[0]);
    uint32_t     imageHeight = static_cast<uint32_t>(image.extent(0)[1]);
    uint32_t     rowStride   = imageWidth * grfx::FormatSize(format);

    // Row stride alignment to handle DX's requirement
    uint32_t rowStrideAlignement = grfx::IsDx12(pQueue->GetDevice()->GetApi()) ? PPX_D3D12_TEXTURE_DATA_PITCH_ALIGNMENT : 1;
    rowStride                    = RoundUp<uint32_t>(rowStride, rowStrideAlignement);

    // Create staging buffer
    grfx::BufferPtr stagingBuffer;
    {
        PPX_LOG_INFO("Storage size for image: " << image.size() << " bytes\n");
        PPX_LOG_INFO("Is image compressed: " << (gli::is_compressed(image.format()) ? "YES" : "NO"));

        grfx::BufferCreateInfo ci      = {};
        ci.size                        = static_cast<uint64_t>(imageHeight) * static_cast<uint64_t>(rowStride);
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

        const char* pSrc = static_cast<const char*>(image.data());
        char*       pDst = static_cast<char*>(pBufferAddress);
        memcpy(pDst, pSrc, image.size());

        stagingBuffer->UnmapMemory();
    }

    // Create target image
    grfx::ImagePtr targetImage;
    {
        grfx::ImageCreateInfo ci       = {};
        ci.type                        = grfx::IMAGE_TYPE_2D;
        ci.width                       = imageWidth;
        ci.height                      = imageHeight;
        ci.depth                       = 1;
        ci.format                      = format;
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = 1;
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.sampled     = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_GPU_ONLY;

        ci.usageFlags.flags |= options.mAdditionalUsage;

        ppxres = pQueue->GetDevice()->CreateImage(&ci, &targetImage);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetImage);
    }

    // Copy info
    grfx::BufferToImageCopyInfo copyInfo = {};
    copyInfo.srcBuffer.imageWidth        = imageWidth;
    copyInfo.srcBuffer.imageHeight       = imageHeight;
    copyInfo.srcBuffer.imageRowStride    = rowStride;
    copyInfo.srcBuffer.footprintOffset   = 0;
    copyInfo.srcBuffer.footprintWidth    = imageWidth;
    copyInfo.srcBuffer.footprintHeight   = imageHeight;
    copyInfo.srcBuffer.footprintDepth    = 1;
    copyInfo.dstImage.mipLevel           = 0;
    copyInfo.dstImage.arrayLayer         = 0;
    copyInfo.dstImage.arrayLayerCount    = 1;
    copyInfo.dstImage.x                  = 0;
    copyInfo.dstImage.y                  = 0;
    copyInfo.dstImage.z                  = 0;
    copyInfo.dstImage.width              = imageWidth;
    copyInfo.dstImage.height             = imageHeight;
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

// -------------------------------------------------------------------------------------------------

Result CreateImageFromFile(
    grfx::Queue*        pQueue,
    const fs::path&     path,
    grfx::Image**       ppImage,
    const ImageOptions& options)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppImage);

    Timer timer;
    PPX_ASSERT_MSG(timer.Start() == ppx::TIMER_RESULT_SUCCESS, "timer start failed");
    double fnStartTime = timer.SecondsSinceStart();

    Result ppxres;
    if (Bitmap::IsBitmapFile(path)) {
        // Load bitmap
        Bitmap bitmap;
        ppxres = Bitmap::LoadFile(path, &bitmap);
        if (Failed(ppxres)) {
            return ppxres;
        }

        ppxres = CreateImageFromBitmap(pQueue, &bitmap, ppImage, options);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }
    else if (IsDDSFile(path)) {
        // Generate a bitmap out of a DDS
        gli::texture image = gli::load(path.c_str());
        if (image.empty()) {
            return Result::ERROR_IMAGE_FILE_LOAD_FAILED;
        }
        PPX_LOG_INFO("Successfully loaded compressed image: " << path);
        ppxres = CreateImageFromCompressedImage(pQueue, image, ppImage, options);
    }
    else {
        ppxres = Result::ERROR_IMAGE_FILE_LOAD_FAILED;
    }

    double fnEndTime = timer.SecondsSinceStart();
    float  fnElapsed = static_cast<float>(fnEndTime - fnStartTime);
    if (ppxres == Result::SUCCESS) {
        PPX_LOG_INFO("Created image from image file: " << path << " (" << FloatString(fnElapsed) << " seconds)");
    }
    else {
        PPX_LOG_INFO("Failed to create image from image file: " << path);
    }

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

Result CopyBitmapToTexture(
    grfx::Queue*        pQueue,
    const Bitmap*       pBitmap,
    grfx::Texture*      pTexture,
    uint32_t            mipLevel,
    uint32_t            arrayLayer,
    grfx::ResourceState stateBefore,
    grfx::ResourceState stateAfter)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pBitmap);
    PPX_ASSERT_NULL_ARG(pTexture);

    Result ppxres = CopyBitmapToImage(
        pQueue,
        pBitmap,
        pTexture->GetImage(),
        mipLevel,
        arrayLayer,
        stateBefore,
        stateAfter);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

Result CreateTextureFromBitmap(
    grfx::Queue*          pQueue,
    const Bitmap*         pBitmap,
    grfx::Texture**       ppTexture,
    const TextureOptions& options)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pBitmap);
    PPX_ASSERT_NULL_ARG(ppTexture);

    Result ppxres = ppx::ERROR_FAILED;

    // Scoped destroy
    grfx::ScopeDestroyer SCOPED_DESTROYER(pQueue->GetDevice());

    // Cap mip level count
    uint32_t maxMipLevelCount = Mipmap::CalculateLevelCount(pBitmap->GetWidth(), pBitmap->GetHeight());
    uint32_t mipLevelCount    = std::min<uint32_t>(options.mMipLevelCount, maxMipLevelCount);

    // Create target texture
    grfx::TexturePtr targetTexture;
    {
        grfx::TextureCreateInfo ci     = {};
        ci.pImage                      = nullptr;
        ci.imageType                   = grfx::IMAGE_TYPE_2D;
        ci.width                       = pBitmap->GetWidth();
        ci.height                      = pBitmap->GetHeight();
        ci.depth                       = 1;
        ci.imageFormat                 = ToGrfxFormat(pBitmap->GetFormat());
        ci.sampleCount                 = grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = mipLevelCount;
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.sampled     = true;
        ci.memoryUsage                 = grfx::MEMORY_USAGE_GPU_ONLY;
        ci.initialState                = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        ci.RTVClearValue               = {0, 0, 0, 0};
        ci.DSVClearValue               = {1.0f, 0xFF};
        ci.sampledImageViewType        = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
        ci.sampledImageViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.renderTargetViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.depthStencilViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.storageImageViewFormat      = grfx::FORMAT_UNDEFINED;
        ci.ownership                   = grfx::OWNERSHIP_REFERENCE;

        ci.usageFlags.flags |= options.mAdditionalUsage;

        ppxres = pQueue->GetDevice()->CreateTexture(&ci, &targetTexture);
        if (Failed(ppxres)) {
            return ppxres;
        }
        SCOPED_DESTROYER.AddObject(targetTexture);
    }

    Mipmap mipmap = Mipmap(*pBitmap, mipLevelCount);
    if (!mipmap.IsOk()) {
        return ppx::ERROR_FAILED;
    }

    // Copy mips to texture
    for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel) {
        const Bitmap* pMip = mipmap.GetMip(mipLevel);

        ppxres = CopyBitmapToTexture(
            pQueue,
            pMip,
            targetTexture,
            mipLevel,
            0,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_SHADER_RESOURCE);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Change ownership to reference so object doesn't get destroyed
    targetTexture->SetOwnership(grfx::OWNERSHIP_REFERENCE);

    // Assign output
    *ppTexture = targetTexture;

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

Result CreateTextureFromFile(
    grfx::Queue*          pQueue,
    const fs::path&       path,
    grfx::Texture**       ppTexture,
    const TextureOptions& options)
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

    ppxres = CreateTextureFromBitmap(pQueue, &bitmap, ppTexture, options);
    if (Failed(ppxres)) {
        return ppxres;
    }

    double fnEndTime = timer.SecondsSinceStart();
    float  fnElapsed = static_cast<float>(fnEndTime - fnStartTime);
    PPX_LOG_INFO("Created texture from image file: " << path << " (" << FloatString(fnElapsed) << " seconds)");

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

Result CreateTexture1x1(
    grfx::Queue*          pQueue,
    const float4&         color,
    grfx::Texture**       ppTexture,
    const TextureOptions& options)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppTexture);

    Result ppxres = ppx::SUCCESS;

    // Create bitmap
    Bitmap bitmap = Bitmap::Create(1, 1, Bitmap::FORMAT_RGBA_UINT8, &ppxres);
    if (Failed(ppxres)) {
        return ppx::ERROR_BITMAP_CREATE_FAILED;
    }

    // Fill color
    bitmap.Fill(color.r, color.g, color.b, color.a);

    ppxres = CreateTextureFromBitmap(pQueue, &bitmap, ppTexture, options);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

struct SubImage
{
    uint32_t width        = 0;
    uint32_t height       = 0;
    uint32_t bufferOffset = 0;
};

SubImage CalcSubimageCrossHorizontalLeft(
    uint32_t     subImageIndex,
    uint32_t     imageWidth,
    uint32_t     imageHeight,
    grfx::Format format)
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

// -------------------------------------------------------------------------------------------------

Result CreateCubeMapFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    const CubeMapCreateInfo*     pCreateInfo,
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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

Result CreateModelFromTriMesh(
    grfx::Queue*   pQueue,
    const TriMesh* pMesh,
    grfx::Model**  ppModel)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pMesh);
    PPX_ASSERT_NULL_ARG(ppModel);

    Result ppxres = ppx::ERROR_FAILED;

    Geometry geo;
    ppxres = Geometry::Create(*pMesh, &geo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = CreateModelFromGeometry(pQueue, &geo, ppModel);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

Result CreateModelFromWireMesh(
    grfx::Queue*    pQueue,
    const WireMesh* pMesh,
    grfx::Model**   ppModel)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pMesh);
    PPX_ASSERT_NULL_ARG(ppModel);

    Result ppxres = ppx::ERROR_FAILED;

    Geometry geo;
    ppxres = Geometry::Create(*pMesh, &geo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = CreateModelFromGeometry(pQueue, &geo, ppModel);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

// -------------------------------------------------------------------------------------------------

Result CreateModelFromFile(
    grfx::Queue*          pQueue,
    const fs::path&       path,
    grfx::Model**         ppModel,
    const TriMeshOptions& options)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(ppModel);

    Result ppxres = ppx::ERROR_FAILED;

    TriMesh mesh = TriMesh::CreateFromOBJ(path, options);

    ppxres = CreateModelFromTriMesh(pQueue, &mesh, ppModel);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx_util
} // namespace ppx
