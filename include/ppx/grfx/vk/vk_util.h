#ifndef ppx_grfx_vk_util_h
#define ppx_grfx_vk_util_h

#include "ppx/grfx/vk/000_vk_config_platform.h"
#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {
namespace vk {

const char* ToString(VkResult value);

VkAttachmentLoadOp       ToVkAttachmentLoadOp(grfx::AttachmentLoadOp value);
VkAttachmentStoreOp      ToVkAttachmentStoreOp(grfx::AttachmentStoreOp value);
VkClearColorValue        ToVkClearColorValue(const grfx::RenderTargetClearValue& value);
VkClearDepthStencilValue ToVkClearDepthStencilValue(const grfx::DepthStencilClearValue& value);
VkComponentSwizzle       ToVkComponentSwizzle(grfx::ComponentSwizzle value);
VkComponentMapping       ToVkComponentMapping(const grfx::ComponentMapping& value);
VkFormat                 ToVkFormat(grfx::Format value);
VkImageType              ToVkImageType(grfx::ImageType value);
VkImageViewType          ToVkImageViewType(grfx::ImageViewType value);
VkPresentModeKHR         ToVkPresentMode(grfx::PresentMode value);
VkSampleCountFlagBits    ToVkSampleCount(grfx::SampleCount value);
Result                   ToVkBarrierSrc(ResourceState state, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout);
Result                   ToVkBarrierDst(ResourceState state, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout);

VkImageAspectFlags DetermineAspectMask(VkFormat format);

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_util_h
