#ifndef ppx_grfx_dx_image_h
#define ppx_grfx_dx_image_h

#include "ppx/grfx/dx12/000_dx12_config.h"
#include "ppx/grfx/dx12/dx12_descriptor_helper.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {
namespace dx {

class Image
    : public grfx::Image
{
public:
    Image() {}
    virtual ~Image();

    typename D3D12ResourcePtr::InterfaceType* GetDxResource() const { return mResource.Get(); }

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) override;
    virtual void   UnmapMemory() override;

protected:
    virtual Result CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12ResourcePtr            mResource;
    ObjPtr<D3D12MA::Allocation> mAllocation;
};

// -------------------------------------------------------------------------------------------------

class Sampler
    : public grfx::Sampler
{
public:
    Sampler() {}
    virtual ~Sampler() {}

    const D3D12_SAMPLER_DESC& GetDesc() const { return mDesc; }

protected:
    virtual Result CreateApiObjects(const grfx::SamplerCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12_SAMPLER_DESC mDesc = {};
};

// -------------------------------------------------------------------------------------------------

class DepthStencilView
    : public grfx::DepthStencilView
{
public:
    DepthStencilView() {}
    virtual ~DepthStencilView() {}

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle() const { return mDescriptor.handle; }

protected:
    virtual Result CreateApiObjects(const grfx::DepthStencilViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12_DEPTH_STENCIL_VIEW_DESC mDesc       = {};
    dx::DescriptorHandle          mDescriptor = {};
};

// -------------------------------------------------------------------------------------------------

class RenderTargetView
    : public grfx::RenderTargetView
{
public:
    RenderTargetView() {}
    virtual ~RenderTargetView() {}

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle() const { return mDescriptor.handle; }

protected:
    virtual Result CreateApiObjects(const grfx::RenderTargetViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12_RENDER_TARGET_VIEW_DESC mDesc       = {};
    dx::DescriptorHandle          mDescriptor = {};
};

// -------------------------------------------------------------------------------------------------

class SampledImageView
    : public grfx::SampledImageView
{
public:
    SampledImageView() {}
    virtual ~SampledImageView() {}

    const D3D12_SHADER_RESOURCE_VIEW_DESC& GetDesc() const { return mDesc; }

protected:
    virtual Result CreateApiObjects(const grfx::SampledImageViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12_SHADER_RESOURCE_VIEW_DESC mDesc = {};
};

// -------------------------------------------------------------------------------------------------

class StorageImageView
    : public grfx::StorageImageView
{
public:
    StorageImageView() {}
    virtual ~StorageImageView() {}

    const D3D12_UNORDERED_ACCESS_VIEW_DESC& GetDesc() const { return mDesc; }

protected:
    virtual Result CreateApiObjects(const grfx::StorageImageViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12_UNORDERED_ACCESS_VIEW_DESC mDesc = {};
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_image_h
