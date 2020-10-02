#ifndef ppx_render_base_pass_h
#define ppx_render_base_pass_h

#include "ppx/000_config.h"

namespace ppx {
namespace render {

class BasePass
{
public:
    BasePass() {}
    virtual ~BasePass() {}

protected:
    ObjPtr<RenderPass> mRenderPass;
};

} // namespace render
} // namespace ppx

#endif // ppx_render_base_pass_h
