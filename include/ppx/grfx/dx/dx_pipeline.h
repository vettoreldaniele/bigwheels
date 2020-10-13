#ifndef ppx_grfx_dx_pipeline_h
#define ppx_grfx_dx_pipeline_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_pipeline.h"

namespace ppx {
namespace grfx {
namespace dx {

class ComputePipeline
    : public grfx::ComputePipeline
{
public:
    ComputePipeline() {}
    virtual ~ComputePipeline() {}

protected:
    virtual Result CreateApiObjects(const grfx::ComputePipelineCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
};

// -------------------------------------------------------------------------------------------------

class GraphicsPipeline
    : public grfx::GraphicsPipeline
{
public:
    GraphicsPipeline() {}
    virtual ~GraphicsPipeline() {}

    D3D12PipelineStatePtr  GetDxPipeline() const { return mPipeline; }
    D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return mPrimitiveTopology; }

protected:
    virtual Result CreateApiObjects(const grfx::GraphicsPipelineCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    void InitializeShaderStages(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        D3D12_GRAPHICS_PIPELINE_STATE_DESC&     desc);

    void InitializeBlendState(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        D3D12_BLEND_DESC&                       desc);

    void InitializeRasterizerState(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        D3D12_RASTERIZER_DESC&                  desc);

    void InitializeDepthStencilState(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        D3D12_DEPTH_STENCIL_DESC&               desc);

    void InitializeInputLayout(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        std::vector<D3D12_INPUT_ELEMENT_DESC>&  inputElements,
        D3D12_INPUT_LAYOUT_DESC&                desc);

    void InitializeOutput(
        const grfx::GraphicsPipelineCreateInfo* pCreateInfo,
        D3D12_GRAPHICS_PIPELINE_STATE_DESC&     desc);

private:
    D3D12PipelineStatePtr  mPipeline;
    D3D_PRIMITIVE_TOPOLOGY mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

// -------------------------------------------------------------------------------------------------

class PipelineInterface
    : public grfx::PipelineInterface
{
public:
    PipelineInterface() {}
    virtual ~PipelineInterface() {}

    D3D12RootSignaturePtr GetDxRootSignature() const { return mRootSignature; }

protected:
    virtual Result CreateApiObjects(const grfx::PipelineInterfaceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12RootSignaturePtr mRootSignature;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_pipeline_h
