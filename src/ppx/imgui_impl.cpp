#include "ppx/imgui_impl.h"

#include "examples//imgui_impl_vulkan.h"

#if defined(PPX_D3D12)
#include "examples/imgui_impl_dx12.h"
#endif // defined(PPX_D3D12)

namespace ppx {

static ImGui_ImplVulkan_InitInfo gImguiVulkanInfo = {0};

Result ImguiImpl::Init()
{
    return ppx::ERROR_FAILED;
}

void ImguiImpl::Shutdown()
{
}

void ImguiImpl::NewFrame()
{
}

void ImguiImpl::RenderDrawData(ImDrawData* draw_data, grfx::CommandBuffer* pCommandBuffer)
{
}

} // namespace ppx
