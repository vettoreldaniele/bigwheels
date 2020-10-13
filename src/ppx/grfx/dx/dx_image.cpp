#include "ppx/grfx/dx/dx_image.h"
#include "ppx/grfx/dx/dx_device.h"

namespace ppx {
namespace grfx {
namespace dx {

// -------------------------------------------------------------------------------------------------
// Image
// -------------------------------------------------------------------------------------------------
Result Image::CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo)
{
    // NOTE: D3D12 does not support mapping of texture resources without the
    //       use of the a custom heap. No plans to make this work currently.
    //       All texture resources must be GPU only.
    //
    if (pCreateInfo->memoryUsage != grfx::MEMORY_USAGE_GPU_ONLY) {
        PPX_ASSERT_MSG(false, "memory mapping of textures is not availalble in D3D12");
        return ppx::ERROR_INVALID_CREATE_ARGUMENT;
    }

    if (IsNull(pCreateInfo->pApiObject)) {
    }
    else {
        ComPtr<ID3D12Resource> resource = static_cast<ID3D12Resource*>(pCreateInfo->pApiObject);
        HRESULT                hr       = resource.As(&mResource);
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "failed casting to ID3D12Resource");
            return ppx::ERROR_API_FAILURE;
        }
    }

    return ppx::SUCCESS;
}

void Image::DestroyApiObjects()
{
    if (mResource) {
    }
}

Result Image::MapMemory(uint64_t offset, void** ppMappedAddress)
{
    PPX_ASSERT_MSG(false, "memory mapping of textures is not availalble in D3D12");
    return ppx::ERROR_FAILED;
}

void Image::UnmapMemory()
{
    PPX_ASSERT_MSG(false, "memory mapping of textures is not availalble in D3D12");
}

// -------------------------------------------------------------------------------------------------
// Sampler
// -------------------------------------------------------------------------------------------------
Result Sampler::CreateApiObjects(const grfx::SamplerCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void Sampler::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// DepthStencilView
// -------------------------------------------------------------------------------------------------
Result DepthStencilView::CreateApiObjects(const grfx::DepthStencilViewCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void DepthStencilView::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// RenderTargetView
// -------------------------------------------------------------------------------------------------
Result RenderTargetView::CreateApiObjects(const grfx::RenderTargetViewCreateInfo* pCreateInfo)
{
    Result ppxres = ToApi(GetDevice())->AllocateRTVHandle(&mDescriptor);
    if (Failed(ppxres)) {
        PPX_ASSERT_MSG(false, "dx::Device::AllocateRTVHandle failed");
        return ppxres;
    }

    D3D12_RENDER_TARGET_VIEW_DESC desc = {};
    desc.Format                        = ToDxgiFormat(pCreateInfo->format);
    desc.ViewDimension                 = ToD3D12RTVDimension(pCreateInfo->imageViewType);

    switch (pCreateInfo->imageViewType) {
        default: break;

        case grfx::IMAGE_VIEW_TYPE_1D: {
            desc.Texture1D.MipSlice = static_cast<UINT>(pCreateInfo->mipLevel);
        } break;

        case grfx::IMAGE_VIEW_TYPE_2D: {
            desc.Texture2D.MipSlice = static_cast<UINT>(pCreateInfo->mipLevel);
        } break;

        case grfx::IMAGE_VIEW_TYPE_1D_ARRAY: {
            desc.Texture1DArray.MipSlice        = static_cast<UINT>(pCreateInfo->mipLevel);
            desc.Texture1DArray.FirstArraySlice = static_cast<UINT>(pCreateInfo->arrayLayer);
            desc.Texture1DArray.ArraySize       = static_cast<UINT>(pCreateInfo->arrayLayerCount);
        } break;

        case grfx::IMAGE_VIEW_TYPE_2D_ARRAY:
        case grfx::IMAGE_VIEW_TYPE_CUBE_ARRAY: {
            desc.Texture2DArray.MipSlice        = static_cast<UINT>(pCreateInfo->mipLevel);
            desc.Texture2DArray.FirstArraySlice = static_cast<UINT>(pCreateInfo->arrayLayer);
            desc.Texture2DArray.ArraySize       = static_cast<UINT>(pCreateInfo->arrayLayerCount);
        } break;

        case grfx::IMAGE_VIEW_TYPE_3D: {
            desc.Texture3D.MipSlice    = static_cast<UINT>(pCreateInfo->mipLevel);
            desc.Texture3D.FirstWSlice = static_cast<UINT>(pCreateInfo->arrayLayer);
            desc.Texture3D.WSize       = static_cast<UINT>(pCreateInfo->arrayLayerCount);
        } break;
    }

    D3D12DevicePtr device = ToApi(GetDevice())->GetDxDevice();
    device->CreateRenderTargetView(
        ToApi(pCreateInfo->pImage)->GetDxImage().Get(),
        &desc,
        mDescriptor.handle);

    return ppx::SUCCESS;
}

void RenderTargetView::DestroyApiObjects()
{
    if (mDescriptor) {
        ToApi(GetDevice())->FreeRTVHandle(&mDescriptor);
        mDescriptor.Reset();
    }
}

// -------------------------------------------------------------------------------------------------
// SampledImageView
// -------------------------------------------------------------------------------------------------
Result SampledImageView::CreateApiObjects(const grfx::SampledImageViewCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void SampledImageView::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// StorageImageView
// -------------------------------------------------------------------------------------------------
Result StorageImageView::CreateApiObjects(const grfx::StorageImageViewCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void StorageImageView::DestroyApiObjects()
{
}

} // namespace dx
} // namespace grfx
} // namespace ppx
