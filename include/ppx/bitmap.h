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
    enum DataType
    {
        DATA_TYPE_UNDEFINED = 0,
        DATA_TYPE_UINT8,
        DATA_TYPE_UINT16,
        DATA_TYPE_UINT32,
        DATA_TYPE_FLOAT,
    };

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

    // ---------------------------------------------------------------------------------------------

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
    uint64_t       GetFootprintSize(uint32_t rowStrideAlignment = 1) const;

    Result Resize(uint32_t width, uint32_t height);
    //Result Resize(uint32_t widht, uint32_t height, Bitmap* pBitmap) const;
    //Result CopyTo(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Bitmap* pBitmap) const;

    void Fill(float r, float g, float b, float a);

    // Returns byte address of pixel at (x,y)
    char*       GetPixelAddress(uint32_t x, uint32_t y);
    const char* GetPixelAddress(uint32_t x, uint32_t y) const;

    // These functions will return null if the Bitmap's format doesn't
    // the function. For example, GetPixel8u() returns null if the format
    // is FORMAT_RGBA_FLOAT.
    //
    uint8_t*        GetPixel8u(uint32_t x, uint32_t y);
    const uint8_t*  GetPixel8u(uint32_t x, uint32_t y) const;
    uint16_t*       GetPixel16u(uint32_t x, uint32_t y);
    const uint16_t* GetPixel16u(uint32_t x, uint32_t y) const;
    uint32_t*       GetPixel32u(uint32_t x, uint32_t y);
    const uint32_t* GetPixel32u(uint32_t x, uint32_t y) const;
    float*          GetPixel32f(uint32_t x, uint32_t y);
    const float*    GetPixel32f(uint32_t x, uint32_t y) const;

    static uint32_t         ChannelSize(Bitmap::Format value);
    static uint32_t         ChannelCount(Bitmap::Format value);
    static Bitmap::DataType ChannelDataType(Bitmap::Format value);
    static uint32_t         FormatSize(Bitmap::Format value);
    static uint64_t         StorageFootprint(uint32_t width, uint32_t height, Bitmap::Format format);

    static Result LoadFile(const fs::path& path, Bitmap* pBitmap);

    // ---------------------------------------------------------------------------------------------

    class PixelIterator
    {
    private:
        friend class ppx::Bitmap;

        PixelIterator(Bitmap* pBitmap)
            : mBitmap(pBitmap)
        {
            Reset();
        }

    public:
        void Reset()
        {
            mX            = 0;
            mY            = 0;
            mPixelAddress = mBitmap->GetPixelAddress(mX, mY);
        }

        bool Done() const
        {
            bool done = (mY >= mBitmap->GetHeight());
            return done;
        }

        bool Next()
        {
            if (Done()) {
                return false;
            }

            mX += 1;
            mPixelAddress += mBitmap->GetPixelStride();
            if (mX == mBitmap->GetWidth()) {
                mY += 1;
                mX            = 0;
                mPixelAddress = mBitmap->GetPixelAddress(mX, mY);
            }

            return Done() ? false : true;
        }

        uint32_t       GetX() const { return mX; }
        uint32_t       GetY() const { return mY; }
        Bitmap::Format GetFormat() const { return mBitmap->GetFormat(); }
        uint32_t       GetChannelCount() const { return Bitmap::ChannelCount(GetFormat()); }

        template <typename T>
        T* GetPixelAddress() const { return reinterpret_cast<T*>(mPixelAddress); }

    private:
        Bitmap*  mBitmap       = nullptr;
        uint32_t mX            = 0;
        uint32_t mY            = 0;
        char*    mPixelAddress = nullptr;
    };

    // ---------------------------------------------------------------------------------------------

    PixelIterator GetPixelIterator() { return PixelIterator(this); }

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
