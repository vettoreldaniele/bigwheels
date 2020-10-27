#ifndef ppx_grfx_vk_swapchain_h
#define ppx_grfx_vk_swapchain_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_swapchain.h"

namespace ppx {
namespace grfx {
namespace vk {

//! @class Surface
//!
//!
class Surface
    : public grfx::Surface
{
public:
    Surface() {}
    virtual ~Surface() {}

    VkSurfacePtr GetVkSurface() const { return mSurface; }

    const VkSurfaceCapabilitiesKHR&       GetCapabilities() const { return mCapabilities; }
    const std::vector<VkSurfaceFormatKHR> GetSurfaceFormats() const { return mSurfaceFormats; }

    virtual uint32_t GetMinImageWidth() const override;
    virtual uint32_t GetMinImageHeight() const override;
    virtual uint32_t GetMinImageCount() const override;

protected:
    virtual Result CreateApiObjects(const grfx::SurfaceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkSurfacePtr                    mSurface;
    VkSurfaceCapabilitiesKHR        mCapabilities = {};
    std::vector<VkSurfaceFormatKHR> mSurfaceFormats;
    std::vector<uint32_t>           mPresentableQueueFamilies;
    std::vector<VkPresentModeKHR>   mPresentModes;
};

// -------------------------------------------------------------------------------------------------

//! @class Swapchain
//!
//!
class Swapchain
    : public grfx::Swapchain
{
public:
    Swapchain() {}
    virtual ~Swapchain() {}

    VkSwapchainPtr GetVkSwapchain() const { return mSwapchain; }

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
    VkSwapchainPtr mSwapchain;
    VkQueuePtr     mQueue;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_swapchain_h
