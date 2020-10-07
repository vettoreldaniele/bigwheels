#ifndef ppx_grfx_pipeline_h
#define ppx_grfx_pipeline_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

struct ShaderStageInfo
{
    const grfx::ShaderModule* pModule    = nullptr;
    std::string               entryPoint = "";
};

// -------------------------------------------------------------------------------------------------

//! @struct ComputePipelineCreateInfo
//!
//!
struct ComputePipelineCreateInfo
{
    grfx::ShaderStageInfo          CS                 = {};
    const grfx::PipelineInterface* pPipelineInterface = nullptr;
};

//! @class ComputePipeline
//!
//!
class ComputePipeline
    : public grfx::DeviceObject<grfx::ComputePipelineCreateInfo>
{
public:
    ComputePipeline() {}
    virtual ~ComputePipeline() {}
};

// -------------------------------------------------------------------------------------------------

struct VertexInputState
{
    uint32_t              attributeCount                     = 0;
    grfx::VertexAttribute attributes[PPX_MAX_RENDER_TARGETS] = {{0, grfx::FORMAT_UNDEFINED, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX}};
};

struct InputAssemblyState
{
    grfx::PrimitiveTopology topology               = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    bool                    primitiveRestartEnable = false;
};

struct TessellationState
{
    uint32_t                       patchControlPoints = 0;
    grfx::TessellationDomainOrigin domainOrigin       = grfx::TESSELLATION_DOMAIN_ORIGIN_UPPER_LEFT;
};

struct RasterState
{
    bool              depthClampEnable        = false;
    bool              rasterizeDiscardEnable  = false;
    grfx::PolygonMode polygonMode             = grfx::POLYGON_MODE_FILL;
    grfx::CullMode    cullMode                = grfx::CULL_MODE_NONE;
    grfx::FrontFace   frontFace               = grfx::FRONT_FACE_CCW;
    bool              depthBiasEnable         = false;
    float             depthBiasConstantFactor = 0.0f;
    float             depthBiasClamp          = 0.0f;
    float             depthBiasSlopeFactor    = 0.0f;
    grfx::SampleCount rasterizationSamples    = grfx::SAMPLE_COUNT_1;
};

struct StencilOpState
{
    grfx::StencilOp failOp      = grfx::STENCIL_OP_KEEP;
    grfx::StencilOp passOp      = grfx::STENCIL_OP_KEEP;
    grfx::StencilOp depthFailOp = grfx::STENCIL_OP_KEEP;
    grfx::CompareOp compareOp   = grfx::COMPARE_OP_NEVER;
    uint32_t        compareMask = 0;
    uint32_t        writeMask   = 0;
    uint32_t        reference   = 0;
};

struct DepthStencilState
{
    bool                 depthTestEnable       = true;
    bool                 depthWriteEnable      = true;
    grfx::CompareOp      depthCompareOp        = grfx::COMPARE_OP_LESS;
    bool                 depthBoundsTestEnable = false;
    float                minDepthBounds        = 0.0f;
    float                maxDepthBounds        = 1.0f;
    bool                 stencilTestEnable     = false;
    grfx::StencilOpState front                 = {};
    grfx::StencilOpState back                  = {};
};

struct BlendAttachmentState
{
    bool                      blendEnable         = false;
    grfx::BlendFactor         srcColorBlendFactor = grfx::BLEND_FACTOR_ONE;
    grfx::BlendFactor         dstColorBlendFactor = grfx::BLEND_FACTOR_ZERO;
    grfx::BlendOp             colorBlendOp        = grfx::BLEND_OP_ADD;
    grfx::BlendFactor         srcAlphaBlendFactor = grfx::BLEND_FACTOR_ONE;
    grfx::BlendFactor         dstAlphaBlendFactor = grfx::BLEND_FACTOR_ZERO;
    grfx::BlendOp             alphaBlendOp        = grfx::BLEND_OP_ADD;
    grfx::ColorComponentFlags colorWriteMask      = grfx::ColorComponentFlags::RGBA();

    // These are best guesses based on random formulas off of the internet.
    // Correct later when authorative literature is found.
    //
    static grfx::BlendAttachmentState BlendModeAdditive();
    static grfx::BlendAttachmentState BlendModeAlpha();
    static grfx::BlendAttachmentState BlendModeOver();
    static grfx::BlendAttachmentState BlendModeUnder();
};

struct ColorBlendState
{
    bool                       logicOpEnable                            = false;
    grfx::LogicOp              logicOp                                  = grfx::LOGIC_OP_CLEAR;
    uint32_t                   blendAttachmentCount                     = 0;
    grfx::BlendAttachmentState blendAttachments[PPX_MAX_RENDER_TARGETS] = {};
    float                      blendConstants[4]                        = {1.0f, 1.0f, 1.0f, 1.0f};
};

struct OutputState
{
    uint32_t     renderTargetCount                           = 0;
    grfx::Format renderTargetFormats[PPX_MAX_RENDER_TARGETS] = {grfx::FORMAT_UNDEFINED};
    grfx::Format depthStencilFormat                          = grfx::FORMAT_UNDEFINED;
};

//! @struct GraphicsPipelineCreateInfo
//!
//!
struct GraphicsPipelineCreateInfo
{
    grfx::ShaderStageInfo          VS                 = {};
    grfx::ShaderStageInfo          HS                 = {};
    grfx::ShaderStageInfo          DS                 = {};
    grfx::ShaderStageInfo          GS                 = {};
    grfx::ShaderStageInfo          PS                 = {};
    grfx::VertexInputState         vertexInputState   = {};
    grfx::InputAssemblyState       inputAssemblyState = {};
    grfx::TessellationState        tessellationState  = {};
    grfx::RasterState              rasterState        = {};
    grfx::DepthStencilState        depthStencilState  = {};
    grfx::ColorBlendState          colorBlendState    = {};
    grfx::OutputState              outputState        = {};
    const grfx::PipelineInterface* pPipelineInterface = nullptr;
};

struct GraphicsPipelineCreateInfo2
{
    grfx::ShaderStageInfo          VS                                 = {};
    grfx::ShaderStageInfo          PS                                 = {};
    grfx::VertexInputState         vertexInputState                   = {};
    grfx::PrimitiveTopology        topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    grfx::PolygonMode              polygonMode                        = grfx::POLYGON_MODE_FILL;
    grfx::CullMode                 cullMode                           = grfx::CULL_MODE_NONE;
    grfx::FrontFace                frontFace                          = grfx::FRONT_FACE_CCW;
    grfx::BlendMode                blendModes[PPX_MAX_RENDER_TARGETS] = {grfx::BLEND_MODE_NONE};
    grfx::OutputState              outputState                        = {};
    const grfx::PipelineInterface* pPipelineInterface                 = nullptr;
};

namespace internal {

void FillOutGraphicsPipelineCreateInfo(
    const grfx::GraphicsPipelineCreateInfo2* pSrcCreateInfo,
    grfx::GraphicsPipelineCreateInfo*        pDstCreateInfo);

} // namespace internal

//! @class GraphicsPipeline
//!
//!
class GraphicsPipeline
    : public grfx::DeviceObject<grfx::GraphicsPipelineCreateInfo>
{
public:
    GraphicsPipeline() {}
    virtual ~GraphicsPipeline() {}

protected:
    virtual Result Create(const grfx::GraphicsPipelineCreateInfo* pCreateInfo) override;
    friend class grfx::Device;

protected:
    struct VertexInputBinding
    {
        uint32_t                           binding    = PPX_MAX_VERTEX_ATTRIBUTES;
        std::vector<grfx::VertexAttribute> attributes = {};
        uint32_t                           stride     = 0;
        grfx::VertexInputRate              inputRate  = grfx::VERTEX_INPUT_RATE_VERTEX;

        void CalculateOffsetsAndStride();
    };

    std::vector<VertexInputBinding> mInputBindings;
};

// -------------------------------------------------------------------------------------------------

//! @struct PipelineInterfaceCreateInfo
//!
//!
struct PipelineInterfaceCreateInfo
{
    uint32_t                         setLayoutCount                             = 0;
    const grfx::DescriptorSetLayout* pSetLayouts[PPX_MAX_BOUND_DESCRIPTOR_SETS] = {nullptr};
};

//! @class PipelineInterface
//!
//! VK: Pipeline layout
//! DX: Root signature
//!
class PipelineInterface
    : public grfx::DeviceObject<grfx::PipelineInterfaceCreateInfo>
{
public:
    PipelineInterface() {}
    virtual ~PipelineInterface() {}
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_pipeline_h
