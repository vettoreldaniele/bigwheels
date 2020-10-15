#include "ppx/application.h"

namespace ppx {

const char*    kDefaultAppName      = "PPX Application";
const uint32_t kDefaultWindowWidth  = 640;
const uint32_t kDefaultWindowHeight = 480;

static Application* sApplicationInstance = nullptr;

Application::Application()
{
    InternalCtor();

    mSettings.appName       = kDefaultAppName;
    mSettings.window.width  = kDefaultWindowWidth;
    mSettings.window.height = kDefaultWindowHeight;
}

Application::Application(uint32_t windowWidth, uint32_t windowHeight, const char* windowTitle)
{
    InternalCtor();

    mSettings.appName       = windowTitle;
    mSettings.window.width  = windowWidth;
    mSettings.window.height = windowHeight;
    mSettings.window.title  = windowTitle;
}

Application::~Application()
{
    if (sApplicationInstance == this) {
        sApplicationInstance = nullptr;
    }
}

void Application::InternalCtor()
{
    if (IsNull(sApplicationInstance)) {
        sApplicationInstance = this;
    }
}

Result Application::InitializeGrfxDevice()
{
    // Instance
    {
        if (mInstance) {
            return ppx::ERROR_SINGLE_INIT_ONLY;
        }

        grfx::InstanceCreateInfo ci = {};
        ci.api                      = mSettings.grfx.api;
        ci.createDevices            = false;
        ci.enableDebug              = mSettings.grfx.enableDebug;
        ci.enableSwapchain          = true;
        ci.applicationName          = mSettings.appName;
        ci.engineName               = mSettings.appName;

        Result ppxres = grfx::CreateInstance(&ci, &mInstance);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::CreateInstance failed");
            return ppxres;
        }
    }

    // Device
    {
        if (mDevice) {
            return ppx::ERROR_SINGLE_INIT_ONLY;
        }

        grfx::GpuPtr gpu;
        Result       ppxres = mInstance->GetGpu(0, &gpu);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Instance::GetGpu failed");
            return ppxres;
        }

        grfx::DeviceCreateInfo ci = {};
        ci.pGpu                   = gpu;
        ci.graphicsQueueCount     = 1;
        ci.computeQueueCount      = 1;
        ci.transferQueueCount     = 1;
        ci.vulkanExtensions       = {};
        ci.pVulkanDeviceFeatures  = nullptr;

        ppxres = mInstance->CreateDevice(&ci, &mDevice);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Instance::CreateDevice failed");
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

Result Application::InitializeGrfxSurface()
{
    // Surface
    {
        grfx::SurfaceCreateInfo ci = {};
        ci.pGpu                    = mDevice->GetGpu();
#if defined(PPX_GGP)
        // Nothing to do
#elif defined(PPX_LINUX_XCB)
        ci.connection = XGetXCBConnection(glfwGetX11Display());
        ci.window     = glfwGetX11Window(m_window);
#elif defined(PPX_LINUX_XLIB)
#error "Xlib not implemented"
#elif defined(PPX_LINUX_WAYLAND)
#error "Wayland not implemented"
#elif defined(PPX_MSW)
        ci.hinstance = ::GetModuleHandle(nullptr);
        ci.hwnd      = glfwGetWin32Window(static_cast<GLFWwindow*>(mWindow));
#endif

        Result ppxres = mInstance->CreateSurface(&ci, &mSurface);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Instance::CreateSurface failed");
            return ppxres;
        }
    }

    // Swapchain
    {
        grfx::SwapchainCreateInfo ci = {};
        ci.pQueue                    = mDevice->GetGraphicsQueue();
        ci.pSurface                  = mSurface;
        ci.width                     = mSettings.window.width;
        ci.height                    = mSettings.window.height;
        ci.colorFormat               = mSettings.grfx.swapchain.colorFormat;
        ci.depthFormat               = mSettings.grfx.swapchain.depthFormat;
        ci.imageCount                = mSettings.grfx.swapchain.imageCount;
        ci.presentMode               = grfx::PRESENT_MODE_IMMEDIATE;

        Result ppxres = mDevice->CreateSwapchain(&ci, &mSwapchain);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Device::CreateSwapchain failed");
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

void Application::ShutdownGrfx()
{
    // Wait for device idle
    if (mDevice) {
        mDevice->WaitIdle();
    }

    if (mInstance) {
        if (mSwapchain) {
            mDevice->DestroySwapchain(mSwapchain);
            mSwapchain.Reset();
        }

        if (mDevice) {
            mInstance->DestroyDevice(mDevice);
            mDevice.Reset();
        }

        if (mSurface) {
            mInstance->DestroySurface(mSurface);
            mSurface.Reset();
        }

        grfx::DestroyInstance(mInstance);
        mInstance.Reset();
    }
}

Result Application::CreatePlatformWindow()
{
    int res = glfwInit();
    if (res != GLFW_TRUE) {
        PPX_ASSERT_MSG(false, "glfwInit failed");
        return ppx::ERROR_GLFW_INIT_FAILED;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, mSettings.window.resizable ? GLFW_TRUE : GLFW_FALSE);

    GLFWwindow* pWindow = glfwCreateWindow(
        static_cast<int>(mSettings.window.width),
        static_cast<int>(mSettings.window.height),
        mSettings.window.title.c_str(),
        nullptr,
        nullptr);
    if (IsNull(pWindow)) {
        PPX_ASSERT_MSG(false, "glfwCreateWindow failed");
        return ppx::ERROR_GLFW_CREATE_WINDOW_FAILED;
    }

    mWindow = static_cast<void*>(pWindow);

    return ppx::SUCCESS;
}

void Application::DestroyPlatformWindow()
{
    if (!IsNull(mWindow)) {
        GLFWwindow* pWindow = static_cast<GLFWwindow*>(mWindow);
        glfwDestroyWindow(pWindow);
        mWindow = nullptr;
    }
}

void Application::DispatchConfig()
{
    Config(mSettings);

    if (mSettings.appName.empty()) {
        mSettings.appName = "PPX Application";
    }

    if (mSettings.window.title.empty()) {
        mSettings.window.title = mSettings.appName;
    }
}

void Application::DispatchSetup()
{
    Setup();
}

void Application::DispatchShutdown()
{
    Shutdown();
}

void Application::DispatchRender()
{
    Render();
}

bool Application::IsRunning() const
{
    bool isRunning = (glfwWindowShouldClose(static_cast<GLFWwindow*>(mWindow)) == 0);
    return isRunning;
}

int Application::Run(int argc, char** argv)
{
    // Only allow one instance of Application. Since we can't stop
    // the app in the ctor - stop it here.
    //
    if (this != sApplicationInstance) {
        return false;
    }

    // Call config
    DispatchConfig();

    // Create graphics instance
    Result ppxres = InitializeGrfxDevice();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Create window
    ppxres = CreatePlatformWindow();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Create surface
    ppxres = InitializeGrfxSurface();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Call setup
    DispatchSetup();

    // ---------------------------------------------------------------------------------------------
    // Main loop [BEGIN]
    // ---------------------------------------------------------------------------------------------

    // Initialize and start timer
    ppx::TimerResult tmres = ppx::Timer::InitializeStaticData();
    if (tmres != ppx::TIMER_RESULT_SUCCESS) {
        return EXIT_FAILURE;
    }
    tmres = mTimer.Start();

    mRunning = true;
    while (IsRunning()) {
        // Poll events
        glfwPollEvents();

        // Call render
        DispatchRender();
    }
    // ---------------------------------------------------------------------------------------------
    // Main loop [END]
    // ---------------------------------------------------------------------------------------------

    // Call shutdown
    DispatchShutdown();

    // Shutdown graphics
    ShutdownGrfx();

    // Destroy window
    DestroyPlatformWindow();

    // Success
    return EXIT_SUCCESS;
}

float Application::GetElapsedSeconds() const
{
    return static_cast<float>(mTimer.SecondsSinceStart());
}

} // namespace ppx
