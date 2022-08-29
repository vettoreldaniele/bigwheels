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
  #if (defined(PPX_DXVK) || defined(PPX_DXIIVK))
    HINSTANCE             hinstance;
    HWND                  hwnd;
  #endif
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

    virtual uint32_t GetMinImageWidth() const  = 0;
    virtual uint32_t GetMinImageHeight() const = 0;
    virtual uint32_t GetMinImageCount() const  = 0;
    virtual uint32_t GetMaxImageWidth() const  = 0;
    virtual uint32_t GetMaxImageHeight() const = 0;
    virtual uint32_t GetMaxImageCount() const  = 0;
};

// -------------------------------------------------------------------------------------------------

//! @struct SwapchainCreateInfo
//!
//! NOTE: The member \b imageCount is the minimum image count.
//!       On Vulkan, the actual number of images created by
//!       the swapchain may be greater than this value.
//!
struct SwapchainCreateInfo
{
    grfx::Queue*      pQueue      = nullptr;
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

    uint32_t     GetWidth() const { return mCreateInfo.width; }
    uint32_t     GetHeight() const { return mCreateInfo.height; }
    uint32_t     GetImageCount() const { return mCreateInfo.imageCount; }
    grfx::Format GetColorFormat() const { return mCreateInfo.colorFormat; }
    grfx::Format GetDepthFormat() const { return mCreateInfo.depthFormat; }

    Result GetColorImage(uint32_t imageIndex, grfx::Image** ppImage) const;
    Result GetDepthImage(uint32_t imageIndex, grfx::Image** ppImage) const;
    Result GetRenderPass(uint32_t imageIndex, grfx::AttachmentLoadOp loadOp, grfx::RenderPass** ppRenderPass) const;

    // Convenience functions - returns empty object if index is invalid
    grfx::ImagePtr      GetColorImage(uint32_t imageIndex) const;
    grfx::ImagePtr      GetDepthImage(uint32_t imageIndex) const;
    grfx::RenderPassPtr GetRenderPass(uint32_t imageIndex, grfx::AttachmentLoadOp loadOp = grfx::ATTACHMENT_LOAD_OP_CLEAR) const;

    virtual Result AcquireNextImage(
        uint64_t         timeout,    // Nanoseconds
        grfx::Semaphore* pSemaphore, // Wait sempahore
        grfx::Fence*     pFence,     // Wait fence
        uint32_t*        pImageIndex) = 0;

    virtual Result Present(
        uint32_t                      imageIndex,
        uint32_t                      waitSemaphoreCount,
        const grfx::Semaphore* const* ppWaitSemaphores) = 0;

    uint32_t GetCurrentImageIndex() const { return currentImageIndex; }

protected:
    virtual Result Create(const grfx::SwapchainCreateInfo* pCreateInfo) override;
    virtual void   Destroy() override;
    friend class grfx::Device;

protected:
    grfx::QueuePtr                   mQueue;
    std::vector<grfx::ImagePtr>      mDepthImages;
    std::vector<grfx::ImagePtr>      mColorImages;
    std::vector<grfx::RenderPassPtr> mClearRenderPasses;
    std::vector<grfx::RenderPassPtr> mLoadRenderPasses;

    // Keeps track of the image index returned by the
    // last AcquireNextImage call.
    uint32_t currentImageIndex = 0;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_swapchain_h
