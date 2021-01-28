#include "ppx/grfx/dx/dx_util.h"

namespace ppx {
namespace grfx {
namespace dx {

D3D12_BLEND ToD3D12Blend(grfx::BlendFactor value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::BLEND_FACTOR_ZERO                     : return D3D12_BLEND_ZERO; break;
        case grfx::BLEND_FACTOR_ONE                      : return D3D12_BLEND_ONE; break;
        case grfx::BLEND_FACTOR_SRC_COLOR                : return D3D12_BLEND_SRC_COLOR; break;
        case grfx::BLEND_FACTOR_ONE_MINUS_SRC_COLOR      : return D3D12_BLEND_INV_SRC_COLOR; break;
        case grfx::BLEND_FACTOR_SRC_ALPHA                : return D3D12_BLEND_SRC_ALPHA; break;
        case grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA      : return D3D12_BLEND_INV_SRC_ALPHA; break;
        case grfx::BLEND_FACTOR_DST_ALPHA                : return D3D12_BLEND_DEST_ALPHA; break;
        case grfx::BLEND_FACTOR_ONE_MINUS_DST_ALPHA      : return D3D12_BLEND_INV_DEST_ALPHA; break;
        case grfx::BLEND_FACTOR_DST_COLOR                : return D3D12_BLEND_DEST_COLOR; break;
        case grfx::BLEND_FACTOR_ONE_MINUS_DST_COLOR      : return D3D12_BLEND_INV_DEST_COLOR; break;
        case grfx::BLEND_FACTOR_SRC_ALPHA_SATURATE       : return D3D12_BLEND_SRC_ALPHA_SAT; break;
        case grfx::BLEND_FACTOR_CONSTANT_COLOR           : return D3D12_BLEND_BLEND_FACTOR; break;     // D3D12 does not have constants
        case grfx::BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR : return D3D12_BLEND_INV_BLEND_FACTOR; break; // for color and alpha blend
        case grfx::BLEND_FACTOR_CONSTANT_ALPHA           : return D3D12_BLEND_BLEND_FACTOR; break;     // constants. 
        case grfx::BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA : return D3D12_BLEND_INV_BLEND_FACTOR; break; //
        case grfx::BLEND_FACTOR_SRC1_COLOR               : return D3D12_BLEND_SRC1_COLOR; break;
        case grfx::BLEND_FACTOR_ONE_MINUS_SRC1_COLOR     : return D3D12_BLEND_INV_SRC1_COLOR; break;
        case grfx::BLEND_FACTOR_SRC1_ALPHA               : return D3D12_BLEND_SRC1_ALPHA; break;
        case grfx::BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA     : return D3D12_BLEND_INV_SRC1_ALPHA; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_BLEND>();
}

D3D12_BLEND_OP ToD3D12BlendOp(grfx::BlendOp value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::BLEND_OP_ADD              : return D3D12_BLEND_OP_ADD; break;
        case grfx::BLEND_OP_SUBTRACT         : return D3D12_BLEND_OP_SUBTRACT; break;
        case grfx::BLEND_OP_REVERSE_SUBTRACT : return D3D12_BLEND_OP_REV_SUBTRACT; break;
        case grfx::BLEND_OP_MIN              : return D3D12_BLEND_OP_MIN; break;
        case grfx::BLEND_OP_MAX              : return D3D12_BLEND_OP_MAX; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_BLEND_OP>();
}

D3D12_COMPARISON_FUNC ToD3D12ComparisonFunc(grfx::CompareOp value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::COMPARE_OP_NEVER            : return D3D12_COMPARISON_FUNC_NEVER; break;
        case grfx::COMPARE_OP_LESS             : return D3D12_COMPARISON_FUNC_LESS; break;
        case grfx::COMPARE_OP_EQUAL            : return D3D12_COMPARISON_FUNC_EQUAL; break;
        case grfx::COMPARE_OP_LESS_OR_EQUAL    : return D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
        case grfx::COMPARE_OP_GREATER          : return D3D12_COMPARISON_FUNC_GREATER; break;
        case grfx::COMPARE_OP_NOT_EQUAL        : return D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
        case grfx::COMPARE_OP_GREATER_OR_EQUAL : return D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
        case grfx::COMPARE_OP_ALWAYS           : return D3D12_COMPARISON_FUNC_ALWAYS; break;      
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_COMPARISON_FUNC>();
}

D3D12_CULL_MODE ToD3D12CullMode(grfx::CullMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::CULL_MODE_NONE  : return D3D12_CULL_MODE_NONE; break;
        case grfx::CULL_MODE_FRONT : return D3D12_CULL_MODE_FRONT; break;
        case grfx::CULL_MODE_BACK  : return D3D12_CULL_MODE_BACK; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_CULL_MODE>();
}

D3D12_DSV_DIMENSION ToD3D12DSVDimension(grfx::ImageViewType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_VIEW_TYPE_1D       : return D3D12_DSV_DIMENSION_TEXTURE1D; break;
        case grfx::IMAGE_VIEW_TYPE_1D_ARRAY : return D3D12_DSV_DIMENSION_TEXTURE1DARRAY; break;
        case grfx::IMAGE_VIEW_TYPE_2D       : return D3D12_DSV_DIMENSION_TEXTURE2D; break;
        case grfx::IMAGE_VIEW_TYPE_2D_ARRAY : return D3D12_DSV_DIMENSION_TEXTURE2DARRAY; break;
    }
    // clang-format on
    return D3D12_DSV_DIMENSION_UNKNOWN;
}

D3D12_FILL_MODE ToD3D12FillMode(grfx::PolygonMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case POLYGON_MODE_FILL  : return D3D12_FILL_MODE_SOLID; break;
        case POLYGON_MODE_LINE  : return D3D12_FILL_MODE_WIREFRAME; break;
        case POLYGON_MODE_POINT : break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_FILL_MODE>();
}

D3D12_FILTER_TYPE ToD3D12FilterType(grfx::Filter value)
{
    // clang-format off
    switch (value) {
        default: break;
        case FILTER_NEAREST : return D3D12_FILTER_TYPE_POINT; break;
        case FILTER_LINEAR  : return D3D12_FILTER_TYPE_LINEAR; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_FILTER_TYPE>();
}

D3D12_FILTER_TYPE ToD3D12FilterType(grfx::SamplerMipmapMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case SAMPLER_MIPMAP_MODE_NEAREST : return D3D12_FILTER_TYPE_POINT; break;
        case SAMPLER_MIPMAP_MODE_LINEAR  : return D3D12_FILTER_TYPE_LINEAR; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_FILTER_TYPE>();
}

D3D12_HEAP_TYPE ToD3D12HeapType(grfx::MemoryUsage value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::MEMORY_USAGE_GPU_ONLY   : return D3D12_HEAP_TYPE_DEFAULT; break;
        case grfx::MEMORY_USAGE_CPU_ONLY   : return D3D12_HEAP_TYPE_UPLOAD; break;
        case grfx::MEMORY_USAGE_CPU_TO_GPU : return D3D12_HEAP_TYPE_UPLOAD; break;
        case grfx::MEMORY_USAGE_GPU_TO_CPU : return D3D12_HEAP_TYPE_READBACK; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_HEAP_TYPE>();
}

DXGI_FORMAT ToD3D12IndexFormat(grfx::IndexType value)
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

D3D12_LOGIC_OP ToD3D12LogicOp(grfx::LogicOp value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::LOGIC_OP_CLEAR         : return D3D12_LOGIC_OP_CLEAR; break;
        case grfx::LOGIC_OP_SET           : return D3D12_LOGIC_OP_SET; break;
        case grfx::LOGIC_OP_COPY          : return D3D12_LOGIC_OP_COPY; break;
        case grfx::LOGIC_OP_COPY_INVERTED : return D3D12_LOGIC_OP_COPY_INVERTED; break;
        case grfx::LOGIC_OP_NO_OP         : return D3D12_LOGIC_OP_NOOP; break;
        case grfx::LOGIC_OP_INVERT        : return D3D12_LOGIC_OP_INVERT; break;
        case grfx::LOGIC_OP_AND           : return D3D12_LOGIC_OP_AND; break;
        case grfx::LOGIC_OP_NAND          : return D3D12_LOGIC_OP_NAND; break;
        case grfx::LOGIC_OP_OR            : return D3D12_LOGIC_OP_OR; break;
        case grfx::LOGIC_OP_NOR           : return D3D12_LOGIC_OP_NOR; break;
        case grfx::LOGIC_OP_XOR           : return D3D12_LOGIC_OP_XOR; break;
        case grfx::LOGIC_OP_EQUIVALENT    : return D3D12_LOGIC_OP_EQUIV; break;
        case grfx::LOGIC_OP_AND_REVERSE   : return D3D12_LOGIC_OP_AND_REVERSE; break;
        case grfx::LOGIC_OP_AND_INVERTED  : return D3D12_LOGIC_OP_AND_INVERTED; break;
        case grfx::LOGIC_OP_OR_REVERSE    : return D3D12_LOGIC_OP_OR_REVERSE; break;
        case grfx::LOGIC_OP_OR_INVERTED   : return D3D12_LOGIC_OP_OR_INVERTED; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_LOGIC_OP>();
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12PrimitiveTopology(grfx::PrimitiveTopology value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST  : return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_FAN   : break;
        case grfx::PRIMITIVE_TOPOLOGY_POINT_LIST     : return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT; break;
        case grfx::PRIMITIVE_TOPOLOGY_LINE_LIST      : return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
        case grfx::PRIMITIVE_TOPOLOGY_LINE_STRIP     : break;
        case grfx::PRIMITIVE_TOPOLOGY_PATCH_LIST     : return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH; break;
    }
    // clang-format on
    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
}

D3D12_QUERY_TYPE ToD3D12QueryType(grfx::QueryType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::QUERY_TYPE_OCCLUSION           : return D3D12_QUERY_TYPE_OCCLUSION; break;
        case grfx::QUERY_TYPE_TIMESTAMP           : return D3D12_QUERY_TYPE_TIMESTAMP; break;
        case grfx::QUERY_TYPE_PIPELINE_STATISTICS : return D3D12_QUERY_TYPE_PIPELINE_STATISTICS; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_QUERY_TYPE>();
}

D3D12_QUERY_HEAP_TYPE ToD3D12QueryHeapType(grfx::QueryType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::QUERY_TYPE_OCCLUSION           : return D3D12_QUERY_HEAP_TYPE_OCCLUSION; break;
        case grfx::QUERY_TYPE_TIMESTAMP           : return D3D12_QUERY_HEAP_TYPE_TIMESTAMP; break;
        case grfx::QUERY_TYPE_PIPELINE_STATISTICS : return D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_QUERY_HEAP_TYPE>();
}

D3D12_DESCRIPTOR_RANGE_TYPE ToD3D12RangeType(grfx::DescriptorType value)
{
    switch (value) {
        default: break;
        case grfx::DESCRIPTOR_TYPE_SAMPLER: {
            return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        } break;

        case grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        case grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER: {
            return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        } break;

        case grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER: {
            return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        } break;

        case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        } break;
    }
    return ppx::InvalidValue<D3D12_DESCRIPTOR_RANGE_TYPE>();
}

D3D12_RESOURCE_STATES ToD3D12ResourceStates(grfx::ResourceState value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::RESOURCE_STATE_UNDEFINED                 : return D3D12_RESOURCE_STATE_COMMON; break;
        case grfx::RESOURCE_STATE_GENERAL                   : return D3D12_RESOURCE_STATE_COMMON; break;
        case grfx::RESOURCE_STATE_CONSTANT_BUFFER           : return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER; break;
        case grfx::RESOURCE_STATE_VERTEX_BUFFER             : return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER; break;
        case grfx::RESOURCE_STATE_INDEX_BUFFER              : return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER; break;
        case grfx::RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE : return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE; break;
        case grfx::RESOURCE_STATE_PIXEL_SHADER_RESOURCE     : return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; break;
        case grfx::RESOURCE_STATE_SHADER_RESOURCE           : return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE| D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; break;
        case grfx::RESOURCE_STATE_DEPTH_STENCIL_READ        : return D3D12_RESOURCE_STATE_DEPTH_READ; break;
        case grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE       : return D3D12_RESOURCE_STATE_DEPTH_WRITE; break;
        case grfx::RESOURCE_STATE_RENDER_TARGET             : return D3D12_RESOURCE_STATE_RENDER_TARGET; break;
        case grfx::RESOURCE_STATE_COPY_SRC                  : return D3D12_RESOURCE_STATE_COPY_SOURCE; break;
        case grfx::RESOURCE_STATE_COPY_DST                  : return D3D12_RESOURCE_STATE_COPY_DEST; break;
        case grfx::RESOURCE_STATE_RESOLVE_SRC               : return D3D12_RESOURCE_STATE_RESOLVE_SOURCE; break;
        case grfx::RESOURCE_STATE_RESOLVE_DST               : return D3D12_RESOURCE_STATE_RESOLVE_DEST; break;
        case grfx::RESOURCE_STATE_PRESENT                   : return D3D12_RESOURCE_STATE_PRESENT; break;
        case grfx::RESOURCE_STATE_UNORDERED_ACCESS          : return D3D12_RESOURCE_STATE_UNORDERED_ACCESS; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_RESOURCE_STATES>();
}

D3D12_RTV_DIMENSION ToD3D12RTVDimension(grfx::ImageViewType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_VIEW_TYPE_1D       : return D3D12_RTV_DIMENSION_TEXTURE1D; break;
        case grfx::IMAGE_VIEW_TYPE_1D_ARRAY : return D3D12_RTV_DIMENSION_TEXTURE1DARRAY; break;
        case grfx::IMAGE_VIEW_TYPE_2D       : return D3D12_RTV_DIMENSION_TEXTURE2D; break;
        case grfx::IMAGE_VIEW_TYPE_2D_ARRAY : return D3D12_RTV_DIMENSION_TEXTURE2DARRAY; break;
        case grfx::IMAGE_VIEW_TYPE_3D       : return D3D12_RTV_DIMENSION_TEXTURE3D; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_RTV_DIMENSION>();
}

D3D12_SHADER_COMPONENT_MAPPING ToD3D12ShaderComponentMapping(grfx::ComponentSwizzle value, D3D12_SHADER_COMPONENT_MAPPING identity)
{
    if (value == grfx::COMPONENT_SWIZZLE_IDENTITY)
        return identity;
    // clang-format off
    switch (value) {
        default: break;
        case grfx::COMPONENT_SWIZZLE_ZERO : return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0; break;
        case grfx::COMPONENT_SWIZZLE_ONE  : return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1; break;
        case grfx::COMPONENT_SWIZZLE_R    : return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0; break;
        case grfx::COMPONENT_SWIZZLE_G    : return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1; break;
        case grfx::COMPONENT_SWIZZLE_B    : return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2; break;
        case grfx::COMPONENT_SWIZZLE_A    : return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_SHADER_COMPONENT_MAPPING>();
}

D3D12_SHADER_VISIBILITY ToD3D12ShaderVisibliity(grfx::ShaderStageBits value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx:: SHADER_STAGE_VS           : return D3D12_SHADER_VISIBILITY_VERTEX; break;
        case grfx:: SHADER_STAGE_HS           : return D3D12_SHADER_VISIBILITY_HULL; break;
        case grfx:: SHADER_STAGE_DS           : return D3D12_SHADER_VISIBILITY_DOMAIN; break;
        case grfx:: SHADER_STAGE_GS           : return D3D12_SHADER_VISIBILITY_GEOMETRY; break;
        case grfx:: SHADER_STAGE_PS           : return D3D12_SHADER_VISIBILITY_PIXEL; break;
        case grfx:: SHADER_STAGE_CS           : return D3D12_SHADER_VISIBILITY_ALL; break;
        case grfx:: SHADER_STAGE_ALL_GRAPHICS : return D3D12_SHADER_VISIBILITY_ALL; break;
        case grfx:: SHADER_STAGE_ALL          : return D3D12_SHADER_VISIBILITY_ALL; break;       
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_SHADER_VISIBILITY>();
}

D3D12_SRV_DIMENSION ToD3D12SRVDimension(grfx::ImageViewType value, uint32_t arrayLayerCount)
{
    // clang-format off
    switch (value) {
        default: break;
        case IMAGE_VIEW_TYPE_1D         : return D3D12_SRV_DIMENSION_TEXTURE1D; break;
        case IMAGE_VIEW_TYPE_2D         : return D3D12_SRV_DIMENSION_TEXTURE2D; break;
        case IMAGE_VIEW_TYPE_3D         : return D3D12_SRV_DIMENSION_TEXTURE3D; break;
        case IMAGE_VIEW_TYPE_CUBE       : return D3D12_SRV_DIMENSION_TEXTURECUBE; break;
        case IMAGE_VIEW_TYPE_1D_ARRAY   : return D3D12_SRV_DIMENSION_TEXTURE1DARRAY; break;
        case IMAGE_VIEW_TYPE_2D_ARRAY   : return D3D12_SRV_DIMENSION_TEXTURE2DARRAY; break;
        case IMAGE_VIEW_TYPE_CUBE_ARRAY : return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_SRV_DIMENSION>();

    /*
    // clang-format off
    switch (value) {
        default: break;
        case IMAGE_VIEW_TYPE_1D   : return (arrayLayerCount > 1) ? D3D12_SRV_DIMENSION_TEXTURE1DARRAY : D3D12_SRV_DIMENSION_TEXTURE1D; break;
        case IMAGE_VIEW_TYPE_2D   : return (arrayLayerCount > 1) ? D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION_TEXTURE2D; break;
        case IMAGE_VIEW_TYPE_3D   : return D3D12_SRV_DIMENSION_TEXTURE3D; break;
        case IMAGE_VIEW_TYPE_CUBE : return (arrayLayerCount > 6) ? D3D12_SRV_DIMENSION_TEXTURECUBEARRAY : D3D12_SRV_DIMENSION_TEXTURECUBE; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_SRV_DIMENSION>();
*/
}

D3D12_STENCIL_OP ToD3D12StencilOp(grfx::StencilOp value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::STENCIL_OP_KEEP                : return D3D12_STENCIL_OP_KEEP; break;
        case grfx::STENCIL_OP_ZERO                : return D3D12_STENCIL_OP_ZERO; break;
        case grfx::STENCIL_OP_REPLACE             : return D3D12_STENCIL_OP_REPLACE; break;
        case grfx::STENCIL_OP_INCREMENT_AND_CLAMP : return D3D12_STENCIL_OP_INCR_SAT; break;
        case grfx::STENCIL_OP_DECREMENT_AND_CLAMP : return D3D12_STENCIL_OP_DECR_SAT; break;
        case grfx::STENCIL_OP_INVERT              : return D3D12_STENCIL_OP_INVERT; break;
        case grfx::STENCIL_OP_INCREMENT_AND_WRAP  : return D3D12_STENCIL_OP_INCR; break;
        case grfx::STENCIL_OP_DECREMENT_AND_WRAP  : return D3D12_STENCIL_OP_DECR; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_STENCIL_OP>();
}

D3D12_TEXTURE_ADDRESS_MODE ToD3D12TextureAddressMode(grfx::SamplerAddressMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::SAMPLER_ADDRESS_MODE_REPEAT          : return D3D12_TEXTURE_ADDRESS_MODE_WRAP; break;
        case grfx::SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : return D3D12_TEXTURE_ADDRESS_MODE_MIRROR; break;
        case grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE   : return D3D12_TEXTURE_ADDRESS_MODE_CLAMP; break;
        case grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER : return D3D12_TEXTURE_ADDRESS_MODE_BORDER; break;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_TEXTURE_ADDRESS_MODE>();
}

D3D12_RESOURCE_DIMENSION ToD3D12TextureResourceDimension(grfx::ImageType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_TYPE_1D   : return D3D12_RESOURCE_DIMENSION_TEXTURE1D; break;
        case grfx::IMAGE_TYPE_2D   : return D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;
        case grfx::IMAGE_TYPE_3D   : return D3D12_RESOURCE_DIMENSION_TEXTURE3D; break;
        case grfx::IMAGE_TYPE_CUBE : return D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;
    }
    // clang-format on
    return D3D12_RESOURCE_DIMENSION_UNKNOWN;
}

D3D12_UAV_DIMENSION ToD3D12UAVDimension(grfx::ImageViewType value, uint32_t arrayLayerCount)
{
    // clang-format off
    switch (value) {
        default: break;
        case IMAGE_VIEW_TYPE_1D   : return (arrayLayerCount > 1) ? D3D12_UAV_DIMENSION_TEXTURE1DARRAY : D3D12_UAV_DIMENSION_TEXTURE1D; break;
        case IMAGE_VIEW_TYPE_2D   : return (arrayLayerCount > 1) ? D3D12_UAV_DIMENSION_TEXTURE2DARRAY : D3D12_UAV_DIMENSION_TEXTURE2D; break;
        case IMAGE_VIEW_TYPE_3D   : return D3D12_UAV_DIMENSION_TEXTURE3D;
    }
    // clang-format on
    return ppx::InvalidValue<D3D12_UAV_DIMENSION>();
}

UINT8 ToD3D12WriteMask(uint32_t value)
{
    UINT8 mask = 0;
    if (value & grfx::COLOR_COMPONENT_R)
        mask |= D3D12_COLOR_WRITE_ENABLE_RED;
    if (value & grfx::COLOR_COMPONENT_G)
        mask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
    if (value & grfx::COLOR_COMPONENT_B)
        mask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
    if (value & grfx::COLOR_COMPONENT_A)
        mask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
    return mask;
}

DXGI_FORMAT ToDxgiFormat(grfx::Format value)
{
    //
    // NOTE: DXGI_FORMAT_UNKNOWN is returned for any format that is
    //       not supported by D3D12.
    //
    // NOTE: D3D12 support for 3 channel formats is limited.
    //
    // NOTE: D3D12 support for SRGB formats is limited.
    //

    // clang-format off
    switch (value) {
        default: break;

        // 8-bit signed normalized
        case grfx::FORMAT_R8_SNORM           : return DXGI_FORMAT_R8_SNORM; break;
        case grfx::FORMAT_R8G8_SNORM         : return DXGI_FORMAT_R8G8_SNORM; break;
        case grfx::FORMAT_R8G8B8_SNORM       : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8B8A8_SNORM     : return DXGI_FORMAT_R8G8B8A8_SNORM; break;
        case grfx::FORMAT_B8G8R8_SNORM       : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_B8G8R8A8_SNORM     : return DXGI_FORMAT_UNKNOWN; break;
                                             
        // 8-bit unsigned normalized         
        case grfx::FORMAT_R8_UNORM           : return DXGI_FORMAT_R8_UNORM; break;
        case grfx::FORMAT_R8G8_UNORM         : return DXGI_FORMAT_R8G8_UNORM; break;
        case grfx::FORMAT_R8G8B8_UNORM       : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8B8A8_UNORM     : return DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case grfx::FORMAT_B8G8R8_UNORM       : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_B8G8R8A8_UNORM     : return DXGI_FORMAT_B8G8R8A8_UNORM; break;
                                             
        // 8-bit signed integer              
        case grfx::FORMAT_R8_SINT            : return DXGI_FORMAT_R8_SINT; break;
        case grfx::FORMAT_R8G8_SINT          : return DXGI_FORMAT_R8G8_SINT; break;
        case grfx::FORMAT_R8G8B8_SINT        : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8B8A8_SINT      : return DXGI_FORMAT_R8G8B8A8_SINT; break;
        case grfx::FORMAT_B8G8R8_SINT        : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_B8G8R8A8_SINT      : return DXGI_FORMAT_UNKNOWN; break;
                                             
        // 8-bit unsigned integer            
        case grfx::FORMAT_R8_UINT            : return DXGI_FORMAT_R8_UINT; break;
        case grfx::FORMAT_R8G8_UINT          : return DXGI_FORMAT_R8G8_UINT; break;
        case grfx::FORMAT_R8G8B8_UINT        : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8B8A8_UINT      : return DXGI_FORMAT_R8G8B8A8_UINT; break;
        case grfx::FORMAT_B8G8R8_UINT        : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_B8G8R8A8_UINT      : return DXGI_FORMAT_UNKNOWN; break;
                                             
        // 16-bit signed normalized          
        case grfx::FORMAT_R16_SNORM          : return DXGI_FORMAT_R16_SNORM; break;
        case grfx::FORMAT_R16G16_SNORM       : return DXGI_FORMAT_R16G16_SNORM; break;
        case grfx::FORMAT_R16G16B16_SNORM    : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R16G16B16A16_SNORM : return DXGI_FORMAT_R16G16B16A16_SNORM; break;
                                             
        // 16-bit unsigned normalized        
        case grfx::FORMAT_R16_UNORM          : return DXGI_FORMAT_R16_UNORM; break;
        case grfx::FORMAT_R16G16_UNORM       : return DXGI_FORMAT_R16G16_UNORM; break;
        case grfx::FORMAT_R16G16B16_UNORM    : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R16G16B16A16_UNORM : return DXGI_FORMAT_R16G16B16A16_UNORM; break;
                                             
        // 16-bit signed integer             
        case grfx::FORMAT_R16_SINT           : return DXGI_FORMAT_R16_SINT; break;
        case grfx::FORMAT_R16G16_SINT        : return DXGI_FORMAT_R16G16_SINT; break;
        case grfx::FORMAT_R16G16B16_SINT     : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R16G16B16A16_SINT  : return DXGI_FORMAT_R16G16B16A16_SINT; break;
                                             
        // 16-bit unsigned integer           
        case grfx::FORMAT_R16_UINT           : return DXGI_FORMAT_R16_UINT; break;
        case grfx::FORMAT_R16G16_UINT        : return DXGI_FORMAT_R16G16_UINT; break;
        case grfx::FORMAT_R16G16B16_UINT     : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R16G16B16A16_UINT  : return DXGI_FORMAT_R16G16B16A16_UINT; break;
                                             
        // 16-bit float                      
        case grfx::FORMAT_R16_FLOAT          : return DXGI_FORMAT_R16_FLOAT; break;
        case grfx::FORMAT_R16G16_FLOAT       : return DXGI_FORMAT_R16G16_FLOAT; break;
        case grfx::FORMAT_R16G16B16_FLOAT    : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R16G16B16A16_FLOAT : return DXGI_FORMAT_R16G16B16A16_FLOAT; break;
                                             
        // 32-bit signed integer             
        case grfx::FORMAT_R32_SINT           : return DXGI_FORMAT_R32_SINT; break;
        case grfx::FORMAT_R32G32_SINT        : return DXGI_FORMAT_R32G32_SINT; break;
        case grfx::FORMAT_R32G32B32_SINT     : return DXGI_FORMAT_R32G32B32_SINT; break;
        case grfx::FORMAT_R32G32B32A32_SINT  : return DXGI_FORMAT_R32G32B32A32_SINT; break;
                                             
        // 32-bit unsigned integer           
        case grfx::FORMAT_R32_UINT           : return DXGI_FORMAT_R32_UINT; break;
        case grfx::FORMAT_R32G32_UINT        : return DXGI_FORMAT_R32G32_UINT; break;
        case grfx::FORMAT_R32G32B32_UINT     : return DXGI_FORMAT_R32G32B32_UINT; break;
        case grfx::FORMAT_R32G32B32A32_UINT  : return DXGI_FORMAT_R32G32B32A32_UINT; break;
                                             
        // 32-bit float                      
        case grfx::FORMAT_R32_FLOAT          : return DXGI_FORMAT_R32_FLOAT; break;
        case grfx::FORMAT_R32G32_FLOAT       : return DXGI_FORMAT_R32G32_FLOAT; break;
        case grfx::FORMAT_R32G32B32_FLOAT    : return DXGI_FORMAT_R32G32B32_FLOAT; break;
        case grfx::FORMAT_R32G32B32A32_FLOAT : return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
                                             
        // 8-bit unsigned integer stencil    
        case grfx::FORMAT_S8_UINT            : return DXGI_FORMAT_UNKNOWN; break;
                                             
        // 16-bit unsigned normalized depth  
        case grfx::FORMAT_D16_UNORM          : return DXGI_FORMAT_D16_UNORM; break;
                                             
        // 32-bit float depth                
        case grfx::FORMAT_D32_FLOAT          : return DXGI_FORMAT_D32_FLOAT; break;
                                             
        // Depth/stencil combinations        
        case grfx::FORMAT_D16_UNORM_S8_UINT  : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_D24_UNORM_S8_UINT  : return DXGI_FORMAT_D24_UNORM_S8_UINT; break;
        case grfx::FORMAT_D32_FLOAT_S8_UINT  : return DXGI_FORMAT_D24_UNORM_S8_UINT; break;
                                             
        // SRGB                              
        case grfx::FORMAT_R8_SRGB            : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8_SRGB          : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8B8_SRGB        : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_R8G8B8A8_SRGB      : return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
        case grfx::FORMAT_B8G8R8_SRGB        : return DXGI_FORMAT_UNKNOWN; break;
        case grfx::FORMAT_B8G8R8A8_SRGB      : return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; break;

        // 10-bit
        case FORMAT_R10G10B10A2_UNORM        : return DXGI_FORMAT_R10G10B10A2_UNORM; break;

        // 11-bit R, 11-bit G, 10-bit B packed
        case FORMAT_R11G11B10_FLOAT          : return DXGI_FORMAT_R11G11B10_FLOAT; break;

        // Compressed images
        case FORMAT_BC1_RGBA_SRGB            : return DXGI_FORMAT_BC1_TYPELESS; break;
        case FORMAT_BC1_RGBA_UNORM           : return DXGI_FORMAT_BC1_UNORM; break;
        case FORMAT_BC1_RGB_SRGB             : return DXGI_FORMAT_BC1_UNORM_SRGB; break;
        case FORMAT_BC1_RGB_UNORM            : return DXGI_FORMAT_BC1_UNORM_SRGB; break; // FAILS with compressonator-generated DDS file
        case FORMAT_BC2_SRGB                 : return DXGI_FORMAT_BC2_UNORM_SRGB; break;
        case FORMAT_BC2_UNORM                : return DXGI_FORMAT_BC2_UNORM; break;
        case FORMAT_BC3_SRGB                 : return DXGI_FORMAT_BC3_UNORM_SRGB; break;
        case FORMAT_BC3_UNORM                : return DXGI_FORMAT_BC3_UNORM; break;
        case FORMAT_BC4_UNORM                : return DXGI_FORMAT_BC4_UNORM; break; // FAILS with compressonator-generated DDS file
        case FORMAT_BC4_SNORM                : return DXGI_FORMAT_BC4_SNORM; break;
        case FORMAT_BC5_UNORM                : return DXGI_FORMAT_BC5_UNORM; break;
        case FORMAT_BC5_SNORM                : return DXGI_FORMAT_BC5_SNORM; break;
        case FORMAT_BC6H_UFLOAT              : return DXGI_FORMAT_BC6H_UF16; break;
        case FORMAT_BC6H_SFLOAT              : return DXGI_FORMAT_BC6H_SF16; break;
        case FORMAT_BC7_UNORM                : return DXGI_FORMAT_BC7_UNORM; break;
        case FORMAT_BC7_SRGB                 : return DXGI_FORMAT_BC7_UNORM_SRGB; break;
    }
    // clang-format on

    return DXGI_FORMAT_UNKNOWN;
}

} // namespace dx
} // namespace grfx
} // namespace ppx
