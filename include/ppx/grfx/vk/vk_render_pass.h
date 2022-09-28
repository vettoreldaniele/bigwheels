#ifndef ppx_grfx_vk_render_pass_h
#define ppx_grfx_vk_render_pass_h

#include "ppx/grfx/vk/vk_config.h"
#include "ppx/grfx/grfx_render_pass.h"

namespace ppx {
namespace grfx {
namespace vk {

class RenderPass
    : public grfx::RenderPass
{
public:
    RenderPass() {}
    virtual ~RenderPass() {}

    VkRenderPassPtr  GetVkRenderPass() const { return mRenderPass; }
    VkFramebufferPtr GetVkFramebuffer() const { return mFramebuffer; }

protected:
    virtual Result CreateApiObjects(const grfx::internal::RenderPassCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result CreateRenderPass(const grfx::internal::RenderPassCreateInfo* pCreateInfo);
    Result CreateFramebuffer(const grfx::internal::RenderPassCreateInfo* pCreateInfo);

private:
    VkRenderPassPtr  mRenderPass;
    VkFramebufferPtr mFramebuffer;
};

// -------------------------------------------------------------------------------------------------

VkResult CreateTransientRenderPass(
    VkDevice              device,
    uint32_t              renderTargetCount,
    const VkFormat*       pRenderTargetFormats,
    VkFormat              depthStencilFormat,
    VkSampleCountFlagBits sampleCount,
    VkRenderPass*         pRenderPass);

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_render_pass_h
