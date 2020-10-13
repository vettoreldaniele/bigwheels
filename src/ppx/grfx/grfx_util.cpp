#include "ppx/grfx/grfx_util.h"

namespace ppx {
namespace grfx {

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