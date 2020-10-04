#include "ppx/grfx/grfx_helper.h"

namespace ppx {
namespace grfx {

// -------------------------------------------------------------------------------------------------
// ColorComponentFlags
// -------------------------------------------------------------------------------------------------
ColorComponentFlags ColorComponentFlags::RGBA()
{
    ColorComponentFlags flags = ColorComponentFlags(COLOR_COMPONENT_R | COLOR_COMPONENT_G | COLOR_COMPONENT_B | COLOR_COMPONENT_A);
    return flags;
}

// -------------------------------------------------------------------------------------------------
// ImageUsageFlags
// -------------------------------------------------------------------------------------------------
ImageUsageFlags ImageUsageFlags::SampledImage()
{
    ImageUsageFlags flags = ImageUsageFlags(grfx::IMAGE_USAGE_SAMPLED);
    return flags;
}

} // namespace grfx
} // namespace ppx
