#include "ppx/ppx.h"
using namespace ppx;

#if defined(USE_DX)
grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
grfx::Api kApi = grfx::API_VK_1_1;
#endif

#define kWindowWidth  1280
#define kWindowHeight 720

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void KeyDown(KeyCode key) override;
    virtual void KeyUp(KeyCode key) override;
    virtual void MouseMove(int32_t x, int32_t y, uint32_t buttons) override;
    virtual void MouseDown(int32_t x, int32_t y, uint32_t buttons) override;
    virtual void MouseUp(int32_t x, int32_t y, uint32_t buttons) override;
    virtual void Render() override;

private:
    void DrawGui();

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
    int32_t               mMouseX = 0;
    int32_t               mMouseY = 0;
    uint32_t              mMouseButtons;
    KeyState              mKeyStates[TOTAL_KEY_COUNT] = {0};
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "input";
    settings.window.width     = kWindowWidth;
    settings.window.height    = kWindowHeight;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::Setup()
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

void ProjApp::KeyDown(KeyCode key)
{
    mKeyStates[key].down     = true;
    mKeyStates[key].timeDown = GetElapsedSeconds();
}

void ProjApp::KeyUp(KeyCode key)
{
    mKeyStates[key].down     = false;
    mKeyStates[key].timeDown = FLT_MAX;
}

void ProjApp::MouseMove(int32_t x, int32_t y, uint32_t buttons)
{
    mMouseX = x;
    mMouseY = y;
}

void ProjApp::MouseDown(int32_t x, int32_t y, uint32_t buttons)
{
    mMouseButtons |= buttons;
}

void ProjApp::MouseUp(int32_t x, int32_t y, uint32_t buttons)
{
    mMouseButtons &= ~buttons;
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
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{0, 0, 0, 0}};

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            // Draw ImGui
            DrawDebugInfo([this]() { this->DrawGui(); });
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

void ProjApp::DrawGui()
{
    if (ImGui::Begin("Mouse Info")) {
        ImGui::Columns(2);

        ImGui::Text("Position");
        ImGui::NextColumn();
        ImGui::Text("%d, %d", mMouseX, mMouseY);
        ImGui::NextColumn();

        ImGui::Text("Left Button");
        ImGui::NextColumn();
        ImGui::Text((mMouseButtons & MOUSE_BUTTON_LEFT) ? "DOWN" : "UP");
        ImGui::NextColumn();

        ImGui::Text("Middle Button");
        ImGui::NextColumn();
        ImGui::Text((mMouseButtons & MOUSE_BUTTON_MIDDLE) ? "DOWN" : "UP");
        ImGui::NextColumn();

        ImGui::Text("Right Button");
        ImGui::NextColumn();
        ImGui::Text((mMouseButtons & MOUSE_BUTTON_RIGHT) ? "DOWN" : "UP");
        ImGui::NextColumn();
    }
    ImGui::End();

    if (ImGui::Begin("Key State")) {
        ImGui::Columns(3);

        ImGui::Text("KEY CODE");
        ImGui::NextColumn();
        ImGui::Text("STATE");
        ImGui::NextColumn();
        ImGui::Text("TIME DOWN");
        ImGui::NextColumn();

        float currentTime = GetElapsedSeconds();
        for (uint32_t i = KEY_RANGE_FIRST; i <= KEY_RANGE_LAST; ++i) {
            const KeyState& state         = mKeyStates[i];
            float           timeSinceDown = std::max(0.0f, currentTime - state.timeDown);

            ImGui::Text("%s", GetKeyCodeString(static_cast<KeyCode>(i)));
            ImGui::NextColumn();
            ImGui::Text(state.down ? "DOWN" : "UP");
            ImGui::NextColumn();
            ImGui::Text("%.06f", timeSinceDown);
            ImGui::NextColumn();
        }
    }
    ImGui::End();
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}