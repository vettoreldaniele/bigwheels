#include "ppx/grfx/dx/dx_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx {

Result RenderPass::CreateApiObjects(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void RenderPass::DestroyApiObjects()
{
}

} // namespace dx
} // namespace grfx
} // namespace ppx