#include "ppx/grfx/grfx_pipeline.h"

namespace ppx {
namespace grfx {

// -------------------------------------------------------------------------------------------------
// BlendAttachmentState
// -------------------------------------------------------------------------------------------------
grfx::BlendAttachmentState BlendAttachmentState::BlendModeAdditive()
{
    grfx::BlendAttachmentState state = {};
    state.blendEnable                = true;
    state.srcColorBlendFactor        = grfx::BLEND_FACTOR_SRC_ALPHA;
    state.dstColorBlendFactor        = grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    state.colorBlendOp               = grfx::BLEND_OP_ADD;
    state.srcAlphaBlendFactor        = grfx::BLEND_FACTOR_SRC_ALPHA;
    state.dstAlphaBlendFactor        = grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    state.alphaBlendOp               = grfx::BLEND_OP_ADD;
    state.colorWriteMask             = grfx::ColorComponentFlags::RGBA();

    return state;
}

grfx::BlendAttachmentState BlendAttachmentState::BlendModeAlpha()
{
    grfx::BlendAttachmentState state = {};
    state.blendEnable                = true;
    state.srcColorBlendFactor        = grfx::BLEND_FACTOR_SRC_ALPHA;
    state.dstColorBlendFactor        = grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    state.colorBlendOp               = grfx::BLEND_OP_ADD;
    state.srcAlphaBlendFactor        = grfx::BLEND_FACTOR_ONE;
    state.dstAlphaBlendFactor        = grfx::BLEND_FACTOR_ZERO;
    state.alphaBlendOp               = grfx::BLEND_OP_ADD;
    state.colorWriteMask             = grfx::ColorComponentFlags::RGBA();

    return state;
}

grfx::BlendAttachmentState BlendAttachmentState::BlendModeOver()
{
    grfx::BlendAttachmentState state = {};
    state.blendEnable                = true;
    state.srcColorBlendFactor        = grfx::BLEND_FACTOR_SRC_ALPHA;
    state.dstColorBlendFactor        = grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    state.colorBlendOp               = grfx::BLEND_OP_ADD;
    state.srcAlphaBlendFactor        = grfx::BLEND_FACTOR_SRC_ALPHA;
    state.dstAlphaBlendFactor        = grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    state.alphaBlendOp               = grfx::BLEND_OP_ADD;
    state.colorWriteMask             = grfx::ColorComponentFlags::RGBA();

    return state;
}

grfx::BlendAttachmentState BlendAttachmentState::BlendModeUnder()
{
    grfx::BlendAttachmentState state = {};
    state.blendEnable                = true;
    state.srcColorBlendFactor        = grfx::BLEND_FACTOR_DST_ALPHA;
    state.dstColorBlendFactor        = grfx::BLEND_FACTOR_ONE;
    state.colorBlendOp               = grfx::BLEND_OP_ADD;
    state.srcAlphaBlendFactor        = grfx::BLEND_FACTOR_ZERO;
    state.dstAlphaBlendFactor        = grfx::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    state.alphaBlendOp               = grfx::BLEND_OP_ADD;
    state.colorWriteMask             = grfx::ColorComponentFlags::RGBA();

    return state;
}

namespace internal {

// -------------------------------------------------------------------------------------------------
// internal
// -------------------------------------------------------------------------------------------------
void FillOutGraphicsPipelineCreateInfo(
    const grfx::GraphicsPipelineCreateInfo2* pSrcCreateInfo,
    grfx::GraphicsPipelineCreateInfo*        pDstCreateInfo)
{
    // Set to default values
    *pDstCreateInfo = {};

    // Shaders
    pDstCreateInfo->VS = pSrcCreateInfo->VS;
    pDstCreateInfo->PS = pSrcCreateInfo->PS;

    // Vertex input
    {
        pDstCreateInfo->vertexInputState.attributeCount = pSrcCreateInfo->vertexInputState.attributeCount;
        for (uint32_t i = 0; i < pDstCreateInfo->vertexInputState.attributeCount; ++i) {
            pDstCreateInfo->vertexInputState.attributes[i] = pSrcCreateInfo->vertexInputState.attributes[i];
        }
    }

    // Input aasembly
    {
        pDstCreateInfo->inputAssemblyState.topology = pSrcCreateInfo->topology;
    }

    // Raster
    {
        pDstCreateInfo->rasterState.polygonMode = pSrcCreateInfo->polygonMode;
        pDstCreateInfo->rasterState.cullMode    = pSrcCreateInfo->cullMode;
        pDstCreateInfo->rasterState.frontFace   = pSrcCreateInfo->frontFace;
    }

    // Depth/stencil
    {
        pDstCreateInfo->depthStencilState.depthTestEnable       = true;
        pDstCreateInfo->depthStencilState.depthWriteEnable      = true;
        pDstCreateInfo->depthStencilState.depthCompareOp        = grfx::COMPARE_OP_LESS;
        pDstCreateInfo->depthStencilState.depthBoundsTestEnable = false;
        pDstCreateInfo->depthStencilState.minDepthBounds        = 0.0f;
        pDstCreateInfo->depthStencilState.maxDepthBounds        = 1.0f;
        pDstCreateInfo->depthStencilState.stencilTestEnable     = false;
        pDstCreateInfo->depthStencilState.front                 = {};
        pDstCreateInfo->depthStencilState.back                  = {};
    }

    // Color blend
    {
        pDstCreateInfo->colorBlendState.blendAttachmentCount = pSrcCreateInfo->outputState.renderTargetCount;
        for (uint32_t i = 0; i < pDstCreateInfo->colorBlendState.blendAttachmentCount; ++i) {
            switch (pSrcCreateInfo->blendModes[i]) {
                default: break;

                case grfx::BLEND_MODE_ADDITIVE: {
                    pDstCreateInfo->colorBlendState.blendAttachments[i] = grfx::BlendAttachmentState::BlendModeAdditive();
                } break;

                case grfx::BLEND_MODE_ALPHA: {
                    pDstCreateInfo->colorBlendState.blendAttachments[i] = grfx::BlendAttachmentState::BlendModeAlpha();
                } break;

                case grfx::BLEND_MODE_OVER: {
                    pDstCreateInfo->colorBlendState.blendAttachments[i] = grfx::BlendAttachmentState::BlendModeOver();
                } break;

                case grfx::BLEND_MODE_UNDER: {
                    pDstCreateInfo->colorBlendState.blendAttachments[i] = grfx::BlendAttachmentState::BlendModeUnder();
                } break;
            }
            pDstCreateInfo->colorBlendState.blendAttachments[i].colorWriteMask = grfx::ColorComponentFlags::RGBA();
        }
    }

    // Output
    {
        pDstCreateInfo->outputState.renderTargetCount = pSrcCreateInfo->outputState.renderTargetCount;
        for (uint32_t i = 0; i < pDstCreateInfo->outputState.renderTargetCount; ++i) {
            pDstCreateInfo->outputState.renderTargetFormats[i] = pSrcCreateInfo->outputState.renderTargetFormats[i];
        }

        pDstCreateInfo->outputState.depthStencilFormat = pSrcCreateInfo->outputState.depthStencilFormat;
    }

    // Pipeline internface
    pDstCreateInfo->pPipelineInterface = pSrcCreateInfo->pPipelineInterface;
}

} // namespace internal

// -------------------------------------------------------------------------------------------------
// GraphicsPipeline::VertexInputBinding
// -------------------------------------------------------------------------------------------------
void GraphicsPipeline::VertexInputBinding::CalculateOffsetsAndStride()
{
    for (size_t i = 0; i < attributes.size(); ++i) {
        if (attributes[i].offset == PPX_APPEND_OFFSET_ALIGNED) {
            if (i > 0) {
                uint32_t formatSize  = grfx::FormatSize(attributes[i - 1].format);
                attributes[i].offset = attributes[i - 1].offset + formatSize;
            }
            else {
                attributes[i].offset = 0;
            }
        }

        uint32_t formatSize = grfx::FormatSize(attributes[i].format);
        uint32_t ub         = attributes[i].offset + formatSize;
        if (ub > stride) {
            stride = ub;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// GraphicsPipeline
// -------------------------------------------------------------------------------------------------
Result GraphicsPipeline::Create(const grfx::GraphicsPipelineCreateInfo* pCreateInfo)
{
    // Checked binding range
    for (uint32_t i = 0; i < pCreateInfo->vertexInputState.attributeCount; ++i) {
        const grfx::VertexAttribute& attribute = pCreateInfo->vertexInputState.attributes[i];
        if (attribute.binding >= PPX_MAX_VERTEX_ATTRIBUTES) {
            PPX_ASSERT_MSG(false, "binding is exceeds PPX_MAX_VERTEX_ATTRIBUTES");
            return ppx::ERROR_GRFX_MAX_VERTEX_BINDING_EXCEEDED;
        }
        if (attribute.format == grfx::FORMAT_UNDEFINED) {
            PPX_ASSERT_MSG(false, "vertex attribute format is undefined");
            return ppx::ERROR_GRFX_VERTEX_ATTRIBUTE_FROMAT_UNDEFINED;
        }
    }

    // Build input bindings
    {
        // Collect attributes into bindings
        for (uint32_t i = 0; i < pCreateInfo->vertexInputState.attributeCount; ++i) {
            const grfx::VertexAttribute& attribute = pCreateInfo->vertexInputState.attributes[i];

            auto it = std::find_if(
                std::begin(mInputBindings),
                std::end(mInputBindings),
                [attribute](const VertexInputBinding& elem) -> bool {
                bool isSame = attribute.binding == elem.binding;
                return isSame; });
            if (it != std::end(mInputBindings)) {
                it->attributes.push_back(attribute);
            }
            else {
                VertexInputBinding set = {attribute.binding};
                mInputBindings.push_back(set);
                mInputBindings.back().attributes.push_back(attribute);
            }
        }

        // Calculate offsets and stride
        for (auto& elem : mInputBindings) {
            elem.CalculateOffsetsAndStride();
        }

        // Check classifactions
        for (auto& elem : mInputBindings) {
            uint32_t inputRateVertexCount   = 0;
            uint32_t inputRateInstanceCount = 0;
            for (auto& attr : elem.attributes) {
                inputRateVertexCount += (attr.inputRate == grfx::VERTEX_INPUT_RATE_VERTEX) ? 1 : 0;
                inputRateInstanceCount += (attr.inputRate == grfx::VERETX_INPUT_RATE_INSTANCE) ? 1 : 0;
            }
            // Cannot mix input rates
            if ((inputRateInstanceCount > 0) && (inputRateVertexCount > 0)) {
                PPX_ASSERT_MSG(false, "cannot mix vertex input rates in same binding");
                return ppx::ERROR_GRFX_CANNOT_MIX_VERTEX_INPUT_RATES;
            }
        }
    }

    Result ppxres = grfx::DeviceObject<grfx::GraphicsPipelineCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
