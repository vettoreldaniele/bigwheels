#include "ppx/grfx/dx11/dx11_image.h"
#include "ppx/grfx/dx11/dx11_device.h"

namespace ppx {
namespace grfx {
namespace dx11 {

// -------------------------------------------------------------------------------------------------
// Image
// -------------------------------------------------------------------------------------------------
Result Image::CreateApiObjects(const grfx::ImageCreateInfo* pCreateInfo)
{
    if (IsNull(pCreateInfo->pApiObject)) {
    }
    else {
        switch (pCreateInfo->type) {
            default: break;
            case grfx::IMAGE_TYPE_1D: {
                mTexture1D = static_cast<typename D3D11Texture1DPtr::InterfaceType*>(pCreateInfo->pApiObject);
            } break;
            case grfx::IMAGE_TYPE_2D: {
                mTexture2D = static_cast<typename D3D11Texture2DPtr::InterfaceType*>(pCreateInfo->pApiObject);
            } break;
            case grfx::IMAGE_TYPE_3D: {
                mTexture3D = static_cast<typename D3D11Texture3DPtr::InterfaceType*>(pCreateInfo->pApiObject);
            } break;
        }
    }

    return ppx::SUCCESS;
}

void Image::DestroyApiObjects()
{
    // Reset if resource isn't external
    if (IsNull(mCreateInfo.pApiObject)) {
        if (mTexture1D) {
            mTexture1D.Reset();
        }

        if (mTexture2D) {
            mTexture2D.Reset();
        }

        if (mTexture3D) {
            mTexture3D.Reset();
        }
    }
    else {
        // Deatch if the resource is external

        if (mTexture1D) {
            mTexture1D.Detach();
        }

        if (mTexture2D) {
            mTexture2D.Detach();
        }

        if (mTexture3D) {
            mTexture3D.Detach();
        }
    }
}

Result Image::MapMemory(uint64_t offset, void** ppMappedAddress)
{
    PPX_ASSERT_MSG(false, "memory mapping of textures is not availalble in D3D11");
    return ppx::ERROR_FAILED;
}

void Image::UnmapMemory()
{
    PPX_ASSERT_MSG(false, "memory mapping of textures is not availalble in D3D11");
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
// RenderTargetView
// -------------------------------------------------------------------------------------------------
Result RenderTargetView::CreateApiObjects(const grfx::RenderTargetViewCreateInfo* pCreateInfo)
{
    D3D11DevicePtr device = ToApi(GetDevice())->GetDxDevice();

    ID3D11Resource* pResource = nullptr;
    switch (pCreateInfo->pImage->GetType()) {
        default: break;
        case grfx::IMAGE_TYPE_1D: {
            pResource = ToApi(pCreateInfo->pImage)->GetDxTexture1D();
        } break;
        case grfx::IMAGE_TYPE_2D: {
            pResource = ToApi(pCreateInfo->pImage)->GetDxTexture2D();
        } break;
        case grfx::IMAGE_TYPE_3D: {
            pResource = ToApi(pCreateInfo->pImage)->GetDxTexture3D();
        } break;
    }

    D3D11_RENDER_TARGET_VIEW_DESC1 desc = {};
    desc.Format                         = dx::ToDxgiFormat(pCreateInfo->format);
    desc.ViewDimension                  = ToD3D11RTVDimension(pCreateInfo->imageViewType);

    switch (desc.ViewDimension) {
        default: {
            PPX_ASSERT_MSG(false, "unknown RTV dimension");
        } break;

        case D3D11_RTV_DIMENSION_TEXTURE1D: {
            desc.Texture1D.MipSlice = static_cast<UINT>(pCreateInfo->mipLevel);
        } break;

        case D3D11_RTV_DIMENSION_TEXTURE1DARRAY: {
            desc.Texture1DArray.MipSlice        = static_cast<UINT>(pCreateInfo->mipLevel);
            desc.Texture1DArray.FirstArraySlice = static_cast<UINT>(pCreateInfo->arrayLayer);
            desc.Texture1DArray.ArraySize       = static_cast<UINT>(pCreateInfo->arrayLayerCount);
        } break;

        case D3D11_RTV_DIMENSION_TEXTURE2D: {
            desc.Texture2D.MipSlice = static_cast<UINT>(pCreateInfo->mipLevel);
        } break;

        case D3D11_RTV_DIMENSION_TEXTURE2DARRAY: {
            desc.Texture2DArray.MipSlice        = static_cast<UINT>(pCreateInfo->mipLevel);
            desc.Texture2DArray.FirstArraySlice = static_cast<UINT>(pCreateInfo->arrayLayer);
            desc.Texture2DArray.ArraySize       = static_cast<UINT>(pCreateInfo->arrayLayerCount);
        } break;

        case D3D11_RTV_DIMENSION_TEXTURE3D: {
            desc.Texture3D.MipSlice    = static_cast<UINT>(pCreateInfo->mipLevel);
            desc.Texture3D.FirstWSlice = static_cast<UINT>(pCreateInfo->arrayLayer);
            desc.Texture3D.WSize       = static_cast<UINT>(pCreateInfo->arrayLayerCount);
        } break;
    }

    HRESULT hr = device->CreateRenderTargetView1(pResource, &desc, &mRenderTargetView);
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void RenderTargetView::DestroyApiObjects()
{
    if (mRenderTargetView) {
        mRenderTargetView.Reset();
    }
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
