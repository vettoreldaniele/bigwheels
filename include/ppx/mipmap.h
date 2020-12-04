#ifndef ppx_mipmap_h
#define ppx_mipmap_h

#include "ppx/bitmap.h"
#include "ppx/grfx/grfx_constants.h"

namespace ppx {

class Mipmap
{
public:
    Mipmap() {}
    Mipmap(uint32_t width, uint32_t height, Bitmap::Format format, uint32_t levelCount);
    Mipmap(const Bitmap& bitmap, uint32_t levelCount);
    ~Mipmap() {}

    // Returns true if there's at least one mip level, format is valid, and storage is valid
    bool IsOk() const;

    Bitmap::Format GetFormat() const;
    uint32_t       GetLevelCount() const { return CountU32(mMips); }
    Bitmap*        GetMip(uint32_t level);
    const Bitmap*  GetMip(uint32_t level) const;

    uint32_t GetWidth(uint32_t level) const;
    uint32_t GetHeight(uint32_t level) const;

    static uint32_t CalculateLevelCount(uint32_t width, uint32_t height);
    static Result   LoadFile(const fs::path& path, Mipmap* pMipmap, uint32_t levelCount = PPX_ALL_MIP_LEVELS);

private:
    std::vector<char>
                        mData;
    std::vector<Bitmap> mMips;
};

} // namespace ppx

#endif // ppx_mipmap_h
