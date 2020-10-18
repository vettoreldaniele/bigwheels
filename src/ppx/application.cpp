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

    InitializeAssetDirs();
}

void Application::InitializeAssetDirs()
{
    fs::path appPath = GetApplicationPath();
    PPX_LOG_INFO("Application path: " << appPath);
    fs::path baseDir = appPath.parent();
    AddAssetDir(baseDir);
    size_t n = baseDir.part_count();
    for (size_t i = 0; i < n; ++i) {
        fs::path assetDir = baseDir / "assets";
        if (fs::exists(assetDir)) {
            AddAssetDir(assetDir);
            PPX_LOG_INFO("Added asset path: " << assetDir);
        }
        baseDir = baseDir.parent();
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

Result Application::InitializeImGui()
{
    switch (mSettings.grfx.api) {
        default: {
            PPX_ASSERT_MSG(false, "[imgui] unknown graphics API");
            return ppx::ERROR_UNSUPPORTED_API;
        } break;

        case grfx::API_VK_1_1:
        case grfx::API_VK_1_2: {
            mImGui = std::unique_ptr<ImGuiImpl>(new ImGuiImplVk());
        } break;

#if defined(PPX_D3D12)
        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            mImGui = std::unique_ptr<ImGuiImpl>(new ImGuiImplDx());
        } break;
#endif // defined(PPX_D3D12)
    }

    Result ppxres = mImGui->Init(this);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Application::ShutdownImGui()
{
    if (mImGui) {
        mImGui->Shutdown(this);
        mImGui.reset();
    }
}

void Application::StopGrfx()
{
    if (mDevice) {
        mDevice->WaitIdle();
    }
}

void Application::ShutdownGrfx()
{
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

    // Decorated window title
    std::stringstream windowTitle;
    windowTitle << mSettings.window.title << " | " << ToString(mSettings.grfx.api) << " | " << mDevice->GetDeviceName();

    GLFWwindow* pWindow = glfwCreateWindow(
        static_cast<int>(mSettings.window.width),
        static_cast<int>(mSettings.window.height),
        windowTitle.str().c_str(),
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

    // Decorate DX's API name with shader bytecode mode
    std::stringstream ss;
    ss << ToString(mSettings.grfx.api);
    switch (mSettings.grfx.api) {
        default: break;
        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            if (mSettings.grfx.enableDXIL) {
                ss << " (DXIL)";
            }
            else {
                ss << " (DXBC)";
            }
        } break;
    }
    mDecoratedApiName = ss.str();
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

void Application::DrawImGui(grfx::CommandBuffer* pCommandBuffer)
{
    if (!mImGui) {
        return;
    }

    mImGui->Render(pCommandBuffer);
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

    // Setup ImGui
    ppxres = InitializeImGui();
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
        // Frame start
        mFrameStartTime = static_cast<float>(mTimer.MillisSinceStart());

        // Poll events
        glfwPollEvents();

        // Start new Imgui frame
        if (mImGui) {
            mImGui->NewFrame();
        }

        // Call render
        DispatchRender();

        // Frame end
        mFrameCount        = mFrameCount + 1;
        mAverageFPS        = static_cast<float>(mFrameCount / mTimer.SecondsSinceStart());
        mFrameEndTime      = static_cast<float>(mTimer.MillisSinceStart());
        mPreviousFrameTime = mFrameEndTime - mFrameStartTime;

        // Pace ffames - if needed
        if (mSettings.grfx.pacedFrameRate > 0) {
            if (mFrameCount > 0) {
                double currentTime  = mTimer.SecondsSinceStart();
                double pacedFPS     = 1.0 / static_cast<double>(mSettings.grfx.pacedFrameRate);
                double expectedTime = mFirstFrameTime + (mFrameCount * pacedFPS);
                double diff         = expectedTime - currentTime;
                if (diff > 0) {
                    Timer::SleepSeconds(diff);
                }
            }
            else {
                mFirstFrameTime = mTimer.SecondsSinceStart();
            }
        }
    }
    // ---------------------------------------------------------------------------------------------
    // Main loop [END]
    // ---------------------------------------------------------------------------------------------

    // Stop graphics first before shutting down to make sure
    // that there aren't any command buffers in flight.
    //
    StopGrfx();

    // Call shutdown
    DispatchShutdown();

    // Shutdown Imgui
    ShutdownImGui();

    // Shutdown graphics
    ShutdownGrfx();

    // Destroy window
    DestroyPlatformWindow();

    // Success
    return EXIT_SUCCESS;
}

grfx::Rect Application::GetScissor() const
{
    grfx::Rect rect = {};
    rect.x          = 0;
    rect.y          = 0;
    rect.width      = GetWindowWidth();
    rect.height     = GetWindowHeight();
    return rect;
}

grfx::Viewport Application::GetViewport(float minDepth, float maxDepth) const
{
    grfx::Viewport viewport = {};
    viewport.x              = 0.0f;
    viewport.y              = 0.0f;
    viewport.width          = static_cast<float>(GetWindowWidth());
    viewport.height         = static_cast<float>(GetWindowHeight());
    viewport.minDepth       = minDepth;
    viewport.maxDepth       = maxDepth;
    return viewport;
}

float Application::GetElapsedSeconds() const
{
    return static_cast<float>(mTimer.SecondsSinceStart());
}

uint32_t Application::GetProcessId() const
{
    uint32_t pid = UINT32_MAX;
#if defined(PPX_LINUX)
    pid = static_cast<uint32_t>(getpid());
#elif defined(PPX_MSW)
    pid = static_cast<uint32_t>(::GetCurrentProcessId());
#endif
    return pid;
}

fs::path Application::GetApplicationPath() const
{
    fs::path path;
#if defined(PPX_LINUX) || defined(PPX_GGP)
    char buf[PATH_MAX];
    std::memset(buf, 0, PATH_MAX);
    readlink("/proc/self/exe", buf, PATH_MAX);
    path = fs::path(buf);
#elif defined(PPX_MSW)
    HMODULE this_win32_module = GetModuleHandleA(nullptr);
    char buf[MAX_PATH];
    std::memset(buf, 0, MAX_PATH);
    GetModuleFileNameA(this_win32_module, buf, MAX_PATH);
    path = fs::path(buf);
#else
#error "not implemented"
#endif
    return path;
}

void Application::AddAssetDir(const fs::path& path, bool insertAtFront)
{
    auto it = Find(mAssetDirs, path);
    if (it != std::end(mAssetDirs)) {
        return;
    }

    if (!fs::is_directory(path)) {
        return;
    }

    mAssetDirs.push_back(path);

    if (insertAtFront) {
        // Rotate to front
        std::rotate(
            std::rbegin(mAssetDirs),
            std::rbegin(mAssetDirs) + 1,
            std::rend(mAssetDirs));
    }
}

fs::path Application::GetAssetPath(const fs::path& subPath) const
{
    fs::path assetPath;
    for (auto& assetDir : mAssetDirs) {
        fs::path path = assetDir / subPath;
        if (fs::exists(path)) {
            assetPath = path;
            break;
        }
    }
    return assetPath;
}

std::vector<char> Application::LoadShader(const fs::path& baseDir, const std::string& baseName) const
{
    fs::path filePath = baseDir;
    switch (mSettings.grfx.api) {
        default: {
            PPX_ASSERT_MSG(false, "unsupported API");
        } break;

        case grfx::API_VK_1_1:
        case grfx::API_VK_1_2: {
            filePath = (filePath / "spv" / baseName).append_extension(".spv");
        } break;

        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            if (mSettings.grfx.enableDXIL) {
                filePath = (filePath / "dxil" / baseName).append_extension(".dxil");
            }
            else {
                filePath = (filePath / "dxbc" / baseName).append_extension(".dxbc");
            }
        } break;
    }

    if (!fs::exists(filePath)) {
        PPX_ASSERT_MSG(false, "shader file not found: " << filePath);
    }

    std::vector<char> bytecode = fs::load_file(filePath);
    return bytecode;
}

void Application::DrawDebugInfo(std::function<void(void)> drawAdditionalFn)
{
    if (!mImGui) {
        return;
    }

    if (ImGui::Begin("Debug Info")) {
        ImGui::Columns(2);

        // Application PID
        {
            ImGui::Text("Application PID");
            ImGui::NextColumn();
            ImGui::Text("%d", GetProcessId());
            ImGui::NextColumn();
        }

        ImGui::Separator();

        // API
        {
            ImGui::Text("API");
            ImGui::NextColumn();
            ImGui::Text("%s", mDecoratedApiName.c_str());
            ImGui::NextColumn();
        }

        // GPU
        {
            ImGui::Text("GPU");
            ImGui::NextColumn();
            ImGui::Text("%s", GetDevice()->GetDeviceName());
            ImGui::NextColumn();
        }

        ImGui::Separator();

        // Average FPS
        {
            ImGui::Text("Frame Count");
            ImGui::NextColumn();
            ImGui::Text("%d", mFrameCount);
            ImGui::NextColumn();
        }

        // Average FPS
        {
            ImGui::Text("Average FPS");
            ImGui::NextColumn();
            ImGui::Text("%f", mAverageFPS);
            ImGui::NextColumn();
        }

        // Previous frame time
        {
            ImGui::Text("Previous Frame Time");
            ImGui::NextColumn();
            ImGui::Text("%f ms", mPreviousFrameTime);
            ImGui::NextColumn();
        }

        ImGui::Columns(1);

        // Draw additional elements
        if (drawAdditionalFn) {
            drawAdditionalFn();
        }
    }
    ImGui::End();
}

} // namespace ppx
