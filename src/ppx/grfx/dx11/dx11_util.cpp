#include "ppx/grfx/dx11/dx11_util.h"

namespace ppx {
namespace grfx {
namespace dx11 {

UINT ToD3D11BindFlags(const grfx::BufferUsageFlags& value)
{
    UINT flags = 0;
    if (value.bits.uniformBuffer) {
        flags |= D3D11_BIND_CONSTANT_BUFFER;
    }
    if (value.bits.storageBuffer) {
        flags |= D3D11_BIND_UNORDERED_ACCESS;
    }
    if (value.bits.structuredBuffer) {
        flags |= D3D11_BIND_SHADER_RESOURCE;
    }
    if (value.bits.indexBuffer) {
        flags |= D3D11_BIND_INDEX_BUFFER;
    }
    if (value.bits.vertexBuffer) {
        flags |= D3D11_BIND_VERTEX_BUFFER;
    }
    return flags;
}

D3D11_CULL_MODE ToD3D11CullMode(grfx::CullMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::CULL_MODE_NONE  : return D3D11_CULL_NONE; break;
        case grfx::CULL_MODE_FRONT : return D3D11_CULL_FRONT; break;
        case grfx::CULL_MODE_BACK  : return D3D11_CULL_BACK; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D11_CULL_MODE>();
}

D3D11_FILL_MODE ToD3D11FillMode(grfx::PolygonMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case POLYGON_MODE_FILL  : return D3D11_FILL_SOLID; break;
        case POLYGON_MODE_LINE  : return D3D11_FILL_WIREFRAME; break;
        case POLYGON_MODE_POINT : break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D11_FILL_MODE>();
}

DXGI_FORMAT ToD3D11IndexFormat(grfx::IndexType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::INDEX_TYPE_UINT16 : return DXGI_FORMAT_R16_UINT; break;
        case grfx::INDEX_TYPE_UINT32 : return DXGI_FORMAT_R32_UINT; break;
    }
    // clang-format on
    return DXGI_FORMAT_UNKNOWN;
}

D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimitiveTopology(grfx::PrimitiveTopology value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST  : return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_FAN   : break;
        case grfx::PRIMITIVE_TOPOLOGY_POINT_LIST     : return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case grfx::PRIMITIVE_TOPOLOGY_LINE_LIST      : return D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case grfx::PRIMITIVE_TOPOLOGY_LINE_STRIP     : return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case grfx::PRIMITIVE_TOPOLOGY_PATCH_LIST     : break;
    }
    // clang-format on
    return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

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

D3D11_USAGE ToD3D11Usage(grfx::MemoryUsage value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::MEMORY_USAGE_GPU_ONLY   : return D3D11_USAGE_DEFAULT; break;
        case grfx::MEMORY_USAGE_CPU_ONLY   : return D3D11_USAGE_STAGING; break;
        case grfx::MEMORY_USAGE_CPU_TO_GPU : return D3D11_USAGE_DYNAMIC; break;
        case grfx::MEMORY_USAGE_GPU_TO_CPU : return D3D11_USAGE_STAGING; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D11_USAGE>();
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
