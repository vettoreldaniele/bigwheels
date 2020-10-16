#ifndef ppx_application_h
#define ppx_application_h

#include "ppx/base_application.h"
#include "ppx/000_math_config.h"
#include "ppx/imgui_impl.h"
#include "ppx/timer.h"

// clang-format off
#include <GLFW/glfw3.h>
#if defined(PPX_LINUX)
#   define GLFW_EXPOSE_NATIVE_X11
#elif defined(PPX_MSW)
#   define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>
// clang-format on

namespace ppx {

struct ApplicationSettings
{
    std::string appName = "";

    struct
    {
        uint32_t    width     = 0;
        uint32_t    height    = 0;
        std::string title     = "";
        bool        resizable = false;
    } window;

    struct
    {
        grfx::Api api         = grfx::API_UNDEFINED;
        bool      enableDebug = false;

        uint32_t numFramesInFlight = 1;
        uint32_t pacedFrameRate    = 60;

        struct
        {
            uint32_t gpuIndex           = 0;
            uint32_t graphicsQueueCount = 1;
            uint32_t computeQueueCount  = 1;
            uint32_t transferQueueCount = 1;
        } device;

        struct
        {
            grfx::Format colorFormat = grfx::FORMAT_B8G8R8A8_UNORM;
            grfx::Format depthFormat = grfx::FORMAT_UNDEFINED;
            uint32_t     imageCount  = 2;
        } swapchain;
    } grfx;
};

class Application
    : public BaseApplication
{
public:
    Application();
    Application(uint32_t windowWidth, uint32_t windowHeight, const char* windowTitle);
    virtual ~Application();

    static Application* Get();

    virtual void Config(ppx::ApplicationSettings& settings) {}
    virtual void Setup() {}
    virtual void Shutdown() {}
    virtual void KeyDown(int key) {}
    virtual void KeyUp(int key) {}
    virtual void MouseMove(int x, int y, int buttons) {}
    virtual void MouseDown(int button) {}
    virtual void MouseUp(int button) {}
    virtual void Render() {}

protected:
    virtual void DispatchConfig();
    virtual void DispatchSetup();
    virtual void DispatchShutdown();
    virtual void DispatchRender();

    void DrawImGui(grfx::CommandBuffer* pCommandBuffer);
    void DrawDebugInfo();

public:
    int Run(int argc, char** argv);

    uint32_t GetNumFramesInFlight() const { return mSettings.grfx.numFramesInFlight; }
    float    GetElapsedSeconds() const;

    uint32_t GetProcessId() const;

    void*              GetWindow() const { return mWindow; }
    grfx::InstancePtr  GetInstance() const { return mInstance; }
    grfx::DevicePtr    GetDevice() const { return mDevice; }
    grfx::QueuePtr     GetGraphicsQueue(uint32_t index = 0) const { return GetDevice()->GetGraphicsQueue(index); }
    grfx::QueuePtr     GetComputeQueue(uint32_t index = 0) const { return GetDevice()->GetComputeQueue(index); }
    grfx::QueuePtr     GetTransferQueue(uint32_t index = 0) const { return GetDevice()->GetTransferQueue(index); }
    grfx::SwapchainPtr GetSwapchain() const { return mSwapchain; }

private:
    void   InternalCtor();
    Result InitializeGrfxDevice();
    Result InitializeGrfxSurface();
    Result InitializeImGui();
    void   ShutdownImGui();
    void   StopGrfx();
    void   ShutdownGrfx();
    Result CreatePlatformWindow();
    void   DestroyPlatformWindow();
    bool   IsRunning() const;

private:
    ApplicationSettings        mSettings = {};
    Timer                      mTimer;
    void*                      mWindow  = nullptr;
    bool                       mRunning = true;
    grfx::InstancePtr          mInstance;
    grfx::DevicePtr            mDevice;
    grfx::SurfacePtr           mSurface;
    grfx::SwapchainPtr         mSwapchain;
    std::unique_ptr<ImGuiImpl> mImGui;

    uint64_t mFrameCount        = 0;
    float    mAverageFPS        = 0;
    float    mFrameStartTime    = 0;
    float    mFrameEndTime      = 0;
    float    mPreviousFrameTime = 0;
    double   mFirstFrameTime    = 0;
};

} // namespace ppx

#endif // ppx_application_h
