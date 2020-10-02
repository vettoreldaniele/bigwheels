#ifndef ppx_render_depth_pass_h
#define ppx_render_depth_pass_h

#include "ppx/render/base_pass.h"

namespace ppx {
namespace render {

struct DepthPassCreateInfo
{
    uint32_t    width  = 0;
    uint32_t    height = 0;
    ppx::Format format = ppx::FORMAT_UNDEFINED;
};

class DepthPass
    : public BasePass
{
public:
};

} // namespace render
} // namespace ppx

#endif // ppx_render_depth_pass_h
