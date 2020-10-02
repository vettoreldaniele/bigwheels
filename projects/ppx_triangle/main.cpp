#include "ppx/ppx.h"
using namespace ppx;

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

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
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "PPX Triangle";
    settings.grfx.api         = ppx::grfx::API_VK;
    settings.grfx.enableDebug = true;
}

void ProjApp::Setup()
{
    {
        PerFrame frame = {};

        Result ppxres = GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "create command buffer failed");

        grfx::SemaphoreCreateInfo semaCreateInfo = {};
        ppxres                                   = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "create semaphore failed");

        grfx::FenceCreateInfo fenceCreateInfo = {};
        ppxres                                = GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "create fence failed");

        ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "create semaphore failed");
      
        fenceCreateInfo = {true}; // Create signaled
        ppxres          = GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "create fence failed");

        mPerFrame.push_back(frame);
    }
}

void ProjApp::Render()
{
    PerFrame& frame = mPerFrame[0];

    grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    Result   ppxres     = swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "acquire next image failed");

    // Wait for image acquired fence
    ppxres = frame.imageAcquiredFence->Wait(UINT64_MAX);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "image acquired fence wait failed");
    ppxres = frame.imageAcquiredFence->Reset();
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "image acquired fence reset failed");

    // Wait for render complete fence
    ppxres = frame.renderCompleteFence->Wait(UINT64_MAX);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "render complete fence wait failed");
    ppxres = frame.renderCompleteFence->Reset();
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "render complete fence reset failed");

    // Build command buffer
    ppxres = frame.cmd->Begin();
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "cmd begin failed");
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{1, 0, 0, 1}};

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), 0, 1, 0, 1, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        frame.cmd->EndRenderPass();
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), 0, 1, 0, 1, grfx::RESOURCE_STATE_RENDER_TARGET, grfx::RESOURCE_STATE_PRESENT);
    }
    ppxres = frame.cmd->End();
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "cmd end failed");

    grfx::SubmitInfo submitInfo     = {};
    submitInfo.commandBufferCount   = 1;
    submitInfo.ppCommandBuffers     = &frame.cmd;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.ppWaitSemaphores     = &frame.imageAcquiredSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.ppSignalSemaphores   = &frame.renderCompleteSemaphore;
    submitInfo.pFence               = frame.renderCompleteFence;

    ppxres = GetGraphicsQueue()->Submit(&submitInfo);

    ppxres = GetGraphicsQueue()->Present(swapchain, imageIndex, 1, &frame.renderCompleteSemaphore);
    PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "present failed");
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
