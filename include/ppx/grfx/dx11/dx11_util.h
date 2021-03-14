#ifndef ppx_grfx_dx11_util_h
#define ppx_grfx_dx11_util_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_format.h"
#include "ppx/grfx/dx/dx_util.h"

#include <d3d11.h>

namespace ppx {
namespace grfx {
namespace dx11 {

UINT                     ToD3D11BindFlags(const grfx::BufferUsageFlags& value);
D3D11_CULL_MODE          ToD3D11CullMode(grfx::CullMode value);
D3D11_FILL_MODE          ToD3D11FillMode(grfx::PolygonMode value);
DXGI_FORMAT              ToD3D11IndexFormat(grfx::IndexType value);
D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimitiveTopology(grfx::PrimitiveTopology value);
D3D11_RTV_DIMENSION      ToD3D11RTVDimension(grfx::ImageViewType value);
D3D11_RESOURCE_DIMENSION ToD3D11TextureResourceDimension(grfx::ImageType value);
D3D11_USAGE              ToD3D11Usage(grfx::MemoryUsage value);

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_util_h
