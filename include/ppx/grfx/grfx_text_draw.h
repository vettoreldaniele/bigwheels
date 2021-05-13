#ifndef ppx_grfx_text_draw_h
#define ppx_grfx_text_draw_h

#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_pipeline.h"
#include "ppx/000_math_config.h"
#include "ppx/font.h"
#include "xxhash.h"

namespace ppx {
namespace grfx {

struct TextureFontUVRect
{
    float u0 = 0;
    float v0 = 0;
    float u1 = 0;
    float v1 = 0;
};

struct TextureFontGlyphMetrics
{
    uint32_t          codepoint    = 0;
    GlyphMetrics      glyphMetrics = {};
    float2            size         = {};
    TextureFontUVRect uvRect       = {};
};

struct TextureFontCreateInfo
{
    ppx::Font   font;
    float       size       = 16.0f;
    std::string characters = ""; // Default characters if empty
};

class TextureFont
    : public grfx::DeviceObject<grfx::TextureFontCreateInfo>
{
public:
    TextureFont() {}
    virtual ~TextureFont() {}

    const ppx::Font& GetFont() const { return mCreateInfo.font; }
    float            GetSize() const { return mCreateInfo.size; }
    std::string      GetCharacters() const { return mCreateInfo.characters; }
    grfx::TexturePtr GetTexture() const { return mTexture; }

    float                                GetAscent() const { return mFontMetrics.ascent; }
    float                                GetDescent() const { return mFontMetrics.descent; }
    float                                GetLineGap() const { return mFontMetrics.lineGap; }
    const grfx::TextureFontGlyphMetrics* GetGlyphMetrics(uint32_t codepoint) const;

protected:
    virtual Result CreateApiObjects(const grfx::TextureFontCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    FontMetrics                                mFontMetrics;
    std::vector<grfx::TextureFontGlyphMetrics> mGlyphMetrics;
    grfx::TexturePtr                           mTexture;
};

// -------------------------------------------------------------------------------------------------

struct TextDrawCreateInfo
{
    grfx::TextureFont*    pFont              = nullptr;
    uint32_t              maxTextLength      = 4096;
    grfx::ShaderStageInfo VS                 = {}; // Use basic/shaders/TextDraw.hlsl (vsmain) for now
    grfx::ShaderStageInfo PS                 = {}; // Use basic/shaders/TextDraw.hlsl (psmain) for now
    grfx::Format          renderTargetFormat = grfx::FORMAT_UNDEFINED;
    grfx::Format          depthStencilFormat = grfx::FORMAT_UNDEFINED;
};

class TextDraw
    : public grfx::DeviceObject<grfx::TextDrawCreateInfo>
{
public:
    TextDraw() {}
    virtual ~TextDraw() {}

    void Clear();

    void AddString(
        const float2&      position,
        const std::string& string,
        const float3&      color   = float3(1, 1, 1),
        float              opacity = 1.0f);

    // Use this if text is static
    ppx::Result UploadToGpu(grfx::Queue* pQueue);

    // Use this if text is dynamic
    void UploadToGpu(grfx::CommandBuffer* pCommandBuffer);

    void PrepareDraw(const float4x4& MVP, grfx::CommandBuffer* pCommandBuffer);
    void Draw(grfx::CommandBuffer* pCommandBuffer);

protected:
    virtual Result CreateApiObjects(const grfx::TextDrawCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    uint32_t                     mTextLength = 0;
    grfx::BufferPtr              mCpuIndexBuffer;
    grfx::BufferPtr              mCpuVertexBuffer;
    grfx::BufferPtr              mGpuIndexBuffer;
    grfx::BufferPtr              mGpuVertexBuffer;
    grfx::IndexBufferView        mIndexBufferView  = {};
    grfx::VertexBufferView       mVertexBufferView = {};
    grfx::BufferPtr              mCpuConstantBuffer;
    grfx::BufferPtr              mGpuConstantBuffer;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mDescriptorSetLayout;
    grfx::DescriptorSetPtr       mDescriptorSet;
    grfx::PipelineInterfacePtr   mPipelineInterface;
    grfx::GraphicsPipelinePtr    mPipeline;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_text_draw_h
