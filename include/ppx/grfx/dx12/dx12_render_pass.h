#ifndef ppx_grfx_dx12_render_pass_h
#define ppx_grfx_dx12_render_pass_h

#include "ppx/grfx/dx12/000_dx12_config.h"
#include "ppx/grfx/grfx_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx12 {

class RenderPass
    : public grfx::RenderPass
{
public:
    RenderPass() {}
    virtual ~RenderPass() {}

protected:
    virtual Result CreateApiObjects(const grfx::internal::RenderPassCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
};

} // namespace dx12
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx12_render_pass_h
