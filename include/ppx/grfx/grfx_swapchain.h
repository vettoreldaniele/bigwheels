#ifndef ppx_grfx_swapchain_h
#define ppx_grfx_swapchain_h

#include "ppx/grfx/000_grfx_config.h"

// clang-format off
#if defined(PPX_GGP)
#   include <ggp_c/vulkan_types.h>
#elif defined(PPX_LINUX_XCB)
#   include <xcb/xcb.h>
#elif defined(PPX_LINUX_XLIB)
#   include <X11/Xlib.h>
#elif defined(PPX_LINUX_WAYLAND)
#   include <wayland-client.h>
#elif defined(PPX_MSW)
#   include <Windows.h>
#endif
// clang-format on

namespace ppx {
namespace grfx {

//! @struct SurfaceCreateInfo
//!
//!
struct SurfaceCreateInfo
{
    // clang-format off
    grfx::Gpu*            pGpu = nullptr;
#if defined(PPX_GGP)
    // Nothing
#elif defined(PPX_LINUX_WAYLAND)
    struct wl_display*    display;
    struct wl_surface*    surface;
#elif defined(PPX_LINUX_XCB)
    xcb_connection_t*     connection;
    xcb_window_t          window;
#elif defined(PPX_LINUX_XLIB)
    Display*              dpy;
    Window                window;
#elif defined(PPX_MSW)
    HINSTANCE             hinstance;
    HWND                  hwnd;
#endif
    // clang-format on
};

//! @class Surface
//!
//!
class Surface
    : public grfx::InstanceObject<grfx::SurfaceCreateInfo>
{
public:
    Surface() {}
    virtual ~Surface() {}
};

// -------------------------------------------------------------------------------------------------

//! @struct SwapchainCreateInfo
//!
//!
struct SwapchainCreateInfo
{
    grfx::Surface*    pSurface    = nullptr;
    uint32_t          width       = 0;
    uint32_t          height      = 0;
    grfx::Format      colorFormat = grfx::FORMAT_UNDEFINED;
    grfx::Format      depthFormat = grfx::FORMAT_UNDEFINED;
    uint32_t          imageCount  = 0;
    grfx::PresentMode presentMode = grfx::PRESENT_MODE_IMMEDIATE;
};

//! @class Swapchain
//!
//!
class Swapchain
    : public grfx::DeviceObject<grfx::SwapchainCreateInfo>
{
public:
    Swapchain() {}
    virtual ~Swapchain() {}

    uint32_t GetImageCount() const { return mCreateInfo.imageCount; }

    Result GetImage(uint32_t imageIndex, grfx::Image** ppImage) const;
    Result GetRenderPass(uint32_t imageIndex, grfx::RenderPass** ppRenderPass) const;

    // Convenience functions - returns empty object if index is invalid
    grfx::ImagePtr      GetImage(uint32_t imageIndex) const;
    grfx::RenderPassPtr GetRenderPass(uint32_t imageIndex) const;

    virtual Result AcquireNextImage(uint64_t timeout, const grfx::Semaphore* pSemaphore, const grfx::Fence* pFence, uint32_t* pImageIndex) = 0;

protected:
    virtual Result Create(const grfx::SwapchainCreateInfo* pCreateInfo) override;
    friend class grfx::Device;

protected:
    std::vector<grfx::ImagePtr>      mImages;
    std::vector<grfx::RenderPassPtr> mRenderPasses;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_swapchain_h
