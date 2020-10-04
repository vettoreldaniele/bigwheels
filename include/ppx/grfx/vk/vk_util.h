#ifndef ppx_grfx_vk_util_h
#define ppx_grfx_vk_util_h

#include "ppx/grfx/vk/000_vk_config_platform.h"
#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {
namespace vk {

const char* ToString(VkResult value);

VkAttachmentLoadOp         ToVkAttachmentLoadOp(grfx::AttachmentLoadOp value);
VkAttachmentStoreOp        ToVkAttachmentStoreOp(grfx::AttachmentStoreOp value);
VkBlendFactor              ToVkBlendFactor(grfx::BlendFactor value);
VkBlendOp                  ToVkBlendOp(grfx::BlendOp value);
VkBufferUsageFlags         ToVkBufferUsageFlags(const grfx::BufferUsageFlags& value);
VkClearColorValue          ToVkClearColorValue(const grfx::RenderTargetClearValue& value);
VkClearDepthStencilValue   ToVkClearDepthStencilValue(const grfx::DepthStencilClearValue& value);
VkColorComponentFlags      ToVkColorComponentFlags(const grfx::ColorComponentFlags& value);
VkCompareOp                ToVkCompareOp(grfx::CompareOp value);
VkComponentSwizzle         ToVkComponentSwizzle(grfx::ComponentSwizzle value);
VkComponentMapping         ToVkComponentMapping(const grfx::ComponentMapping& value);
VkCullModeFlagBits         ToVkCullMode(grfx::CullMode value);
VkFormat                   ToVkFormat(grfx::Format value);
VkFrontFace                ToVkFrontFace(grfx::FrontFace value);
VkImageType                ToVkImageType(grfx::ImageType value);
VkImageUsageFlags          ToVkImageUsageFlags(const grfx::ImageUsageFlags& value);
VkImageViewType            ToVkImageViewType(grfx::ImageViewType value);
VkIndexType                ToVkIndexType(grfx::IndexType value);
VkLogicOp                  ToVkLogicOp(grfx::LogicOp value);
VkPolygonMode              ToVkPolygonMode(grfx::PolygonMode value);
VkPresentModeKHR           ToVkPresentMode(grfx::PresentMode value);
VkPrimitiveTopology        ToVkPrimitiveTopology(grfx::PrimitiveTopology value);
VkStencilOp                ToVkStencilOp(grfx::StencilOp value);
VkSampleCountFlagBits      ToVkSampleCount(grfx::SampleCount value);
VkTessellationDomainOrigin ToVkTessellationDomainOrigin(grfx::TessellationDomainOrigin value);
VkVertexInputRate          ToVkVertexInputRate(grfx::VertexInputRate value);

Result ToVkBarrierSrc(ResourceState state, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout);
Result ToVkBarrierDst(ResourceState state, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout);

VkImageAspectFlags DetermineAspectMask(VkFormat format);

VmaMemoryUsage ToVmaMemoryUsage(grfx::MemoryUsage value);

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_util_h
