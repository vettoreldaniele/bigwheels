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

/** @enum MouseButton
 *
 */
enum MouseButton
{
    MOUSE_BUTTON_LEFT   = 0x00000001,
    MOUSE_BUTTON_RIGHT  = 0x00000002,
    MOUSE_BUTTON_MIDDLE = 0x00000004,
};

/* @enum CursorMode
 *
 */
enum CursorMode
{
    CURSOR_MODE_VISIBLE = 0,
    CURSOR_MODE_HIDDEN,
    CURSOR_MODE_CAPTURED,
};

//! @enum Keyboard
//!
//!
enum KeyCode
{
    KEY_UNDEFINED   = 0,
    KEY_RANGE_FIRST = 32,
    KEY_SPACE       = KEY_RANGE_FIRST,
    KEY_APOSTROPHE,
    KEY_COMMA,
    KEY_MINUS,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_SEMICOLON,
    KEY_EQUAL,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_LEFT_BRACKET,
    KEY_BACKSLASH,
    KEY_RIGHT_BRACKET,
    KEY_GRAVE_ACCENT,
    KEY_WORLD_1,
    KEY_WORLD_2,
    KEY_ESCAPE,
    KEY_ENTER,
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_INSERT,
    KEY_DELETE,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_HOME,
    KEY_END,
    KEY_CAPS_LOCK,
    KEY_SCROLL_LOCK,
    KEY_NUM_LOCK,
    KEY_PRINT_SCREEN,
    KEY_PAUSE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_F21,
    KEY_F22,
    KEY_F23,
    KEY_F24,
    KEY_F25,
    KEY_KEY_PAD_0,
    KEY_KEY_PAD_1,
    KEY_KEY_PAD_2,
    KEY_KEY_PAD_3,
    KEY_KEY_PAD_4,
    KEY_KEY_PAD_5,
    KEY_KEY_PAD_6,
    KEY_KEY_PAD_7,
    KEY_KEY_PAD_8,
    KEY_KEY_PAD_9,
    KEY_KEY_PAD_DECIMAL,
    KEY_KEY_PAD_DIVIDE,
    KEY_KEY_PAD_MULTIPLY,
    KEY_KEY_PAD_SUBTRACT,
    KEY_KEY_PAD_ADD,
    KEY_KEY_PAD_ENTER,
    KEY_KEY_PAD_EQUAL,
    KEY_LEFT_SHIFT,
    KEY_LEFT_CONTROL,
    KEY_LEFT_ALT,
    KEY_LEFT_SUPER,
    KEY_RIGHT_SHIFT,
    KEY_RIGHT_CONTROL,
    KEY_RIGHT_ALT,
    KEY_RIGHT_SUPER,
    KEY_MENU,
    KEY_RANGE_LAST = KEY_MENU,
    TOTAL_KEY_COUNT,
};

//! @struct KeyState
//!
//!
struct KeyState
{
    bool  down     = false;
    float timeDown = FLT_MAX;
};

// -------------------------------------------------------------------------------------------------
// Application
// -------------------------------------------------------------------------------------------------

//! @class ApplicationSettings
//!
//!
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

//! @class Application
//!
//!
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
    virtual void Move(int32_t x, int32_t y) {}                                                  // Window move event
    virtual void Resize(uint32_t width, uint32_t height) {}                                     // Window resize event
    virtual void KeyDown(KeyCode key) {}                                                        // Key down event
    virtual void KeyUp(KeyCode key) {}                                                          // Key up event
    virtual void MouseMove(int32_t x, int32_t y, int32_t dx12, int32_t dy, uint32_t buttons) {} // Mouse move event
    virtual void MouseDown(int32_t x, int32_t y, uint32_t buttons) {}                           // Mouse down event
    virtual void MouseUp(int32_t x, int32_t y, uint32_t buttons) {}                             // Mouse up event
    virtual void Scroll(float dx12, float dy) {}                                                // Mouse wheel or touchpad scroll event
    virtual void Render() {}

protected:
    virtual void DispatchConfig();
    virtual void DispatchSetup();
    virtual void DispatchShutdown();
    virtual void DispatchMove(int32_t x, int32_t y);
    virtual void DispatchResize(uint32_t width, uint32_t height);
    virtual void DispatchKeyDown(KeyCode key);
    virtual void DispatchKeyUp(KeyCode key);
    virtual void DispatchMouseMove(int32_t x, int32_t y, int32_t dx12, int32_t dy, uint32_t buttons);
    virtual void DispatchMouseDown(int32_t x, int32_t y, uint32_t buttons);
    virtual void DispatchMouseUp(int32_t x, int32_t y, uint32_t buttons);
    virtual void DispatchScroll(float dx12, float dy);
    virtual void DispatchRender();

    void DrawImGui(grfx::CommandBuffer* pCommandBuffer);
    void DrawDebugInfo(std::function<void(void)> drawAdditionalFn = []() {});
    void DrawProfilerGrfxApiFunctions();

public:
    int  Run(int argc, char** argv);
    void Quit();

    std::vector<const char*> GetCommandLineArgs() const;

    uint32_t       GetWindowWidth() const { return mSettings.window.width; }
    uint32_t       GetWindowHeight() const { return mSettings.window.height; }
    float          GetWindowAspect() const { return static_cast<float>(mSettings.window.width) / static_cast<float>(mSettings.window.height); }
    grfx::Rect     GetScissor() const;
    grfx::Viewport GetViewport(float minDepth = 0.0f, float maxDepth = 1.0f) const;

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
    Result            CreateShader(const fs::path& baseDir, const std::string& baseName, grfx::ShaderModule** ppShaderModule) const;

    void*              GetWindow() const { return mWindow; }
    grfx::InstancePtr  GetInstance() const { return mInstance; }
    grfx::DevicePtr    GetDevice() const { return mDevice; }
    grfx::QueuePtr     GetGraphicsQueue(uint32_t index = 0) const { return GetDevice()->GetGraphicsQueue(index); }
    grfx::QueuePtr     GetComputeQueue(uint32_t index = 0) const { return GetDevice()->GetComputeQueue(index); }
    grfx::QueuePtr     GetTransferQueue(uint32_t index = 0) const { return GetDevice()->GetTransferQueue(index); }
    grfx::SwapchainPtr GetSwapchain() const { return mSwapchain; }

    float    GetElapsedSeconds() const;
    uint64_t GetFrameCount() const { return mFrameCount; }
    float    GetAverageFPS() const { return mAverageFPS; }
    uint32_t GetNumFramesInFlight() const { return mSettings.grfx.numFramesInFlight; }
    uint32_t GetInFlightFrameIndex() const { return static_cast<uint32_t>(mFrameCount % mSettings.grfx.numFramesInFlight); }
    uint32_t GetPreviousInFlightFrameIndex() const { return static_cast<uint32_t>((mFrameCount - 1) % mSettings.grfx.numFramesInFlight); }

    const KeyState& GetKeyState(KeyCode code) const;
    float2          GetNormalizedDeviceCoordinates(int32_t x, int32_t y) const;

private:
    void   InternalCtor();
    void   InitializeAssetDirs();
    Result InitializePlatform();
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
    friend struct WindowEvents;
    //
    // These functions exists so that applications can override the
    // corresponding Dispatch* methods without interfering with the
    // internal bookkeeping the app needs to do for these events.
    //
    void MoveCallback(int32_t x, int32_t y);
    void ResizeCallback(uint32_t width, uint32_t height);
    void KeyDownCallback(KeyCode key);
    void KeyUpCallback(KeyCode key);
    void MouseMoveCallback(int32_t x, int32_t y, uint32_t buttons);
    void MouseDownCallback(int32_t x, int32_t y, uint32_t buttons);
    void MouseUpCallback(int32_t x, int32_t y, uint32_t buttons);
    void ScrollCallback(float dx12, float dy);

private:
    std::vector<std::string>   mCommandLineArgs;
    ApplicationSettings        mSettings = {};
    std::string                mDecoratedApiName;
    Timer                      mTimer;
    void*                      mWindow                     = nullptr;
    bool                       mWindowSurfaceInvalid       = false;
    KeyState                   mKeyStates[TOTAL_KEY_COUNT] = {false, 0.0f};
    int32_t                    mPreviousMouseX             = INT32_MAX;
    int32_t                    mPreviousMouseY             = INT32_MAX;
    bool                       mRunning                    = true;
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

const char* GetKeyCodeString(KeyCode code);

} // namespace ppx

#endif // ppx_application_h
