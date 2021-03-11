#ifndef ppx_grfx_dx11_command_h
#define ppx_grfx_dx11_command_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_command.h"

namespace ppx {
namespace grfx {
namespace dx11 {

enum Cmd
{
    CMD_UNKNOWN = 0,
    CMD_BEGIN_RENDER_PASS,
    CMD_IM_GUI_RENDER,
};

class CommandBuffer
    : public grfx::CommandBuffer
{
public:
    CommandBuffer() {}
    virtual ~CommandBuffer() {}

    virtual Result Begin() override;
    virtual Result End() override;

    virtual void BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo) override;
    virtual void EndRenderPass() override;

    virtual void TransitionImageLayout(
        const grfx::Image*  pImage,
        uint32_t            mipLevel,
        uint32_t            mipLevelCount,
        uint32_t            arrayLayer,
        uint32_t            arrayLayerCount,
        grfx::ResourceState beforeState,
        grfx::ResourceState afterState,
        const grfx::Queue*  pSrcQueue,
        const grfx::Queue*  pDstQueue) override;

    virtual void BufferResourceBarrier(
        const grfx::Buffer* pBuffer,
        grfx::ResourceState beforeState,
        grfx::ResourceState afterState,
        const grfx::Queue*  pSrcQueue = nullptr,
        const grfx::Queue*  pDstQueue = nullptr) override;

    virtual void SetViewports(
        uint32_t              viewportCount,
        const grfx::Viewport* pViewports) override;

    virtual void SetScissors(
        uint32_t          scissorCount,
        const grfx::Rect* pScissors) override;

    virtual void BindGraphicsDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets) override;

    virtual void BindGraphicsPipeline(const grfx::GraphicsPipeline* pPipeline) override;

    virtual void BindComputeDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets) override;

    virtual void BindComputePipeline(const grfx::ComputePipeline* pPipeline) override;

    virtual void BindIndexBuffer(const grfx::IndexBufferView* pView) override;

    virtual void BindVertexBuffers(
        uint32_t                      viewCount,
        const grfx::VertexBufferView* pViews) override;

    virtual void Draw(
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance) override;

    virtual void DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t  vertexOffset,
        uint32_t firstInstance) override;

    virtual void Dispatch(
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ) override;

    virtual void CopyBufferToBuffer(
        const grfx::BufferToBufferCopyInfo* pCopyInfo,
        const grfx::Buffer*                 pSrcBuffer,
        const grfx::Buffer*                 pDstBuffer) override;

    virtual void CopyBufferToImage(
        const grfx::BufferToImageCopyInfo* pCopyInfo,
        const grfx::Buffer*                pSrcBuffer,
        const grfx::Image*                 pDstImage) override;

    virtual void CopyImageToBuffer(
        const grfx::ImageToBufferCopyInfo* pCopyInfo,
        const grfx::Image*                 pSrcImage,
        const grfx::Buffer*                pDstBuffer) override;

    virtual void BeginQuery(
        const grfx::QueryPool* pQueryPool,
        uint32_t               queryIndex) override;

    virtual void EndQuery(
        const grfx::QueryPool* pQueryPool,
        uint32_t               queryIndex) override;

    virtual void WriteTimestamp(
        grfx::PipelineStage    pipelineStage,
        const grfx::QueryPool* pQueryPool,
        uint32_t               queryIndex) override;

    void ImGuiRender(void (*pFn)(void));

protected:
    friend class dx11::Queue;

    virtual Result CreateApiObjects(const grfx::internal::CommandBufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    void BindDescriptorSets(
        const grfx::PipelineInterface*    pInterface,
        uint32_t                          setCount,
        const grfx::DescriptorSet* const* ppSets,
        size_t&                           rdtCountCBVSRVUAV,
        size_t&                           rdtCountSampler);

    struct BeginRenderPassArgs
    {
        UINT                    NumViews                                    = 0;
        ID3D11RenderTargetView* ppRenderTargetViews[PPX_MAX_RENDER_TARGETS] = {nullptr};
        ID3D11DepthStencilView* pDepthStencilView                           = nullptr;

        struct
        {
            ID3D11RenderTargetView* pRenderTargetView = nullptr;
            FLOAT                   ColorRGBA[4]      = {0};
        } RTVClearOp[PPX_MAX_RENDER_TARGETS];
    };

    struct ImGuiRenderArgs
    {
        void (*pRenderFn)(void) = nullptr;
    };

    union CmdArgs
    {
        BeginRenderPassArgs beginRenderPass;
        ImGuiRenderArgs     imGuiRender;
    };

    struct PipelineState
    {
        uint32_t                currentRenderTargetCount                         = 0;
        ID3D11RenderTargetView* currentRenderTargetViews[PPX_MAX_RENDER_TARGETS] = {nullptr};
        ID3D11DepthStencilView* currentDepthStencilViews                         = nullptr;
    };

    struct ActionCmd
    {
        Cmd     cmd  = CMD_UNKNOWN;
        CmdArgs args = {};

        ActionCmd(Cmd cmd_)
            : cmd(cmd_) {}
    };

    PipelineState          mPipelineState = {};
    std::vector<ActionCmd> mActionCmds;
};

// -------------------------------------------------------------------------------------------------

class CommandPool
    : public grfx::CommandPool
{
public:
    CommandPool() {}
    virtual ~CommandPool() {}

protected:
    virtual Result CreateApiObjects(const grfx::CommandPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_command_h
