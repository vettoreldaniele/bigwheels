#ifndef ppx_grfx_vk_pipeline_h
#define ppx_grfx_vk_pipeline_h

#include "ppx/grfx/vk/vk_config.h"
#include "ppx/grfx/grfx_pipeline.h"

namespace ppx {
namespace grfx {
namespace vk {

//! @class ComputePipeline
//!
//!
class ComputePipeline
    : public grfx::ComputePipeline
{
public:
    ComputePipeline() {}
    virtual ~ComputePipeline() {}

    VkPipelinePtr GetVkPipeline() const { return mPipeline; }

protected:
    virtual Result CreateApiObjects(const grfx::ComputePipelineCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkPipelinePtr mPipeline;
};

// -------------------------------------------------------------------------------------------------

//! @class GraphicsPipeline
//!
//!
class GraphicsPipeline
    : public grfx::GraphicsPipeline
{
public:
    GraphicsPipeline() {}
    virtual ~GraphicsPipeline() {}

    VkPipelinePtr GetVkPipeline() const { return mPipeline; }

protected:
    virtual Result CreateApiObjects(const grfx::GraphicsPipelineCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result InitializeShaderStages(
        const grfx::GraphicsPipelineCreateInfo*       pCreateInfo,
        std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
        VkGraphicsPipelineCreateInfo&                 vkCreateInfo);
    Result InitializeVertexInput(
        const grfx::GraphicsPipelineCreateInfo*         pCreateInfo,
        std::vector<VkVertexInputAttributeDescription>& attribues,
        std::vector<VkVertexInputBindingDescription>&   bindings,
        VkPipelineVertexInputStateCreateInfo&           stateCreateInfo);
    Result InitializeInputAssembly(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        VkPipelineInputAssemblyStateCreateInfo& stateCreateInfo);
    Result InitializeTessellation(
        const grfx::GraphicsPipelineCreateInfo*               pCreateInfo,
        VkPipelineTessellationDomainOriginStateCreateInfoKHR& domainOriginStateCreateInfo,
        VkPipelineTessellationStateCreateInfo&                stateCreateInfo);
    Result InitializeViewports(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        VkPipelineViewportStateCreateInfo&      stateCreateInfo);
    Result InitializeRasterization(
        const grfx::GraphicsPipelineCreateInfo*             pCreateInfo,
        VkPipelineRasterizationDepthClipStateCreateInfoEXT& depthClipStateCreateInfo,
        VkPipelineRasterizationStateCreateInfo&             stateCreateInfo);
    Result InitializeMultisample(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        VkPipelineMultisampleStateCreateInfo&   stateCreateInfo);
    Result InitializeDepthStencil(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        VkPipelineDepthStencilStateCreateInfo&  stateCreateInfo);
    Result InitializeColorBlend(
        const grfx::GraphicsPipelineCreateInfo*           pCreateInfo,
        std::vector<VkPipelineColorBlendAttachmentState>& attachments,
        VkPipelineColorBlendStateCreateInfo&              stateCreateInfo);
    Result InitializeDynamicState(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        std::vector<VkDynamicState>&            dynamicStates,
        VkPipelineDynamicStateCreateInfo&       stateCreateInfo);

private:
    VkPipelinePtr mPipeline;
};

// -------------------------------------------------------------------------------------------------

//! @class PipelineInterface
//!
//!
class PipelineInterface
    : public grfx::PipelineInterface
{
public:
    PipelineInterface() {}
    virtual ~PipelineInterface() {}

    VkPipelineLayoutPtr GetVkPipelineLayout() const { return mPipelineLayout; }

    uint32_t GetPushDescriptorsSetIndex() const { return mPushDescriptorsSetIndex; }

protected:
    virtual Result CreateApiObjects(const grfx::PipelineInterfaceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkPipelineLayoutPtr      mPipelineLayout;
    VkDescriptorSetLayoutPtr mPushDescriptorsSetLayout;
    uint32_t                 mPushDescriptorsSetIndex = PPX_VALUE_IGNORED;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_pipeline_h
