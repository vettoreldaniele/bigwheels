#ifndef ppx_bitmap_h
#define ppx_bitmap_h

#include "ppx/000_config.h"
#include "ppx/fs.h"
#include "ppx/grfx/grfx_format.h"

namespace ppx {

//! @class Bitmap
//!
//!
class Bitmap
{
public:
    enum Format
    {
        FORMAT_UNDEFINED = 0,
        FORMAT_R_UINT8,
        FORMAT_RG_UINT8,
        FORMAT_RGB_UINT8,
        FORMAT_RGBA_UINT8,
        FORMAT_R_UINT16,
        FORMAT_RG_UINT16,
        FORMAT_RGB_UINT16,
        FORMAT_RGBA_UINT16,
        FORMAT_R_UINT32,
        FORMAT_RG_UINT32,
        FORMAT_RGB_UINT32,
        FORMAT_RGBA_UINT32,
        FORMAT_R_FLOAT,
        FORMAT_RG_FLOAT,
        FORMAT_RGB_FLOAT,
        FORMAT_RGBA_FLOAT,
    };

    Bitmap(Bitmap::Format format = Bitmap::FORMAT_UNDEFINED);

    // If 'pExternalStorage' is not null then it must point to a valid
    // allocation with enough storage for the bitmap.
    //
    Bitmap(uint32_t width, uint32_t height, Bitmap::Format format, char* pExternalStorage = nullptr);

    // Returns true if dimensions are greater than one, format is valid, and storage isvalid
    bool IsOk() const;

    uint32_t       GetWidth() const { return mWidth; }
    uint32_t       GetHeight() const { return mHeight; }
    Bitmap::Format GetFormat() const { return mFormat; };
    uint32_t       GetPixelStride() const { return mPixelStride; }
    uint32_t       GetRowStride() const { return mRowStride; }
    char*          GetData() const { return mData; }
    uint64_t       GetFootprintSize() const;

    Result Resize(uint32_t width, uint32_t height);

    static uint32_t ChannelSize(Bitmap::Format value);
    static uint32_t ChannelCount(Bitmap::Format value);
    static uint32_t FormatSize(Bitmap::Format value);
    static uint64_t StorageFootprint(uint32_t width, uint32_t height, Bitmap::Format format);

    static Result LoadFile(const fs::path& path, Bitmap* pBitmap);

private:
    uint32_t          mWidth           = 0;
    uint32_t          mHeight          = 0;
    Bitmap::Format    mFormat          = Bitmap::FORMAT_UNDEFINED;
    uint32_t          mPixelStride     = 0;
    uint32_t          mRowStride       = 0;
    char*             mData            = nullptr;
    std::vector<char> mInternalStorage = {};
};

} // namespace ppx

#endif // ppx_bitmap_h
