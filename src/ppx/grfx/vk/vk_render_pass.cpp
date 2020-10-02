#include "ppx/grfx/vk/vk_render_pass.h"
#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_image.h"

namespace ppx {
namespace grfx {
namespace vk {

Result RenderPass::CreateRenderPass(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    bool   hasDepthSencil = mDepthStencilView.object ? true : false;
    size_t rtvCount       = CountU32(mRenderTargetViews);

    // Attachment descriptions
    std::vector<VkAttachmentDescription> attachmentDesc;
    {
        for (uint32_t i = 0; i < rtvCount; ++i) {
            grfx::RenderTargetViewPtr rtv = mRenderTargetViews[i].object;

            VkAttachmentDescription desc = {};
            desc.flags                   = 0;
            desc.format                  = ToVkFormat(rtv->GetFormat());
            desc.samples                 = ToVkSampleCount(rtv->GetSampleCount());
            desc.loadOp                  = ToVkAttachmentLoadOp(rtv->GetLoadOp());
            desc.storeOp                 = ToVkAttachmentStoreOp(rtv->GetStoreOp());
            desc.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.initialLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            desc.finalLayout             = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            attachmentDesc.push_back(desc);
        }

        if (hasDepthSencil) {
            grfx::DepthStencilViewPtr dsv = mDepthStencilView.object;

            VkAttachmentDescription desc = {};
            desc.flags                   = 0;
            desc.format                  = ToVkFormat(dsv->GetFormat());
            desc.samples                 = VK_SAMPLE_COUNT_1_BIT;
            desc.loadOp                  = ToVkAttachmentLoadOp(dsv->GetDepthLoadOp());
            desc.storeOp                 = ToVkAttachmentStoreOp(dsv->GetDepthStoreOp());
            desc.stencilLoadOp           = ToVkAttachmentLoadOp(dsv->GetStencilLoadOp());
            desc.stencilStoreOp          = ToVkAttachmentStoreOp(dsv->GetStencilStoreOp());
            desc.initialLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            desc.finalLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachmentDesc.push_back(desc);
        }
    }

    std::vector<VkAttachmentReference> colorRefs;
    {
        for (uint32_t i = 0; i < rtvCount; ++i) {
            VkAttachmentReference ref = {};
            ref.attachment            = i;
            ref.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorRefs.push_back(ref);
        }
    }

    VkAttachmentReference depthStencilRef = {};
    if (hasDepthSencil) {
        depthStencilRef.attachment = static_cast<uint32_t>(attachmentDesc.size() - 1);
        depthStencilRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpassDescription    = {};
    subpassDescription.flags                   = 0;
    subpassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount    = 0;
    subpassDescription.pInputAttachments       = nullptr;
    subpassDescription.colorAttachmentCount    = CountU32(colorRefs);
    subpassDescription.pColorAttachments       = DataPtr(colorRefs);
    subpassDescription.pResolveAttachments     = nullptr;
    subpassDescription.pDepthStencilAttachment = hasDepthSencil ? &depthStencilRef : nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments    = nullptr;

    VkSubpassDependency subpassDependencies = {};
    subpassDependencies.srcSubpass          = VK_SUBPASS_EXTERNAL;
    subpassDependencies.dstSubpass          = 0;
    subpassDependencies.srcStageMask        = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies.dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies.srcAccessMask       = 0;
    subpassDependencies.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependencies.dependencyFlags     = 0;

    VkRenderPassCreateInfo vkci = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    vkci.flags                  = 0;
    vkci.attachmentCount        = CountU32(attachmentDesc);
    vkci.pAttachments           = DataPtr(attachmentDesc);
    vkci.subpassCount           = 1;
    vkci.pSubpasses             = &subpassDescription;
    vkci.dependencyCount        = 1;
    vkci.pDependencies          = &subpassDependencies;

    VkResult vkres = vkCreateRenderPass(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mRenderPass);
    if (vkres != VK_SUCCESS) {
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

Result RenderPass::CreateFramebuffer(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    bool   hasDepthSencil = mDepthStencilView.object ? true : false;
    size_t rtvCount       = CountU32(mRenderTargetViews);

    std::vector<VkImageView> attachments;
    for (uint32_t i = 0; i < rtvCount; ++i) {
        grfx::RenderTargetViewPtr rtv = mRenderTargetViews[i].object;
        attachments.push_back(ToApi(rtv.Get())->GetVkImageView());
    }

    if (hasDepthSencil) {
        grfx::DepthStencilViewPtr dsv = mDepthStencilView.object;
        attachments.push_back(ToApi(dsv.Get())->GetVkImageView());
    }

    VkFramebufferCreateInfo vkci = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    vkci.flags                   = 0;
    vkci.renderPass              = mRenderPass;
    vkci.attachmentCount         = CountU32(attachments);
    vkci.pAttachments            = DataPtr(attachments);
    vkci.width                   = pCreateInfo->width;
    vkci.height                  = pCreateInfo->height;
    vkci.layers                  = 1;

    VkResult vkres = vkCreateFramebuffer(
        ToApi(GetDevice())->GetVkDevice(),
        &vkci,
        nullptr,
        &mFramebuffer);
    if (vkres != VK_SUCCESS) {
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

Result RenderPass::CreateApiObjects(const grfx::internal::RenderPassCreateInfo* pCreateInfo)
{
    Result ppxres = CreateRenderPass(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = CreateFramebuffer(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void RenderPass::DestroyApiObjects()
{
    if (mFramebuffer) {
        vkDestroyFramebuffer(
            ToApi(GetDevice())->GetVkDevice(),
            mFramebuffer,
            nullptr);
        mFramebuffer.Reset();
    }

    if (mRenderPass) {
        vkDestroyRenderPass(
            ToApi(GetDevice())->GetVkDevice(),
            mRenderPass,
            nullptr);
        mRenderPass.Reset();
    }
}

} // namespace vk
} // namespace grfx
} // namespace ppx
