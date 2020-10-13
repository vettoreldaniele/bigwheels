#ifndef ppx_grfx_dx_util_h
#define ppx_grfx_dx_util_h

#include "ppx/grfx/000_grfx_config.h"
#include "ppx/grfx/grfx_format.h"

#include <d3d12.h>
#include <dxgiformat.h>

namespace ppx {
namespace grfx {
namespace dx {

D3D12_BLEND                   ToD3D12Blend(grfx::BlendFactor value);
D3D12_BLEND_OP                ToD3D12BlendOp(grfx::BlendOp value);
D3D12_COMPARISON_FUNC         ToD3D12ComparisonFunc(grfx::CompareOp value);
D3D12_CULL_MODE               ToD3D12CullMode(grfx::CullMode value);
D3D12_FILL_MODE               ToD3D12FillMode(grfx::PolygonMode value);
D3D12_HEAP_TYPE               ToD3D12HeapType(grfx::MemoryUsage value);
D3D12_LOGIC_OP                ToD3D12LogicOp(grfx::LogicOp value);
D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12PrimitiveTopology(grfx::PrimitiveTopology value);
D3D12_RESOURCE_STATES         ToD3D12ResourceStates(grfx::ResourceState value);
D3D12_RTV_DIMENSION           ToD3D12RTVDimension(grfx::ImageViewType value);
D3D12_SHADER_VISIBILITY       ToD3D12ShaderVisibliity(grfx::ShaderStageBits value);
D3D12_STENCIL_OP              ToD3D12StencilOp(grfx::StencilOp value);
UINT8                         ToD3D12WriteMask(uint32_t value);

DXGI_FORMAT ToDxgiFormat(grfx::Format value);

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_util_h
