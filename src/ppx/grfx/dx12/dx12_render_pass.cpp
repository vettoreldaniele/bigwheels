#include "ppx/grfx/dx12/dx12_render_pass.h"

namespace ppx {
namespace grfx {
namespace dx12 {

Result RenderPass::CreateApiObjects(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void RenderPass::DestroyApiObjects()
{
}

} // namespace dx12
} // namespace grfx
} // namespace ppx
