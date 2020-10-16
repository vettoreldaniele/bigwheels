#include "ppx/imgui_impl.h"

#include "ppx/application.h"

#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_vulkan.h"

#include "ppx/grfx/vk/vk_command.h"
#include "ppx/grfx/vk/vk_descriptor.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_gpu.h"
#include "ppx/grfx/vk/vk_instance.h"
#include "ppx/grfx/vk/vk_queue.h"
#include "ppx/grfx/vk/vk_render_pass.h"

#if defined(PPX_D3D12)
#include "examples/imgui_impl_dx12.h"

#include "ppx/grfx/dx/dx_command.h"
#include "ppx/grfx/dx/dx_device.h"
#endif // defined(PPX_D3D12)

namespace ppx {

// -------------------------------------------------------------------------------------------------
// ImGuiImpl
// -------------------------------------------------------------------------------------------------
void ImGuiImpl::SetColorStyle()
{
    //ImGui::StyleColorsClassic();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
}

// -------------------------------------------------------------------------------------------------
// ImGuiImplVk
// -------------------------------------------------------------------------------------------------
Result ImGuiImplVk::Init(ppx::Application* pApp)
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup GLFW binding
    GLFWwindow* pWindow = static_cast<GLFWwindow*>(pApp->GetWindow());
    ImGui_ImplGlfw_InitForVulkan(pWindow, false);

    // Setup style
    SetColorStyle();

    // Create descriptor pool
    {
        grfx::DescriptorPoolCreateInfo ci = {};
        ci.combinedImageSampler           = 1;

        Result ppxres = pApp->GetDevice()->CreateDescriptorPool(&ci, &mPool);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Setup Vulkan binding
    {
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance                  = grfx::vk::ToApi(pApp->GetInstance())->GetVkInstance();
        init_info.PhysicalDevice            = grfx::vk::ToApi(pApp->GetDevice()->GetGpu())->GetVkGpu();
        init_info.Device                    = grfx::vk::ToApi(pApp->GetDevice())->GetVkDevice();
        init_info.QueueFamily               = grfx::vk::ToApi(pApp->GetGraphicsQueue())->GetQueueFamilyIndex();
        init_info.Queue                     = grfx::vk::ToApi(pApp->GetGraphicsQueue())->GetVkQueue();
        init_info.PipelineCache             = VK_NULL_HANDLE;
        init_info.DescriptorPool            = grfx::vk::ToApi(mPool)->GetVkDescriptorPool();
        init_info.MinImageCount             = pApp->GetSwapchain()->GetImageCount();
        init_info.ImageCount                = pApp->GetSwapchain()->GetImageCount();
        init_info.Allocator                 = VK_NULL_HANDLE;
        init_info.CheckVkResultFn           = nullptr;

        grfx::RenderPassPtr renderPass = pApp->GetSwapchain()->GetRenderPass(0, grfx::ATTACHMENT_LOAD_OP_LOAD);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "[imgui:vk] failed to get swapchain renderpass");

        bool result = ImGui_ImplVulkan_Init(&init_info, grfx::vk::ToApi(renderPass)->GetVkRenderPass());
        if (!result) {
            return ppx::ERROR_IMGUI_INITIALIZATION_FAILED;
        }
    }

    // Upload Fonts
    {
        // Create command buffer
        grfx::CommandBufferPtr commandBuffer;
        Result                 ppxres = pApp->GetGraphicsQueue()->CreateCommandBuffer(&commandBuffer, 0, 0);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "[imgui:vk] command buffer create failed");
            return ppxres;
        }

        // Begin command buffer
        ppxres = commandBuffer->Begin();
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "[imgui:vk] command buffer begin failed");
            return ppxres;
        }

        // Create fonts texture
        ImGui_ImplVulkan_CreateFontsTexture(grfx::vk::ToApi(commandBuffer)->GetVkCommandBuffer());

        // End command buffer
        ppxres = commandBuffer->End();
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "[imgui:vk] command buffer end failed");
            return ppxres;
        }

        // Submit
        grfx::SubmitInfo submitInfo   = {};
        submitInfo.commandBufferCount = 1;
        submitInfo.ppCommandBuffers   = &commandBuffer;

        ppxres = pApp->GetGraphicsQueue()->Submit(&submitInfo);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "[imgui:vk] command buffer submit failed");
            return ppxres;
        }

        // Wait for idle
        ppxres = pApp->GetGraphicsQueue()->WaitIdle();
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "[imgui:vk] queue wait idle failed");
            return ppxres;
        }

        // Destroy command buffer
        pApp->GetGraphicsQueue()->DestroyCommandBuffer(commandBuffer);

        // Destroy font upload objects
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    return ppx::SUCCESS;
}

void ImGuiImplVk::Shutdown(ppx::Application* pApp)
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (mPool) {
        pApp->GetDevice()->DestroyDescriptorPool(mPool);
        mPool.Reset();
    }
}

void ImGuiImplVk::NewFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiImplVk::Render(grfx::CommandBuffer* pCommandBuffer)
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), grfx::vk::ToApi(pCommandBuffer)->GetVkCommandBuffer());
}

// -------------------------------------------------------------------------------------------------
// ImGuiImplDx
// -------------------------------------------------------------------------------------------------
#if defined(PPX_D3D12)

Result ImGuiImplDx::Init(ppx::Application* pApp)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup GLFW binding
    GLFWwindow* pWindow = static_cast<GLFWwindow*>(pApp->GetWindow());
    ImGui_ImplGlfw_InitForVulkan(pWindow, false);

    // Setup style
    SetColorStyle();

    // Setup descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors             = 1;
        desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask                   = 0;

        grfx::dx::D3D12DevicePtr device = grfx::dx::ToApi(pApp->GetDevice())->GetDxDevice();
        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapCBVSRVUAV));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateDescriptorHeap(CBVSRVUAV) failed");
            return ppx::ERROR_API_FAILURE;
        }
        PPX_LOG_OBJECT_CREATION(D3D12DescriptorHeap(CBVSRVUAV), mHeapCBVSRVUAV.Get());
    }

    // Setup DX12 binding
    bool result = ImGui_ImplDX12_Init(
        grfx::dx::ToApi(pApp->GetDevice())->GetDxDevice(),
        static_cast<int>(pApp->GetNumFramesInFlight()),
        grfx::dx::ToDxgiFormat(pApp->GetSwapchain()->GetColorFormat()),
        mHeapCBVSRVUAV,
        mHeapCBVSRVUAV->GetCPUDescriptorHandleForHeapStart(),
        mHeapCBVSRVUAV->GetGPUDescriptorHandleForHeapStart());
    if (!result) {
        return ppx::ERROR_IMGUI_INITIALIZATION_FAILED;
    }

    return ppx::SUCCESS;
}

void ImGuiImplDx::Shutdown(ppx::Application* pApp)
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (mHeapCBVSRVUAV != nullptr) {
        mHeapCBVSRVUAV->Release();
        mHeapCBVSRVUAV = nullptr;
    }
}

void ImGuiImplDx::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiImplDx::Render(grfx::CommandBuffer* pCommandBuffer)
{
    grfx::dx::D3D12GraphicsCommandListPtr commandList = grfx::dx::ToApi(pCommandBuffer)->GetDxCommandList();
    commandList->SetDescriptorHeaps(1, &mHeapCBVSRVUAV);

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), grfx::dx::ToApi(pCommandBuffer)->GetDxCommandList());
}

#endif // defined(PPX_D3D12)

} // namespace ppx
