#ifndef ppx_grfx_dx_swapchain_h
#define ppx_grfx_dx_swapchain_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_swapchain.h"

namespace ppx {
namespace grfx {
namespace dx {

class Surface
    : public grfx::Surface
{
public:
    Surface() {}
    virtual ~Surface() {}

    HWND GetWindowHandle() const { return mWindowHandle; }

    virtual uint32_t GetMinImageCount() const override { return 1; }

protected:
    virtual Result CreateApiObjects(const grfx::SurfaceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    HWND mWindowHandle = nullptr;
};

// -------------------------------------------------------------------------------------------------

class Swapchain
    : public grfx::Swapchain
{
public:
    Swapchain() {}
    virtual ~Swapchain() {}

    virtual Result AcquireNextImage(
        uint64_t         timeout,
        grfx::Semaphore* pSemaphore,
        grfx::Fence*     pFence,
        uint32_t*        pImageIndex) override;

    virtual Result Present(
        uint32_t                      imageIndex,
        uint32_t                      waitSemaphoreCount,
        const grfx::Semaphore* const* ppWaitSemaphores) override;

protected:
    virtual Result CreateApiObjects(const grfx::SwapchainCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    DXGISwapChainPtr     mSwapchain;
    HANDLE               mFrameLatencyWaitableObject = nullptr;
    D3D12CommandQueuePtr mQueue;

    //
    // Store sync internval so we can control its behavior based
    // on which present mode the client requested.
    //
    // See:
    //   https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
    //
    UINT mSyncInterval = 1;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_swapchain_h
