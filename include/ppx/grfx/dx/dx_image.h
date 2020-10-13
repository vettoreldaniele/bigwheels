#ifndef ppx_grfx_dx_image_h
#define ppx_grfx_dx_image_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/dx/dx_descriptor_helper.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {
namespace dx {

class Image
    : public grfx::Image
{
public:
    Image() {}
    virtual ~Image() {}

    D3D12ResourcePtr GetDxImage() const { return mResource; }

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) override;
    virtual void   UnmapMemory() override;

protected:
    virtual Result CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12ResourcePtr mResource;
};

// -------------------------------------------------------------------------------------------------

class Sampler
    : public grfx::Sampler
{
public:
    Sampler() {}
    virtual ~Sampler() {}

protected:
    virtual Result CreateApiObjects(const grfx::SamplerCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
};

// -------------------------------------------------------------------------------------------------

class DepthStencilView
    : public grfx::DepthStencilView
{
public:
    DepthStencilView() {}
    virtual ~DepthStencilView() {}

protected:
    virtual Result CreateApiObjects(const grfx::DepthStencilViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
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
    dx::DescriptorHandle mDescriptor = {};
};

// -------------------------------------------------------------------------------------------------

class SampledImageView
    : public grfx::SampledImageView
{
public:
    SampledImageView() {}
    virtual ~SampledImageView() {}

protected:
    virtual Result CreateApiObjects(const grfx::SampledImageViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
};

// -------------------------------------------------------------------------------------------------

class StorageImageView
    : public grfx::StorageImageView
{
public:
    StorageImageView() {}
    virtual ~StorageImageView() {}

protected:
    virtual Result CreateApiObjects(const grfx::StorageImageViewCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_image_h
