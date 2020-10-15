#include "ppx/grfx/grfx_util.h"

namespace ppx {
namespace grfx {

const char* ToString(grfx::Api value)
{
    switch (value) {
        default: break;
        case grfx::API_VK_1_1  : return "Vulkan 1.1"; break;
        case grfx::API_VK_1_2  : return "Vulkan 1.2"; break;
        case grfx::API_DX_12_0 : return "D3D12 12.0"; break;
        case grfx::API_DX_12_1 : return "D3D12 12.1"; break;
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

} // namespace grfx
} // namespace ppx