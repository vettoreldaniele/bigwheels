#include "ppx/grfx/dx/dx_pipeline.h"
#include "ppx/grfx/dx/dx_descriptor.h"
#include "ppx/grfx/dx/dx_device.h"
#include "ppx/grfx/dx/dx_shader.h"
#include "ppx/grfx/dx/dx_util.h"

namespace ppx {
namespace grfx {
namespace dx {

// -------------------------------------------------------------------------------------------------
// ComputePipeline
// -------------------------------------------------------------------------------------------------
Result ComputePipeline::CreateApiObjects(const grfx::ComputePipelineCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void ComputePipeline::DestroyApiObjects()
{
}

// -------------------------------------------------------------------------------------------------
// GraphicsPipeline
// -------------------------------------------------------------------------------------------------
void GraphicsPipeline::InitializeShaderStages(
    const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
    D3D12_GRAPHICS_PIPELINE_STATE_DESC&     desc)
{
    // VS
    if (!IsNull(pCreateInfo->VS.pModule)) {
        desc.VS.pShaderBytecode = ToApi(pCreateInfo->VS.pModule)->GetCode();
        desc.VS.BytecodeLength  = ToApi(pCreateInfo->VS.pModule)->GetSize();
    }

    // HS
    if (!IsNull(pCreateInfo->HS.pModule)) {
        desc.HS.pShaderBytecode = ToApi(pCreateInfo->HS.pModule)->GetCode();
        desc.HS.BytecodeLength  = ToApi(pCreateInfo->HS.pModule)->GetSize();
    }

    // DS
    if (!IsNull(pCreateInfo->DS.pModule)) {
        desc.DS.pShaderBytecode = ToApi(pCreateInfo->DS.pModule)->GetCode();
        desc.DS.BytecodeLength  = ToApi(pCreateInfo->DS.pModule)->GetSize();
    }

    // GS
    if (!IsNull(pCreateInfo->GS.pModule)) {
        desc.GS.pShaderBytecode = ToApi(pCreateInfo->GS.pModule)->GetCode();
        desc.GS.BytecodeLength  = ToApi(pCreateInfo->GS.pModule)->GetSize();
    }

    // PS
    if (!IsNull(pCreateInfo->PS.pModule)) {
        desc.PS.pShaderBytecode = ToApi(pCreateInfo->PS.pModule)->GetCode();
        desc.PS.BytecodeLength  = ToApi(pCreateInfo->PS.pModule)->GetSize();
    }
}

void GraphicsPipeline::InitializeBlendState(
    const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
    D3D12_BLEND_DESC&                       desc)
{
    desc.AlphaToCoverageEnable  = static_cast<BOOL>(pCreateInfo->multisampleState.alphaToCoverageEnable);
    desc.IndependentBlendEnable = (pCreateInfo->colorBlendState.blendAttachmentCount > 0) ? TRUE : FALSE;

    PPX_ASSERT_MSG(pCreateInfo->colorBlendState.blendAttachmentCount < PPX_MAX_RENDER_TARGETS, "blendAttachmentCount exceeds PPX_MAX_RENDER_TARGETS");
    for (uint32_t i = 0; i < pCreateInfo->colorBlendState.blendAttachmentCount; ++i) {
        const grfx::BlendAttachmentState& ppxBlend = pCreateInfo->colorBlendState.blendAttachments[i];
        D3D12_RENDER_TARGET_BLEND_DESC&   d3dBlend = desc.RenderTarget[i];

        d3dBlend.BlendEnable           = static_cast<BOOL>(ppxBlend.blendEnable);
        d3dBlend.LogicOpEnable         = static_cast<BOOL>(pCreateInfo->colorBlendState.logicOpEnable);
        d3dBlend.SrcBlend              = ToD3D12Blend(ppxBlend.srcColorBlendFactor);
        d3dBlend.DestBlend             = ToD3D12Blend(ppxBlend.dstColorBlendFactor);
        d3dBlend.BlendOp               = ToD3D12BlendOp(ppxBlend.colorBlendOp);
        d3dBlend.SrcBlendAlpha         = ToD3D12Blend(ppxBlend.srcAlphaBlendFactor);
        d3dBlend.DestBlendAlpha        = ToD3D12Blend(ppxBlend.dstAlphaBlendFactor);
        d3dBlend.BlendOpAlpha          = ToD3D12BlendOp(ppxBlend.alphaBlendOp);
        d3dBlend.LogicOp               = ToD3D12LogicOp(pCreateInfo->colorBlendState.logicOp);
        d3dBlend.RenderTargetWriteMask = ToD3D12WriteMask(ppxBlend.colorWriteMask);
    }
}

void GraphicsPipeline::InitializeRasterizerState(
    const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
    D3D12_RASTERIZER_DESC&                  desc)
{
    desc.FillMode              = ToD3D12FillMode(pCreateInfo->rasterState.polygonMode);
    desc.CullMode              = ToD3D12CullMode(pCreateInfo->rasterState.cullMode);
    desc.FrontCounterClockwise = (pCreateInfo->rasterState.frontFace == grfx::FRONT_FACE_CCW) ? TRUE : FALSE;
    desc.DepthBias             = pCreateInfo->rasterState.depthBiasEnable ? static_cast<INT>(pCreateInfo->rasterState.depthBiasConstantFactor) : 0;
    desc.DepthBiasClamp        = pCreateInfo->rasterState.depthBiasEnable ? pCreateInfo->rasterState.depthBiasClamp : 0;
    desc.SlopeScaledDepthBias  = pCreateInfo->rasterState.depthBiasEnable ? pCreateInfo->rasterState.depthBiasSlopeFactor : 0;
    desc.DepthClipEnable       = static_cast<BOOL>(pCreateInfo->rasterState.depthClipEnable);
    desc.MultisampleEnable     = FALSE; // @TODO: Route this in
    desc.AntialiasedLineEnable = FALSE;
    desc.ForcedSampleCount     = 0;
    desc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void GraphicsPipeline::InitializeDepthStencilState(
    const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
    D3D12_DEPTH_STENCIL_DESC&               desc)
{
    desc.DepthEnable                  = static_cast<BOOL>(pCreateInfo->depthStencilState.depthTestEnable);
    desc.DepthWriteMask               = pCreateInfo->depthStencilState.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc                    = ToD3D12ComparisonFunc(pCreateInfo->depthStencilState.depthCompareOp);
    desc.StencilEnable                = static_cast<BOOL>(pCreateInfo->depthStencilState.stencilTestEnable);
    desc.StencilReadMask              = D3D12_DEFAULT_STENCIL_READ_MASK;  // @TODO: Figure out to set properly
    desc.StencilWriteMask             = D3D12_DEFAULT_STENCIL_WRITE_MASK; // @TODO: Figure out to set properly
    desc.FrontFace.StencilFailOp      = ToD3D12StencilOp(pCreateInfo->depthStencilState.front.failOp);
    desc.FrontFace.StencilDepthFailOp = ToD3D12StencilOp(pCreateInfo->depthStencilState.front.depthFailOp);
    desc.FrontFace.StencilPassOp      = ToD3D12StencilOp(pCreateInfo->depthStencilState.front.passOp);
    desc.FrontFace.StencilFunc        = ToD3D12ComparisonFunc(pCreateInfo->depthStencilState.front.compareOp);
    desc.BackFace.StencilFailOp       = ToD3D12StencilOp(pCreateInfo->depthStencilState.back.failOp);
    desc.BackFace.StencilDepthFailOp  = ToD3D12StencilOp(pCreateInfo->depthStencilState.back.depthFailOp);
    desc.BackFace.StencilPassOp       = ToD3D12StencilOp(pCreateInfo->depthStencilState.back.passOp);
    desc.BackFace.StencilFunc         = ToD3D12ComparisonFunc(pCreateInfo->depthStencilState.back.compareOp);
}

void GraphicsPipeline::InitializeInputLayout(
    const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
    std::vector<D3D12_INPUT_ELEMENT_DESC>&  inputElements,
    D3D12_INPUT_LAYOUT_DESC&                desc)
{
    for (uint32_t bindingIndex = 0; bindingIndex < pCreateInfo->vertexInputState.bindingCount; ++bindingIndex) {
        const grfx::VertexBinding& binding = pCreateInfo->vertexInputState.bindings[bindingIndex];
        // Iterate each attribute in the binding
        const uint32_t attributeCount = binding.GetAttributeCount();
        for (uint32_t attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex) {
            // This should be safe since there's no modifications to the index
            const grfx::VertexAttribute* pAttribute = nullptr;
            binding.GetAttribute(attributeIndex, &pAttribute);

            D3D12_INPUT_ELEMENT_DESC element = {};
            element.SemanticName             = pAttribute->semanticName.c_str();
            element.SemanticIndex            = 0;
            element.Format                   = ToDxgiFormat(pAttribute->format);
            element.InputSlot                = static_cast<UINT>(pAttribute->binding);
            element.AlignedByteOffset        = static_cast<UINT>(pAttribute->offset);
            element.InputSlotClass           = (pAttribute->inputRate == grfx::VERETX_INPUT_RATE_INSTANCE) ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            element.InstanceDataStepRate     = (pAttribute->inputRate == grfx::VERETX_INPUT_RATE_INSTANCE) ? 1 : 0;
            inputElements.push_back(element);
        }
    }

    //for (auto& binding : mInputBindings) {
    //    for (auto& attribute : binding.attributes) {
    //        D3D12_INPUT_ELEMENT_DESC element = {};
    //        element.SemanticName             = attribute.semanticName.c_str();
    //        element.SemanticIndex            = 0;
    //        element.Format                   = ToDxgiFormat(attribute.format);
    //        element.InputSlot                = static_cast<UINT>(attribute.binding);
    //        element.AlignedByteOffset        = static_cast<UINT>(attribute.offset);
    //        element.InputSlotClass           = (attribute.inputRate == grfx::VERETX_INPUT_RATE_INSTANCE) ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    //        element.InstanceDataStepRate     = (attribute.inputRate == grfx::VERETX_INPUT_RATE_INSTANCE) ? 1 : 0;
    //        inputElements.push_back(element);
    //    }
    //}

    desc.NumElements        = static_cast<UINT>(inputElements.size());
    desc.pInputElementDescs = inputElements.data();
}

void GraphicsPipeline::InitializeOutput(
    const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
    D3D12_GRAPHICS_PIPELINE_STATE_DESC&     desc)
{
    desc.NumRenderTargets = pCreateInfo->outputState.renderTargetCount;

    for (UINT i = 0; i < desc.NumRenderTargets; ++i) {
        desc.RTVFormats[i] = ToDxgiFormat(pCreateInfo->outputState.renderTargetFormats[i]);
    }

    desc.DSVFormat = ToDxgiFormat(pCreateInfo->outputState.depthStencilFormat);
}

Result GraphicsPipeline::CreateApiObjects(const grfx::GraphicsPipelineCreateInfo* pCreateInfo)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature                     = ToApi(pCreateInfo->pPipelineInterface)->GetDxRootSignature().Get();

    InitializeShaderStages(pCreateInfo, desc);

    desc.StreamOutput = {};

    InitializeBlendState(pCreateInfo, desc.BlendState);

    // @TODO: Figure out the right way to handle this
    desc.SampleMask = UINT_MAX;

    InitializeRasterizerState(pCreateInfo, desc.RasterizerState);

    InitializeDepthStencilState(pCreateInfo, desc.DepthStencilState);

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
    InitializeInputLayout(pCreateInfo, inputElements, desc.InputLayout);

    // @TODO: Figure out how to route this in so it plays nice with Vulkan
    desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    desc.PrimitiveTopologyType = ToD3D12PrimitiveTopology(pCreateInfo->inputAssemblyState.topology);

    InitializeOutput(pCreateInfo, desc);

    // @TODO: Add logic to handle quality
    desc.SampleDesc.Count   = static_cast<UINT>(pCreateInfo->rasterState.rasterizationSamples);
    desc.SampleDesc.Quality = 0;

    desc.NodeMask  = 0;
    desc.CachedPSO = {};
    desc.Flags     = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr = ToApi(GetDevice())->GetDxDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&mPipeline));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateGraphicsPipelineState failed");
        return ppx::ERROR_API_FAILURE;
    }

    // clang-format off
    switch (pCreateInfo->inputAssemblyState.topology) {
        default: {
            PPX_ASSERT_MSG(false, "unknown primitive teopolgy type");
            return ppx::ERROR_INVALID_CREATE_ARGUMENT;
        }
        break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST  : mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        case grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_FAN   : break;
        case grfx::PRIMITIVE_TOPOLOGY_POINT_LIST     : mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case grfx::PRIMITIVE_TOPOLOGY_LINE_LIST      : mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case grfx::PRIMITIVE_TOPOLOGY_LINE_STRIP     : mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case grfx::PRIMITIVE_TOPOLOGY_PATCH_LIST     : break;
    }
    // clang-format on

    return ppx::SUCCESS;
}

void GraphicsPipeline::DestroyApiObjects()
{
    if (mPipeline) {
        mPipeline.Reset();
    }
}

// -------------------------------------------------------------------------------------------------
// PipelineInterface
// -------------------------------------------------------------------------------------------------
Result PipelineInterface::CreateApiObjects(const grfx::PipelineInterfaceCreateInfo* pCreateInfo)
{
    dx::Device* pDevice = ToApi(GetDevice());

    std::vector<std::unique_ptr<std::vector<D3D12_DESCRIPTOR_RANGE1>>> parameterRanges;

    std::vector<D3D12_ROOT_PARAMETER1> parameters;
    for (uint32_t setIndex = 0; setIndex < pCreateInfo->setCount; ++setIndex) {
        uint32_t                                    set      = pCreateInfo->sets[setIndex].set;
        const dx::DescriptorSetLayout*              pLayout  = ToApi(pCreateInfo->sets[setIndex].pLayout);
        const std::vector<grfx::DescriptorBinding>& bindings = pLayout->GetBindings();

        // Allocate unique container for this table's ranges
        std::unique_ptr<std::vector<D3D12_DESCRIPTOR_RANGE1>> ranges = std::make_unique<std::vector<D3D12_DESCRIPTOR_RANGE1>>();
        if (!ranges) {
            PPX_ASSERT_MSG(false, "allocation for descriptor table ranges failed");
            return ppx::ERROR_ALLOCATION_FAILED;
        }
        // Fill out ranges
        for (size_t bindingIndex = 0; bindingIndex < bindings.size(); ++bindingIndex) {
            const grfx::DescriptorBinding& binding = bindings[bindingIndex];

            D3D12_DESCRIPTOR_RANGE1 range           = {};
            range.RangeType                         = ToD3D12RangeType(binding.type);
            range.NumDescriptors                    = static_cast<UINT>(binding.arrayCount);
            range.BaseShaderRegister                = static_cast<UINT>(binding.binding);
            range.RegisterSpace                     = static_cast<UINT>(set);
            range.Flags                             = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
            range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            ranges->push_back(range);
        }

        // Fill out parameter
        D3D12_ROOT_PARAMETER1 parameter               = {};
        parameter.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(ranges->size());
        parameter.DescriptorTable.pDescriptorRanges   = DataPtr(*ranges);
        parameter.ShaderVisibility                    = ToD3D12ShaderVisibliity(pLayout->GetShaderVisiblity());
        // Store parameter
        parameters.push_back(parameter);
        // Store ranges
        parameterRanges.push_back(std::move(ranges));
    }

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
    desc.Version                             = D3D_ROOT_SIGNATURE_VERSION_1_1;
    desc.Desc_1_1.NumParameters              = static_cast<UINT>(parameters.size());
    desc.Desc_1_1.pParameters                = DataPtr(parameters);
    desc.Desc_1_1.NumStaticSamplers          = 0;
    desc.Desc_1_1.pStaticSamplers            = nullptr;
    desc.Desc_1_1.Flags                      = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> sigBlob   = nullptr;
    ComPtr<ID3DBlob> errorMsgs = nullptr;
    HRESULT          hr        = pDevice->SerializeVersionedRootSignature(
        &desc,
        &sigBlob,
        &errorMsgs);
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "dx::Device::SerializeVersionedRootSignature failed");
        return ppx::ERROR_API_FAILURE;
    }

    UINT nodeMask = 0;
    hr            = pDevice->GetDxDevice()->CreateRootSignature(
        nodeMask,
        sigBlob->GetBufferPointer(),
        sigBlob->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature));
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "ID3D12Device::CreateRootSignature failed");
        return ppx::ERROR_API_FAILURE;
    }

    return ppx::SUCCESS;
}

void PipelineInterface::DestroyApiObjects()
{
    if (mRootSignature) {
        mRootSignature.Reset();
    }
}

} // namespace dx
} // namespace grfx
} // namespace ppx
