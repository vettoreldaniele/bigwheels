#ifndef ppx_grfx_render_pass_h
#define ppx_grfx_render_pass_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct RenderPassCreateInfo
//!
//! Use this if the RTVs and/or the DSV exists.
//!
struct RenderPassCreateInfo
{
    uint32_t                     width                                           = 0;
    uint32_t                     height                                          = 0;
    uint32_t                     renderTargetCount                               = 0;
    grfx::RenderTargetView*      pRenderTargetViews[PPX_MAX_RENDER_TARGETS]      = {};
    grfx::DepthStencilView*      pDepthStencilView                               = nullptr;
    grfx::RenderTargetClearValue renderTargetClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue depthStencilClearValue                          = {};

    void SetAllRenderTargetClearValue(const grfx::RenderTargetClearValue& value);
};

//! @struct RenderPassCreateInfo2
//!
//! Use this version if only the format is RTVs and/or DSV.
//!
//! RTVs, DSV, and backing images will be created using the
//! criteria provided in this struct.
//!
struct RenderPassCreateInfo2
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
    grfx::AttachmentLoadOp       renderTargetLoadOps[PPX_MAX_RENDER_TARGETS]     = {grfx::ATTACHMENT_LOAD_OP_LOAD};
    grfx::AttachmentStoreOp      renderTargetStoreOps[PPX_MAX_RENDER_TARGETS]    = {grfx::ATTACHMENT_STORE_OP_STORE};
    grfx::AttachmentLoadOp       depthLoadOp                                     = grfx::ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp      depthStoreOp                                    = grfx::ATTACHMENT_STORE_OP_STORE;
    grfx::AttachmentLoadOp       stencilLoadOp                                   = grfx::ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp      stencilStoreOp                                  = grfx::ATTACHMENT_STORE_OP_STORE;

    void SetAllRenderTargetUsageFlags(const grfx::ImageUsageFlags& flags);
    void SetAllRenderTargetClearValue(const grfx::RenderTargetClearValue& value);
    void SetAllRenderTargetLoadOp(grfx::AttachmentLoadOp op);
    void SetAllRenderTargetStoreOp(grfx::AttachmentStoreOp op);
    void SetAllRenderTargetToClear();
};

//! @struct RenderPassCreateInfo3
//!
//! Use this if the images exists but views need creation.
//!
struct RenderPassCreateInfo3
{
    uint32_t                     width                                           = 0;
    uint32_t                     height                                          = 0;
    uint32_t                     renderTargetCount                               = 0;
    grfx::Image*                 pRenderTargetImages[PPX_MAX_RENDER_TARGETS]     = {};
    grfx::Image*                 pDepthStencilImage                              = nullptr;
    grfx::RenderTargetClearValue renderTargetClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue depthStencilClearValue                          = {};
    grfx::AttachmentLoadOp       renderTargetLoadOps[PPX_MAX_RENDER_TARGETS]     = {grfx::ATTACHMENT_LOAD_OP_LOAD};
    grfx::AttachmentStoreOp      renderTargetStoreOps[PPX_MAX_RENDER_TARGETS]    = {grfx::ATTACHMENT_STORE_OP_STORE};
    grfx::AttachmentLoadOp       depthLoadOp                                     = grfx::ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp      depthStoreOp                                    = grfx::ATTACHMENT_STORE_OP_STORE;
    grfx::AttachmentLoadOp       stencilLoadOp                                   = grfx::ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp      stencilStoreOp                                  = grfx::ATTACHMENT_STORE_OP_STORE;

    void SetAllRenderTargetClearValue(const grfx::RenderTargetClearValue& value);
    void SetAllRenderTargetLoadOp(grfx::AttachmentLoadOp op);
    void SetAllRenderTargetStoreOp(grfx::AttachmentStoreOp op);
    void SetAllRenderTargetToClear();
};

namespace internal {

struct RenderPassCreateInfo
{
    enum CreateInfoVersion
    {
        CREATE_INFO_VERSION_UNDEFINED = 0,
        CREATE_INFO_VERSION_1         = 1,
        CREATE_INFO_VERSION_2         = 2,
        CREATE_INFO_VERSION_3         = 3,
    };

    CreateInfoVersion version           = CREATE_INFO_VERSION_UNDEFINED;
    uint32_t          width             = 0;
    uint32_t          height            = 0;
    uint32_t          renderTargetCount = 0;

    // Data unique to grfx::RenderPassCreateInfo
    struct
    {
        grfx::RenderTargetView* pRenderTargetViews[PPX_MAX_RENDER_TARGETS] = {};
        grfx::DepthStencilView* pDepthStencilView                          = nullptr;
    } V1;

    // Data unique to grfx::RenderPassCreateInfo2
    struct
    {
        grfx::SampleCount     sampleCount                                    = grfx::SAMPLE_COUNT_1;
        grfx::Format          renderTargetFormats[PPX_MAX_RENDER_TARGETS]    = {};
        grfx::Format          depthStencilFormat                             = grfx::FORMAT_UNDEFINED;
        grfx::ImageUsageFlags renderTargetUsageFlags[PPX_MAX_RENDER_TARGETS] = {};
        grfx::ImageUsageFlags depthStencilUsageFlags                         = {};
    } V2;

    // Data unique to grfx::RenderPassCreateInfo3
    struct
    {
        grfx::Image* pRenderTargetImages[PPX_MAX_RENDER_TARGETS] = {};
        grfx::Image* pDepthStencilImage                          = nullptr;
    } V3;

    // Clear values
    grfx::RenderTargetClearValue renderTargetClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue depthStencilClearValue                          = {};

    // Load/store ops
    grfx::AttachmentLoadOp  renderTargetLoadOps[PPX_MAX_RENDER_TARGETS]  = {grfx::ATTACHMENT_LOAD_OP_LOAD};
    grfx::AttachmentStoreOp renderTargetStoreOps[PPX_MAX_RENDER_TARGETS] = {grfx::ATTACHMENT_STORE_OP_STORE};
    grfx::AttachmentLoadOp  depthLoadOp                                  = grfx::ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp depthStoreOp                                 = grfx::ATTACHMENT_STORE_OP_STORE;
    grfx::AttachmentLoadOp  stencilLoadOp                                = grfx::ATTACHMENT_LOAD_OP_LOAD;
    grfx::AttachmentStoreOp stencilStoreOp                               = grfx::ATTACHMENT_STORE_OP_STORE;

    RenderPassCreateInfo() {}
    RenderPassCreateInfo(const grfx::RenderPassCreateInfo& obj);
    RenderPassCreateInfo(const grfx::RenderPassCreateInfo2& obj);
    RenderPassCreateInfo(const grfx::RenderPassCreateInfo3& obj);
};

} // namespace internal

//! @class RenderPass
//!
//!
class RenderPass
    : public grfx::DeviceObject<grfx::internal::RenderPassCreateInfo>
{
public:
    RenderPass() {}
    virtual ~RenderPass() {}

    const grfx::Rect& GetRenderArea() const { return mRenderArea; }

    uint32_t GetRenderTargetCount() const { return mCreateInfo.renderTargetCount; }

    Result GetRenderTargetView(uint32_t index, grfx::RenderTargetView** ppView) const;
    Result GetDepthStencilView(grfx::DepthStencilView** ppView) const;

    Result GetRenderTargetImage(uint32_t index, grfx::Image** ppImage) const;
    Result GetDepthStencilImage(grfx::Image** ppImage) const;

    //! This only applies to grfx::RenderPass objects created using grfx::RenderPassCreateInfo2.
    //! These functions will 'isExternal' to true resulting in these objects *not* getting
    //! destroyed when the encapsulating grfx::RenderPass object is destroyed.
    //!
    //! Calling these fuctions on grfx::RenderPass objects created using using grfx::RenderPassCreateInfo
    //! will still return a valid object if the index or DSV object exists.
    //!
    Result DisownRenderTargetView(uint32_t index, grfx::RenderTargetView** ppView);
    Result DisownDepthStencilView(grfx::DepthStencilView** ppView);
    Result DisownRenderTargetImage(uint32_t index, grfx::Image** ppImage);
    Result DisownDepthStencilImage(grfx::Image** ppImage);

    // Convenience functions returns empty ptr if index is out of range or DSV object does not exist.
    grfx::RenderTargetViewPtr GetRenderTargetView(uint32_t index) const;
    grfx::DepthStencilViewPtr GetDepthStencilView() const;
    grfx::ImagePtr            GetRenderTargetImage(uint32_t index) const;
    grfx::ImagePtr            GetDepthStencilImage() const;

protected:
    virtual Result Create(const grfx::internal::RenderPassCreateInfo* pCreateInfo) override;
    virtual void   Destroy() override;
    friend class grfx::Device;

private:
    Result CreateImagesAndViewsV1(const grfx::internal::RenderPassCreateInfo* pCreateInfo);
    Result CreateImagesAndViewsV2(const grfx::internal::RenderPassCreateInfo* pCreateInfo);
    Result CreateImagesAndViewsV3(const grfx::internal::RenderPassCreateInfo* pCreateInfo);

protected:
    template <typename ObjectPtrT>
    struct ExtObjPtr
    {
        bool       isExternal = true;
        ObjectPtrT object     = nullptr;
    };

    grfx::Rect                                        mRenderArea = {};
    std::vector<ExtObjPtr<grfx::RenderTargetViewPtr>> mRenderTargetViews;
    ExtObjPtr<grfx::DepthStencilViewPtr>              mDepthStencilView;
    std::vector<ExtObjPtr<grfx::ImagePtr>>            mRenderTargetImages;
    ExtObjPtr<grfx::ImagePtr>                         mDepthStencilImage;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_render_pass_h