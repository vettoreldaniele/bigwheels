#include "ppx/grfx/grfx_util.h"

namespace ppx {
namespace grfx {

const char* ToString(grfx::Api value)
{
    switch (value) {
        default: break;
        case grfx::API_VK_1_1: return "Vulkan 1.1"; break;
        case grfx::API_VK_1_2: return "Vulkan 1.2"; break;
        case grfx::API_DX_12_0: return "D3D12 12.0"; break;
        case grfx::API_DX_12_1: return "D3D12 12.1"; break;
    }
    return "<unknown graphics API>";
}

const char* ToString(grfx::DescriptorType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::DESCRIPTOR_TYPE_SAMPLER                : return "grfx::DESCRIPTOR_TYPE_SAMPLER"; break;
        case grfx::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : return "grfx::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"; break;
        case grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE          : return "grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE"; break;
        case grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE          : return "grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE"; break;
        case grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER   : return "grfx::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER  "; break;
        case grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER   : return "grfx::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER  "; break;
        case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER         : return "grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER"; break;
        case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER         : return "grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER"; break;
        case grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : return "grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC"; break;
        case grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : return "grfx::DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC"; break;
        case grfx::DESCRIPTOR_TYPE_INPUT_ATTACHMENT       : return "grfx::DESCRIPTOR_TYPE_INPUT_ATTACHMENT"; break;
    }
    // clang-format on
    return "<unknown descriptor type>";
}

const char* ToString(grfx::VertexSemantic value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::VERTEX_SEMANTIC_POSITION   : return "POSITION"; break;
        case grfx::VERTEX_SEMANTIC_NORMAL     : return "NORMAL"; break;
        case grfx::VERTEX_SEMANTIC_COLOR      : return "COLOR"; break;
        case grfx::VERTEX_SEMANTIC_TANGENT    : return "TANGENT"; break;
        case grfx::VERTEX_SEMANTIC_BITANGENT  : return "BITANGENT"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD   : return "TEXCOORD"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD0  : return "TEXCOORD0"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD1  : return "TEXCOORD1"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD2  : return "TEXCOORD2"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD3  : return "TEXCOORD3"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD4  : return "TEXCOORD4"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD5  : return "TEXCOORD5"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD6  : return "TEXCOORD6"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD7  : return "TEXCOORD7"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD8  : return "TEXCOORD8"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD9  : return "TEXCOORD9"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD10 : return "TEXCOORD10"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD11 : return "TEXCOORD11"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD12 : return "TEXCOORD12"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD13 : return "TEXCOORD13"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD14 : return "TEXCOORD14"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD15 : return "TEXCOORD15"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD16 : return "TEXCOORD16"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD17 : return "TEXCOORD17"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD18 : return "TEXCOORD18"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD19 : return "TEXCOORD19"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD20 : return "TEXCOORD20"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD21 : return "TEXCOORD21"; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD22 : return "TEXCOORD22"; break;       
    }
    // clang-format on
    return "";
}

grfx::Format VertexSemanticFormat(grfx::VertexSemantic value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::VERTEX_SEMANTIC_POSITION   : return grfx::FORMAT_R32G32B32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_NORMAL     : return grfx::FORMAT_R32G32B32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_COLOR      : return grfx::FORMAT_R32G32B32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TANGENT    : return grfx::FORMAT_R32G32B32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_BITANGENT  : return grfx::FORMAT_R32G32B32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD   : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD0  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD1  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD2  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD3  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD4  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD5  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD6  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD7  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD8  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD9  : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD10 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD11 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD12 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD13 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD14 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD15 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD16 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD17 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD18 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD19 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD20 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD21 : return grfx::FORMAT_R32G32_FLOAT; break;
        case grfx::VERTEX_SEMANTIC_TEXCOORD22 : return grfx::FORMAT_R32G32_FLOAT; break;       
    }
    // clang-format on
    return grfx::FORMAT_UNDEFINED;
}

} // namespace grfx
} // namespace ppx
