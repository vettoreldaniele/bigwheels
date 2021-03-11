#include "ppx/grfx/dx11/dx11_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx11 {

Result RenderPass::CreateApiObjects(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void RenderPass::DestroyApiObjects()
{
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
