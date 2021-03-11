#include "ppx/grfx/dx11/dx11_util.h"

namespace ppx {
namespace grfx {
namespace dx11{

D3D11_RTV_DIMENSION ToD3D11RTVDimension(grfx::ImageViewType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_VIEW_TYPE_1D       : return D3D11_RTV_DIMENSION_TEXTURE1D; break;
        case grfx::IMAGE_VIEW_TYPE_1D_ARRAY : return D3D11_RTV_DIMENSION_TEXTURE1DARRAY; break;
        case grfx::IMAGE_VIEW_TYPE_2D       : return D3D11_RTV_DIMENSION_TEXTURE2D; break;
        case grfx::IMAGE_VIEW_TYPE_2D_ARRAY : return D3D11_RTV_DIMENSION_TEXTURE2DARRAY; break;
        case grfx::IMAGE_VIEW_TYPE_3D       : return D3D11_RTV_DIMENSION_TEXTURE3D; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D11_RTV_DIMENSION>();
}

D3D11_RESOURCE_DIMENSION ToD3D11TextureResourceDimension(grfx::ImageType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_TYPE_1D   : return D3D11_RESOURCE_DIMENSION_TEXTURE1D; break;
        case grfx::IMAGE_TYPE_2D   : return D3D11_RESOURCE_DIMENSION_TEXTURE2D; break;
        case grfx::IMAGE_TYPE_3D   : return D3D11_RESOURCE_DIMENSION_TEXTURE3D; break;
        case grfx::IMAGE_TYPE_CUBE : return D3D11_RESOURCE_DIMENSION_TEXTURE2D; break;
    }
    // clang-format on
    return D3D11_RESOURCE_DIMENSION_UNKNOWN;
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
