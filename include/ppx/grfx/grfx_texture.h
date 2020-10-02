#ifndef ppx_grfx_texture_h
#define ppx_grfx_texture_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct TextureCreateInfo
//!
//!
struct TextureCreateInfo
{
    bool usageDepthStencil = false;
    bool usageRenderTarget = false;
    bool usageSampeldImage = false;
    bool usageStorageImage = false;
    bool usageCopySrc      = false;
    bool usageCopyDst      = false;
};

//! @class Texture
//!
//!
class Texture
    : public grfx::DeviceObject<grfx::TextureCreateInfo>
{
public:
    Texture() {}
    virtual ~Texture() {}

    grfx::ImagePtr            GetImage() const { return mImage; }
    grfx::DepthStencilViewPtr GetDepthStencilView() const { return mDepthStencilView; }
    grfx::RenderTargetViewPtr GetRenderTargetView() const { return mRenderTargetView; }
    grfx::SampledImageViewPtr GetSampledImageView() const { return mSampledImageView; }
    grfx::StorageImageViewPtr GetStorageImageView() const { return mStorageImageView; }

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetDepth() const;

protected:
    virtual Result CreateApiObjects(const grfx::TextureCreateInfo* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                                          = 0;
    friend class grfx::Device;

private:
    grfx::TextureCreateInfo   mCreateInfo = {};
    grfx::ImagePtr            mImage;
    grfx::DepthStencilViewPtr mDepthStencilView;
    grfx::RenderTargetViewPtr mRenderTargetView;
    grfx::SampledImageViewPtr mSampledImageView;
    grfx::StorageImageViewPtr mStorageImageView;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_texture_h
