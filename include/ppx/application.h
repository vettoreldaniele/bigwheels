#ifndef ppx_application_h
#define ppx_application_h

#include "ppx/base_application.h"
#include "ppx/000_math_config.h"
#include "ppx/imgui_impl.h"
#include "ppx/timer.h"

// clang-format off
#if ! defined(GLFW_INCLUDE_NONE)
#   define GLFW_INCLUDE_NONE
#endif
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
        grfx::Api api               = grfx::API_UNDEFINED;
        bool      enableDebug       = false;
        bool      enableDXIL        = false;
        uint32_t  numFramesInFlight = 1;
        uint32_t  pacedFrameRate    = 60;

        struct
        {
            uint32_t gpuIndex           = 0;
            uint32_t graphicsQueueCount = 1;
            uint32_t computeQueueCount  = 0;
            uint32_t transferQueueCount = 0;
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
    void DrawDebugInfo(std::function<void(void)> drawAdditionalFn = []() {});

public:
    int Run(int argc, char** argv);

    uint32_t       GetWindowWidth() const { return mSettings.window.width; }
    uint32_t       GetWindowHeight() const { return mSettings.window.height; }
    float          GetWindowAspect() const { return static_cast<float>(mSettings.window.width) / static_cast<float>(mSettings.window.height); }
    grfx::Rect     GetScissor() const;
    grfx::Viewport GetViewport(float minDepth = 0.0f, float maxDepth = 1.0f) const;

    uint32_t                     GetProcessId() const;
    fs::path                     GetApplicationPath() const;
    const std::vector<fs::path>& GetAssetDirs() const { return mAssetDirs; }
    void                         AddAssetDir(const fs::path& path, bool insertAtFront = false);

    // Returns the first valid subPath in the asset directories list
    //
    // Example(s):
    //
    //    mAssetDirs = {"/a/valid/system/path",
    //                  "/another/valid/system/path",
    //                  "/some/valid/system/path"};
    //
    //    GetAssetPath("file.ext") - returns the full path to file.ext if it exists
    //      in any of the paths in mAssetDirs on the file system.
    //      Search starts with mAssetsDir[0].
    //
    //    GetAssetPath("subdir") - returns the full path to subdir if it exists
    //      in any of the paths in mAssetDirs on the file system.
    //      Search starts with mAssetsDir[0].
    //
    fs::path GetAssetPath(const fs::path& subPath) const;

    // Loads a DXBC, DXIL, or SPV shader from baseDir
    //
    // 'baseDir' is path to the directory that contains dxbc, dxil, and spv subdirectories.
    // 'baseName' is the filename WITHOUT the dxbc, dxil, and spv extension.
    //
    // Example(s):
    //   LoadShader("shaders", "Texture.vs")
    //     - loads shader file: shaders/dxbc/Texture.vs.dxbc for API_DX_12_0, API_DX_12_1 if enableDXIL = false
    //     - loads shader file: shaders/dxil/Texture.vs.dxil for API_DX_12_0, API_DX_12_1 if enableDXIL = true
    //     - loads shader file: shaders/spv/Texture.vs.spv   for API_VK_1_1, API_VK_1_2
    //
    //   LoadShader("some/path/shaders", "Texture.vs")
    //     - loads shader file: some/path/shaders/dxbc/Texture.vs.dxbc for API_DX_12_0, API_DX_12_1 if enableDXIL = false
    //     - loads shader file: some/path/shaders/dxil/Texture.vs.dxil for API_DX_12_0, API_DX_12_1 if enableDXIL = true
    //     - loads shader file: some/path/shaders/spv/Texture.vs.spv   for API_VK_1_1, API_VK_1_2
    //
    std::vector<char> LoadShader(const fs::path& baseDir, const std::string& baseName) const;

    void*              GetWindow() const { return mWindow; }
    grfx::InstancePtr  GetInstance() const { return mInstance; }
    grfx::DevicePtr    GetDevice() const { return mDevice; }
    grfx::QueuePtr     GetGraphicsQueue(uint32_t index = 0) const { return GetDevice()->GetGraphicsQueue(index); }
    grfx::QueuePtr     GetComputeQueue(uint32_t index = 0) const { return GetDevice()->GetComputeQueue(index); }
    grfx::QueuePtr     GetTransferQueue(uint32_t index = 0) const { return GetDevice()->GetTransferQueue(index); }
    grfx::SwapchainPtr GetSwapchain() const { return mSwapchain; }

    float    GetElapsedSeconds() const;
    uint64_t GetFrameCount() const { return mFrameCount; }
    uint32_t GetNumFramesInFlight() const { return mSettings.grfx.numFramesInFlight; }
    uint32_t GetFrameInFlightIndex() const { return static_cast<uint32_t>(mFrameCount % mSettings.grfx.numFramesInFlight); }

private:
    void   InternalCtor();
    void   InitializeAssetDirs();
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
    std::string                mDecoratedApiName;
    std::vector<fs::path>      mAssetDirs;
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
