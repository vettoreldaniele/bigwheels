#include "ppx/bitmap.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

namespace ppx {

Bitmap::Bitmap(Bitmap::Format format)
    : mFormat(format),
      mPixelStride(Bitmap::FormatSize(format))
{
}

Bitmap::Bitmap(uint32_t width, uint32_t height, Bitmap::Format format, char* pExternalStorage)
    : mWidth(width),
      mHeight(height),
      mFormat(format),
      mPixelStride(Bitmap::FormatSize(format)),
      mRowStride(width * Bitmap::FormatSize(format)),
      mData(pExternalStorage)
{
    assert(format != Bitmap::FORMAT_UNDEFINED);

    if (mData == nullptr) {
        size_t n = Bitmap::StorageFootprint(width, height, format);
        if (n > 0) {
            mInternalStorage.resize(n);
            mData = mInternalStorage.data();
        }
    }
}

bool Bitmap::IsOk() const
{
    bool isSizeValid    = (mWidth > 0) && (mHeight > 0);
    bool isFormatValid  = (mFormat != Bitmap::FORMAT_UNDEFINED);
    bool isStorageValid = (mData != nullptr);
    return isSizeValid && isFormatValid && isStorageValid;
}

uint64_t Bitmap::GetFootprintSize(uint32_t rowStrideAlignment) const
{
    uint32_t alignedRowStride = RoundUp<uint32_t>(mRowStride, rowStrideAlignment);
    uint64_t size             = alignedRowStride * mHeight;
    return size;
}

Result Bitmap::Resize(uint32_t width, uint32_t height)
{
    // If internal storage is empty then this bitmap is using
    // external storage...so don't resize!
    //
    if (mInternalStorage.empty()) {
        return ppx::ERROR_IMAGE_CANNOT_RESIZE_EXTERNAL_STORAGE;
    }

    mWidth     = width;
    mHeight    = height;
    mRowStride = width * mPixelStride;

    size_t n = Bitmap::StorageFootprint(mWidth, mHeight, mFormat);
    mInternalStorage.resize(n);
    mData = (mInternalStorage.size() > 0) ? mInternalStorage.data() : nullptr;

    return ppx::SUCCESS;
}

void Bitmap::Fill(float r, float g, float b, float a)
{
    PPX_ASSERT_MSG(mData != nullptr, "data is null");

    if (mFormat == Bitmap::FORMAT_UNDEFINED) {
        PPX_ASSERT_MSG(false, "format is undefined");
        return;
    }

    uint8_t rgbaU8[4] = {
        static_cast<uint8_t>(UINT8_MAX * std::min<float>(r, 1.0f)),
        static_cast<uint8_t>(UINT8_MAX * std::min<float>(g, 1.0f)),
        static_cast<uint8_t>(UINT8_MAX * std::min<float>(b, 1.0f)),
        static_cast<uint8_t>(UINT8_MAX * std::min<float>(a, 1.0f)),
    };

    uint16_t rgbaU16[4] = {
        static_cast<uint8_t>(UINT16_MAX * std::min<float>(r, 1.0f)),
        static_cast<uint8_t>(UINT16_MAX * std::min<float>(g, 1.0f)),
        static_cast<uint8_t>(UINT16_MAX * std::min<float>(b, 1.0f)),
        static_cast<uint8_t>(UINT16_MAX * std::min<float>(a, 1.0f)),
    };

    uint16_t rgbaU32[4] = {
        static_cast<uint8_t>(UINT32_MAX * std::min<float>(r, 1.0f)),
        static_cast<uint8_t>(UINT32_MAX * std::min<float>(g, 1.0f)),
        static_cast<uint8_t>(UINT32_MAX * std::min<float>(b, 1.0f)),
        static_cast<uint8_t>(UINT32_MAX * std::min<float>(a, 1.0f)),
    };

    float rgbaF32[4] = {
        r,
        g,
        b,
        a,
    };

    const uint32_t         channelCount = Bitmap::ChannelCount(mFormat);
    const Bitmap::DataType dataType     = Bitmap::ChannelDataType(mFormat);

    char* pData = mData;
    for (uint32_t y = 0; y < mHeight; ++y) {
        for (uint32_t x = 0; x < mWidth; ++x) {
            uint8_t*  pDataU8  = reinterpret_cast<uint8_t*>(pData);
            uint16_t* pDataU16 = reinterpret_cast<uint16_t*>(pData);
            uint32_t* pDataU32 = reinterpret_cast<uint32_t*>(pData);
            float*    pDataF32 = reinterpret_cast<float*>(pData);
            for (uint32_t c = 0; c < channelCount; ++c) {
                // clang-format off
                switch (dataType) {
                    default: break;
                    case Bitmap::DATA_TYPE_UINT8  : pDataU8[c]  = rgbaU8[c]; break;
                    case Bitmap::DATA_TYPE_UINT16 : pDataU16[c] = rgbaU16[c]; break;
                    case Bitmap::DATA_TYPE_UINT32 : pDataU32[c] = rgbaU32[c]; break;
                    case Bitmap::DATA_TYPE_FLOAT  : pDataF32[c] = rgbaF32[c]; break;
                }
                // clang-foramt on
            }
            pData += mPixelStride;
        }
    }
}

uint32_t Bitmap::ChannelSize(Bitmap::Format value)
{
    // clang-format off
    switch (value) {
    default: break;
        case Bitmap::FORMAT_R_UINT8     : return 1; break;
        case Bitmap::FORMAT_RG_UINT8    : return 1; break;
        case Bitmap::FORMAT_RGB_UINT8   : return 1; break;
        case Bitmap::FORMAT_RGBA_UINT8  : return 1; break;

        case Bitmap::FORMAT_R_UINT16    : return 2; break;
        case Bitmap::FORMAT_RG_UINT16   : return 2; break;
        case Bitmap::FORMAT_RGB_UINT16  : return 2; break;
        case Bitmap::FORMAT_RGBA_UINT16 : return 2; break;

        case Bitmap::FORMAT_R_UINT32    : return 4; break;
        case Bitmap::FORMAT_RG_UINT32   : return 4; break;
        case Bitmap::FORMAT_RGB_UINT32  : return 4; break;
        case Bitmap::FORMAT_RGBA_UINT32 : return 4; break;

        case Bitmap::FORMAT_R_FLOAT     : return 4; break;
        case Bitmap::FORMAT_RG_FLOAT    : return 4; break;
        case Bitmap::FORMAT_RGB_FLOAT   : return 4; break;
        case Bitmap::FORMAT_RGBA_FLOAT  : return 4; break;
    }
    // clang-format on
    return 0;
}

uint32_t Bitmap::ChannelCount(Bitmap::Format value)
{
    // clang-format off
    switch (value) {
    default: break;
        case Bitmap::FORMAT_R_UINT8     : return 1; break;
        case Bitmap::FORMAT_RG_UINT8    : return 2; break;
        case Bitmap::FORMAT_RGB_UINT8   : return 3; break;
        case Bitmap::FORMAT_RGBA_UINT8  : return 4; break;

        case Bitmap::FORMAT_R_UINT16    : return 1; break;
        case Bitmap::FORMAT_RG_UINT16   : return 2; break;
        case Bitmap::FORMAT_RGB_UINT16  : return 3; break;
        case Bitmap::FORMAT_RGBA_UINT16 : return 4; break;

        case Bitmap::FORMAT_R_UINT32    : return 1; break;
        case Bitmap::FORMAT_RG_UINT32   : return 2; break;
        case Bitmap::FORMAT_RGB_UINT32  : return 3; break;
        case Bitmap::FORMAT_RGBA_UINT32 : return 4; break;

        case Bitmap::FORMAT_R_FLOAT     : return 1; break;
        case Bitmap::FORMAT_RG_FLOAT    : return 2; break;
        case Bitmap::FORMAT_RGB_FLOAT   : return 3; break;
        case Bitmap::FORMAT_RGBA_FLOAT  : return 4; break;
    }
    // clang-format on
    return 0;
}

Bitmap::DataType Bitmap::ChannelDataType(Bitmap::Format value)
{
    // clang-format off
    switch (value) {
        default: break;
        case Bitmap::FORMAT_R_UINT8:
        case Bitmap::FORMAT_RG_UINT8:
        case Bitmap::FORMAT_RGB_UINT8:
        case Bitmap::FORMAT_RGBA_UINT8: {
            return Bitmap::DATA_TYPE_UINT8;
        } break;

        case Bitmap::FORMAT_R_UINT16:
        case Bitmap::FORMAT_RG_UINT16:
        case Bitmap::FORMAT_RGB_UINT16:
        case Bitmap::FORMAT_RGBA_UINT16: {
            return Bitmap::DATA_TYPE_UINT16;
        } break;

        case Bitmap::FORMAT_R_UINT32:
        case Bitmap::FORMAT_RG_UINT32:
        case Bitmap::FORMAT_RGB_UINT32:
        case Bitmap::FORMAT_RGBA_UINT32: {
            return Bitmap::DATA_TYPE_UINT32;
        } break;

        case Bitmap::FORMAT_R_FLOAT:
        case Bitmap::FORMAT_RG_FLOAT:
        case Bitmap::FORMAT_RGB_FLOAT:
        case Bitmap::FORMAT_RGBA_FLOAT: {
            return Bitmap::DATA_TYPE_FLOAT;
        } break;
    }
    // clang-format on
    return Bitmap::DATA_TYPE_UNDEFINED;
}

uint32_t Bitmap::FormatSize(Bitmap::Format value)
{
    uint32_t channelSize  = Bitmap::ChannelSize(value);
    uint32_t channelCount = Bitmap::ChannelCount(value);
    uint32_t size         = channelSize * channelCount;
    return size;
}

uint64_t Bitmap::StorageFootprint(uint32_t width, uint32_t height, Bitmap::Format format)
{
    uint64_t size = width * height * Bitmap::FormatSize(format);
    return size;
}

Result Bitmap::LoadFile(const fs::path& path, Bitmap* pBitmap)
{
    if (!fs::exists(path)) {
        return ppx::ERROR_PATH_DOES_NOT_EXIST;
    }

    const char* kRadianceSig = "#?RADIANCE";

    // Detect if file is an HDR Radiance file
    bool isRadiance = false;
    {
        // Open file
        FILE* pFile = fopen(path, "rb");
        if (pFile == nullptr) {
            return ppx::ERROR_IMAGE_FILE_LOAD_FAILED;
        }
        // Signature buffer
        const size_t kBufferSize      = 10;
        char         buf[kBufferSize] = {0};

        // Read signature
        size_t n = fread(buf, 1, kBufferSize, pFile);

        // Close file
        fclose(pFile);

        // Only check if kBufferSize bytes were read
        if (n == kBufferSize) {
            int res    = strncmp(buf, kRadianceSig, kBufferSize);
            isRadiance = (res == 0);
        }
    }

    // @TODO: Refactor to remove redundancies from both blocks
    //
    if (isRadiance) {
        int width            = 0;
        int height           = 0;
        int channels         = 0;
        int requiredChannels = 4;

        // Force 3 channels since we're being lazy
        float* pData = stbi_loadf(path, &width, &height, &channels, requiredChannels);
        if (pData == nullptr) {
            return ppx::ERROR_IMAGE_FILE_LOAD_FAILED;
        }

        Bitmap::Format format = Bitmap::FORMAT_RGBA_FLOAT;
        *pBitmap              = Bitmap(width, height, format);
        if (!pBitmap->IsOk()) {
            // Something has gone really wrong if this happens
            return ppx::ERROR_FAILED;
        }

        size_t nbytes = Bitmap::StorageFootprint(static_cast<uint32_t>(width), static_cast<uint32_t>(height), format);
        std::memcpy(pBitmap->GetData(), pData, nbytes);
    }
    else {
        int width            = 0;
        int height           = 0;
        int channels         = 0;
        int requiredChannels = 4; // Force to 4 chanenls to make things easier for the graphics APIs

        unsigned char* pData = stbi_load(path.c_str(), &width, &height, &channels, requiredChannels);
        if (IsNull(pData)) {
            return ppx::ERROR_IMAGE_FILE_LOAD_FAILED;
        }

        Bitmap::Format format = Bitmap::FORMAT_RGBA_UINT8;
        *pBitmap              = Bitmap(width, height, format);
        if (!pBitmap->IsOk()) {
            // Something has gone really wrong if this happens
            return ppx::ERROR_FAILED;
        }

        size_t nbytes = Bitmap::StorageFootprint(static_cast<uint32_t>(width), static_cast<uint32_t>(height), format);
        std::memcpy(pBitmap->GetData(), pData, nbytes);
    }

    return ppx::SUCCESS;
}

} // namespace ppx
