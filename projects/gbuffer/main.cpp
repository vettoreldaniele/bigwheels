#include "ppx/ppx.h"
using namespace ppx;

#if defined(USE_DX)
grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
grfx::Api kApi = grfx::API_VK_1_1;
#endif

#define kWindowWidth  1920
#define kWindowHeight 1080

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

private:
    void SetupPerFrame();

private:
    struct PerFrame
    {
        ppx::grfx::CommandBufferPtr cmd;
        ppx::grfx::SemaphorePtr     imageAcquiredSemaphore;
        ppx::grfx::FencePtr         imageAcquiredFence;
        ppx::grfx::SemaphorePtr     renderCompleteSemaphore;
        ppx::grfx::FencePtr         renderCompleteFence;
    };

    std::vector<PerFrame> mPerFrame;

    grfx::DrawPassPtr mGBuffer;
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "gbuffer";
    settings.window.width     = kWindowWidth;
    settings.window.height    = kWindowHeight;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::SetupPerFrame()
{
    Result ppxres = ppx::SUCCESS;

    // Per frame data
    {
        PerFrame frame = {};

        PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd));

        grfx::SemaphoreCreateInfo semaCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore));

        grfx::FenceCreateInfo fenceCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence));

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore));

        fenceCreateInfo = {true}; // Create signaled
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence));

        mPerFrame.push_back(frame);
    }
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    SetupPerFrame();

    // GBuffer using a draw pass
    {
        // RT0: Position (RGB)
        // RT1: Albedo (RGB) Roughness (A)
        // RT2: Normal (RGB) Metalness (A)
        // RT3: AmbOcc (R) EnvStrength (G) ReflStrength(B)

        grfx::ImageUsageFlags        imageUsageFlags = grfx::IMAGE_USAGE_SAMPLED;
        grfx::RenderTargetClearValue rtvClearValue   = {0, 0, 0, 0};
        grfx::DepthStencilClearValue dsvClearValue   = {1.0f, 0xFF};

        grfx::DrawPassCreateInfo createInfo   = {};
        createInfo.width                      = GetWindowWidth();
        createInfo.height                     = GetWindowHeight();
        createInfo.renderTargetCount          = 4;
        createInfo.renderTargetFormats[0]     = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.renderTargetFormats[1]     = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.renderTargetFormats[2]     = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.renderTargetFormats[3]     = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.depthStencilFormat         = grfx::FORMAT_D32_FLOAT;
        createInfo.renderTargetUsageFlags[0]  = imageUsageFlags;
        createInfo.renderTargetUsageFlags[1]  = imageUsageFlags;
        createInfo.renderTargetUsageFlags[2]  = imageUsageFlags;
        createInfo.renderTargetUsageFlags[3]  = imageUsageFlags;
        createInfo.depthStencilUsageFlags     = imageUsageFlags;
        createInfo.renderTargetClearValues[0] = rtvClearValue;
        createInfo.renderTargetClearValues[1] = rtvClearValue;
        createInfo.renderTargetClearValues[2] = rtvClearValue;
        createInfo.renderTargetClearValues[3] = rtvClearValue;
        createInfo.depthStencilClearValue     = dsvClearValue;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDrawPass(&createInfo, &mGBuffer));
    }
}

void ProjApp::Render()
{
    Result    ppxres = ppx::SUCCESS;
    PerFrame& frame  = mPerFrame[0];

    grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(ppxres = swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(ppxres = frame.imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(ppxres = frame.renderCompleteFence->WaitAndReset());

    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "swapchain render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{1, 0, 0, 1}};

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            // Draw ImGui
            DrawDebugInfo();
            DrawImGui(frame.cmd);
        }
        frame.cmd->EndRenderPass();
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_RENDER_TARGET, grfx::RESOURCE_STATE_PRESENT);
    }
    PPX_CHECKED_CALL(ppxres = frame.cmd->End());

    grfx::SubmitInfo submitInfo     = {};
    submitInfo.commandBufferCount   = 1;
    submitInfo.ppCommandBuffers     = &frame.cmd;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.ppWaitSemaphores     = &frame.imageAcquiredSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.ppSignalSemaphores   = &frame.renderCompleteSemaphore;
    submitInfo.pFence               = frame.renderCompleteFence;

    PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->Submit(&submitInfo));

    PPX_CHECKED_CALL(ppxres = swapchain->Present(imageIndex, 1, &frame.renderCompleteSemaphore));
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
