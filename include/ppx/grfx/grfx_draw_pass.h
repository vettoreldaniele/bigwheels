#ifndef ppx_grfx_draw_pass_h
#define ppx_grfx_draw_pass_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

struct RenderPassBeginInfo;

//! @struct DrawPassCreateInfo
//!
//! Use this version if the format(s) are known but images need creation.
//!
//! Backing images will be created using the criteria provided in this struct.
//!
struct DrawPassCreateInfo
{
    uint32_t                     width                                           = 0;
    uint32_t                     height                                          = 0;
    grfx::SampleCount            sampleCount                                     = grfx::SAMPLE_COUNT_1;
    uint32_t                     renderTargetCount                               = 0;
    grfx::Format                 renderTargetFormats[PPX_MAX_RENDER_TARGETS]     = {};
    grfx::Format                 depthStencilFormat                              = grfx::FORMAT_UNDEFINED;
    grfx::ImageUsageFlags        renderTargetUsageFlags[PPX_MAX_RENDER_TARGETS]  = {};
    grfx::ImageUsageFlags        depthStencilUsageFlags                          = {};
    grfx::RenderTargetClearValue renderTargetClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue depthStencilClearValue                          = {};
};

//! @struct DrawPassCreateInfo2
//!
//! Use this version if the images exists.
//!
struct DrawPassCreateInfo2
{
    uint32_t                     width                                           = 0;
    uint32_t                     height                                          = 0;
    uint32_t                     renderTargetCount                               = 0;
    grfx::Image*                 pRenderTargetImages[PPX_MAX_RENDER_TARGETS]     = {};
    grfx::Image*                 pDepthStencilImage                              = nullptr;
    grfx::RenderTargetClearValue renderTargetClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue depthStencilClearValue                          = {};
};

namespace internal {

struct DrawPassCreateInfo
{
    enum CreateInfoVersion
    {
        CREATE_INFO_VERSION_UNDEFINED = 0,
        CREATE_INFO_VERSION_1         = 1,
        CREATE_INFO_VERSION_2         = 2,
    };

    CreateInfoVersion version           = CREATE_INFO_VERSION_UNDEFINED;
    uint32_t          width             = 0;
    uint32_t          height            = 0;
    uint32_t          renderTargetCount = 0;

    // Data unique to grfx::DrawPassCreateInfo2
    struct
    {
        grfx::SampleCount     sampleCount                                    = grfx::SAMPLE_COUNT_1;
        grfx::Format          renderTargetFormats[PPX_MAX_RENDER_TARGETS]    = {};
        grfx::Format          depthStencilFormat                             = grfx::FORMAT_UNDEFINED;
        grfx::ImageUsageFlags renderTargetUsageFlags[PPX_MAX_RENDER_TARGETS] = {};
        grfx::ImageUsageFlags depthStencilUsageFlags                         = {};
    } V1;

    // Data unique to grfx::DrawPassCreateInfo3
    struct
    {
        grfx::Image* pRenderTargetImages[PPX_MAX_RENDER_TARGETS] = {};
        grfx::Image* pDepthStencilImage                          = nullptr;
    } V2;

    // Clear values
    grfx::RenderTargetClearValue renderTargetClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue depthStencilClearValue                          = {};

    DrawPassCreateInfo() {}
    DrawPassCreateInfo(const grfx::DrawPassCreateInfo& obj);
    DrawPassCreateInfo(const grfx::DrawPassCreateInfo2& obj);
};

} // namespace internal

//! @class DrawPass
//!
//!
class DrawPass
    : public DeviceObject<grfx::internal::DrawPassCreateInfo>
{
public:
    DrawPass() {}
    virtual ~DrawPass() {}

    const grfx::Rect& GetRenderArea() const { return mRenderArea; }

    void PrepareRenderPassBeginInfo(const grfx::DrawPassClearFlags& clearFlags, grfx::RenderPassBeginInfo* pBeginInfo) const;

protected:
    virtual Result CreateApiObjects(const grfx::internal::DrawPassCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result CreateTexturesV1(const grfx::internal::DrawPassCreateInfo* pCreateInfo);
    Result CreateTexturesV2(const grfx::internal::DrawPassCreateInfo* pCreateInfo);

private:
    grfx::Rect                    mRenderArea = {};
    std::vector<grfx::TexturePtr> mRenderTargetTextures;
    grfx::TexturePtr              mDepthStencilTexture;

    struct Pass
    {
        uint32_t            clearMask = 0;
        grfx::RenderPassPtr renderPass;
    };

    std::vector<Pass> mPasses;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_draw_pass_h
