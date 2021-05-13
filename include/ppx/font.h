#ifndef ppx_font_h
#define ppx_font_h

#include "ppx/000_config.h"
#include "ppx/000_math_config.h"
#include "ppx/fs.h"
#include "stb_truetype.h"

namespace ppx {

struct FontMetrics
{
    float ascent  = 0;
    float descent = 0;
    float lineGap = 0;
};

struct GlyphBox
{
    int32_t x0 = 0;
    int32_t y0 = 0;
    int32_t x1 = 0;
    int32_t y1 = 0;
};

struct GlyphMetrics
{
    float    advance     = 0;
    float    leftBearing = 0;
    GlyphBox box         = {};
};

class Font
{
public:
    Font();
    virtual ~Font();

    static ppx::Result CreateFromFile(const fs::path& path, ppx::Font* pFont);
    static ppx::Result CreateFromMemory(size_t size, const char* pData, ppx::Font* pFont);

    float GetScale(float fontSizeInPixels) const;

    void GetFontMetrics(float fontSizeInPixels, FontMetrics* pMetrics) const;

    void GetGlyphMetrics(
        float         fontSizeInPixels,
        uint32_t      codepoint,
        float         subpixelShiftX,
        float         subpixelShiftY,
        GlyphMetrics* pMetrics) const;

    void RenderGlyphBitmap(
        float          fontSizeInPixels,
        uint32_t       codepoint,
        float          subpixelShiftX,
        float          subpixelShiftY,
        uint32_t       glyphWidth,
        uint32_t       glyphHeight,
        uint32_t       rowStride,
        unsigned char* pOutput) const;

private:
    void AcquireFontMetrics();

private:
    struct Object
    {
        std::vector<unsigned char> fontData;
        stbtt_fontinfo             fontInfo;
        int                        ascent  = 0;
        int                        descent = 0;
        int                        lineGap = 0;
    };
    std::shared_ptr<Object> mObject;
};

} // namespace ppx

#endif // ppx_font_h
