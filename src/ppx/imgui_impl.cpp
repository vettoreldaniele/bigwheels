#include "ppx/imgui_impl.h"
#include "ppx/imgui/font_inconsolata.h"

#include "ppx/application.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "ppx/grfx/vk/vk_command.h"
#include "ppx/grfx/vk/vk_descriptor.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_gpu.h"
#include "ppx/grfx/vk/vk_instance.h"
#include "ppx/grfx/vk/vk_queue.h"
#include "ppx/grfx/vk/vk_render_pass.h"

#if defined(PPX_D3D12)
#include "backends/imgui_impl_dx12.h"

#include "ppx/grfx/dx12/dx12_command.h"
#include "ppx/grfx/dx12/dx12_device.h"
#endif // defined(PPX_D3D12)

#if defined(PPX_MSW)
#include <ShellScalingApi.h>
#endif

namespace ppx {

// -------------------------------------------------------------------------------------------------
// ImGuiImpl
// -------------------------------------------------------------------------------------------------
Result ImGuiImpl::Init(ppx::Application* pApp)
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    float fontSize = 16.0f;
#if defined(PPX_GGP)
    if (pApp->GetWindowHeight() > 1080) {
        fontSize = 40.0f;
    }
#elif defined(PPX_MSW)
    HWND     activeWindow = GetActiveWindow();
    HMONITOR monitor      = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTONEAREST);

    DEVICE_SCALE_FACTOR scale = SCALE_100_PERCENT;
    HRESULT hr = GetScaleFactorForMonitor(monitor, &scale);
    if (FAILED(hr)) {
        return ppx::ERROR_FAILED;
    }

    float fontScale = 1.0f;
    // clang-format off
    switch (scale) {
        default: break;
        case SCALE_120_PERCENT: fontScale = 1.20f; break;
        case SCALE_125_PERCENT: fontScale = 1.25f; break;
        case SCALE_140_PERCENT: fontScale = 1.40f; break;
        case SCALE_150_PERCENT: fontScale = 1.50f; break;
        case SCALE_160_PERCENT: fontScale = 1.60f; break;
        case SCALE_175_PERCENT: fontScale = 1.75f; break;
        case SCALE_180_PERCENT: fontScale = 1.80f; break;
        case SCALE_200_PERCENT: fontScale = 2.00f; break;
        case SCALE_225_PERCENT: fontScale = 2.25f; break;
        case SCALE_250_PERCENT: fontScale = 2.50f; break;
        case SCALE_300_PERCENT: fontScale = 3.00f; break;
        case SCALE_350_PERCENT: fontScale = 3.50f; break;
        case SCALE_400_PERCENT: fontScale = 4.00f; break;
        case SCALE_450_PERCENT: fontScale = 4.50f; break;
        case SCALE_500_PERCENT: fontScale = 5.00f; break;
    }
    // clang-format on

    //// Get the logical width and height of the monitor
    //MONITORINFOEX monitorInfoEx = {};
    //monitorInfoEx.cbSize        = sizeof(monitorInfoEx);
    //GetMonitorInfo(monitor, &monitorInfoEx);
    //auto cxLogical = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
    //auto cyLogical = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;
    //
    //// Get the physical width and height of the monitor
    //DEVMODE devMode       = {};
    //devMode.dmSize        = sizeof(devMode);
    //devMode.dmDriverExtra = 0;
    //EnumDisplaySettings(monitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    //auto cxPhysical = devMode.dmPelsWidth;
    //auto cyPhysical = devMode.dmPelsHeight;
    //
    //// Calculate the scaling factor
    //float horizontalScale = ((float)cxPhysical / (float)cxLogical);
    //float verticalScale   = ((float)cyPhysical / (float)cyLogical);

    // Scale fontSize based on scaling factor
    fontSize *= fontScale;
#endif

    ImFontConfig fontConfig         = {};
    fontConfig.FontDataOwnedByAtlas = false;

    ImFont* pFont = io.Fonts->AddFontFromMemoryTTF(
        const_cast<void*>(static_cast<const void*>(imgui::kFontInconsolata)),
        static_cast<int>(imgui::kFontInconsolataSize),
        fontSize,
        &fontConfig);

    PPX_ASSERT_MSG(!IsNull(pFont), "imgui add font failed");

    Result ppxres = InitApiObjects(pApp);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void ImGuiImpl::SetColorStyle()
{
    //ImGui::StyleColorsClassic();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
}

void ImGuiImpl::NewFrame()
{
    Application* pApp = Application::Get();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(pApp->GetWindowWidth());
    io.DisplaySize.y = static_cast<float>(pApp->GetWindowHeight());
    NewFrameApi();
}

// -------------------------------------------------------------------------------------------------
// ImGuiImplVk
// -------------------------------------------------------------------------------------------------
Result ImGuiImplVk::InitApiObjects(ppx::Application* pApp)
{
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

void ImGuiImplVk::NewFrameApi()
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

Result ImGuiImplDx::InitApiObjects(ppx::Application* pApp)
{
    // Setup GLFW binding - yes...we're using the one for Vulkan :)
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
        HRESULT                  hr     = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHeapCBVSRVUAV));
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

void ImGuiImplDx::NewFrameApi()
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
