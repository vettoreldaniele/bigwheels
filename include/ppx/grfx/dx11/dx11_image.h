#ifndef ppx_grfx_dx11_image_h
#define ppx_grfx_dx11_image_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class Image
    : public grfx::Image
{
public:
    Image() {}
    virtual ~Image() {}

    typename D3D11Texture1DPtr::InterfaceType* GetDxTexture1D() const { return mTexture1D.Get(); }
    typename D3D11Texture2DPtr::InterfaceType* GetDxTexture2D() const { return mTexture2D.Get(); }
    typename D3D11Texture3DPtr::InterfaceType* GetDxTexture3D() const { return mTexture3D.Get(); }

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) override;
    virtual void   UnmapMemory() override;

protected:
    virtual Result CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D11Texture1DPtr mTexture1D;
    D3D11Texture2DPtr mTexture2D;
    D3D11Texture3DPtr mTexture3D;
};

// -------------------------------------------------------------------------------------------------

class Sampler
    : public grfx::Sampler
{
public:
    Sampler() {}
    virtual ~Sampler() {}

    typename D3D11SamplerStatePtr::InterfaceType* GetDxSampler() const { return mSamplerState.Get(); }

protected:
    virtual Result CreateApiObjects(const grfx::SamplerCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D11SamplerStatePtr mSamplerState;
};

// -------------------------------------------------------------------------------------------------

//class DepthStencilView
//    : public grfx::DepthStencilView
//{
//public:
//    DepthStencilView() {}
//    virtual ~DepthStencilView() {}
//
//    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle() const { return mDescriptor.handle; }
//
//protected:
//    virtual Result CreateApiObjects(const grfx::DepthStencilViewCreateInfo* pCreateInfo) override;
//    virtual void   DestroyApiObjects() override;
//
//private:
//    D3D12_DEPTH_STENCIL_VIEW_DESC mDesc       = {};
//    dx12::DescriptorHandle        mDescriptor = {};
//};

// -------------------------------------------------------------------------------------------------

class RenderTargetView
    : public grfx::RenderTargetView
{
public:
    RenderTargetView() {}
    virtual ~RenderTargetView() {}

    typename D3D11RenderTargetViewPtr::InterfaceType* GetDxRenderTargetView() const { return mRenderTargetView.Get(); }

protected:
    virtual Result CreateApiObjects(const grfx::RenderTargetViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D11RenderTargetViewPtr mRenderTargetView;
};

//// -------------------------------------------------------------------------------------------------
//
//class SampledImageView
//    : public grfx::SampledImageView
//{
//public:
//    SampledImageView() {}
//    virtual ~SampledImageView() {}
//
//    const D3D12_SHADER_RESOURCE_VIEW_DESC& GetDesc() const { return mDesc; }
//
//protected:
//    virtual Result CreateApiObjects(const grfx::SampledImageViewCreateInfo* pCreateInfo) override;
//    virtual void   DestroyApiObjects() override;
//
//private:
//    D3D12_SHADER_RESOURCE_VIEW_DESC mDesc = {};
//};
//
//// -------------------------------------------------------------------------------------------------
//
//class StorageImageView
//    : public grfx::StorageImageView
//{
//public:
//    StorageImageView() {}
//    virtual ~StorageImageView() {}
//
//    const D3D12_UNORDERED_ACCESS_VIEW_DESC& GetDesc() const { return mDesc; }
//
//protected:
//    virtual Result CreateApiObjects(const grfx::StorageImageViewCreateInfo* pCreateInfo) override;
//    virtual void   DestroyApiObjects() override;
//
//private:
//    D3D12_UNORDERED_ACCESS_VIEW_DESC mDesc = {};
//};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_image_h
