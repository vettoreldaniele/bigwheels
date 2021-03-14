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
    CMD_SET_VIEWPORTS,
    CMD_SET_SCISSORS,
    CMD_BIND_INDEX_BUFFER,
    CMD_BIND_VERTEX_BUFFERS,
    CMD_DRAW,
    CMD_DRAW_INDEXED,
    CMD_DISPATCH,
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

private:
    virtual void BeginRenderPassImpl(const grfx::RenderPassBeginInfo* pBeginInfo) override;
    virtual void EndRenderPassImpl() override;

public:
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
    template <typename DataT>
    class StateStackT
    {
    public:
        StateStackT()
        {
            mStack.reserve(32);
            Reset();
        }

        void Reset()
        {
            mDirty = false;
            mStack.resize(1);
            mCurrent = &mStack.back();
        }

        DataT* GetCurrent()
        {
            //
            // Assume any calls to this function will write data
            //
            mDirty = true;
            return mCurrent;
        }

        const DataT& At(uint32_t index) const
        {
            return mStack[index];
        }

        uint32_t Commit()
        {
            uint32_t committedIndex = CountU32(mStack) - 1;
            if (mDirty) {
                mStack.emplace_back(mStack.back());
                mCurrent = &mStack.back();
                mDirty   = false;
            }
            return committedIndex;
        }

    private:
        bool               mDirty   = false;
        std::vector<DataT> mStack   = {};
        DataT*             mCurrent = nullptr;
    };

    struct RTVClearValue
    {
        std::array<FLOAT, 4> rgba;
    };

    struct DSVClearValue
    {
        FLOAT depth;
        UINT  stencil;
    };

    struct IndexBufferState
    {
        ID3D11Buffer* buffer = nullptr;
        DXGI_FORMAT   format = DXGI_FORMAT_UNKNOWN;
        UINT          offset = 0;
    };

    struct VertexBufferState
    {
        UINT                                               startSlot  = 0;
        UINT                                               numBuffers = 0;
        std::array<ID3D11Buffer*, PPX_MAX_VERTEX_BINDINGS> buffers    = {nullptr};
        std::array<UINT, PPX_MAX_VERTEX_BINDINGS>          strides    = {0};
        std::array<UINT, PPX_MAX_VERTEX_BINDINGS>          offsets    = {0};
    };

    struct ViewportState
    {
        UINT                                          numViewports = 0;
        std::array<D3D11_VIEWPORT, PPX_MAX_VIEWPORTS> viewports    = {};
    };

    struct ScissorState
    {
        UINT                                      numRects = 0;
        std::array<D3D11_RECT, PPX_MAX_VIEWPORTS> rects    = {};
    };

    struct GraphicsPipelineState
    {
        ID3D11VertexShader*      VS                = nullptr;
        ID3D11HullShader*        HS                = nullptr;
        ID3D11DomainShader*      DS                = nullptr;
        ID3D11GeometryShader*    GS                = nullptr;
        ID3D11PixelShader*       PS                = nullptr;
        ID3D11InputLayout*       inputLayout       = nullptr;
        D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        ID3D11RasterizerState2*  rasterizerState   = nullptr;
    };

    // ---------------------------------------------------------------------------------------------
    // Args [BEGIN]
    // ---------------------------------------------------------------------------------------------
    struct RenderPassArgs
    {
        struct
        {
            uint32_t                                                    numViews    = 0;
            std::array<ID3D11RenderTargetView*, PPX_MAX_RENDER_TARGETS> views       = {nullptr};
            std::array<grfx::AttachmentLoadOp, PPX_MAX_RENDER_TARGETS>  loadOps     = {grfx::ATTACHMENT_LOAD_OP_LOAD};
            std::array<RTVClearValue, PPX_MAX_RENDER_TARGETS>           clearValues = {};
        } rtvs;

        struct
        {
            ID3D11DepthStencilView* pView  = nullptr;
            grfx::AttachmentLoadOp  loadOp = grfx::ATTACHMENT_LOAD_OP_LOAD;
            DSVClearValue           clearValue;
        } dsv;
    };

    struct DispatchArgs
    {
        UINT threadGroupCountX = 0;
        UINT threadGroupCountY = 0;
        UINT threadGroupCountZ = 0;
    };

    struct DrawArgs
    {
        UINT vertexCountPerInstance = 0;
        UINT instanceCount          = 0;
        UINT startVertexLocation    = 0;
        UINT startInstanceLocation  = 0;
    };

    struct ImGuiRenderArgs
    {
        void (*pRenderFn)(void) = nullptr;
    };
    // ---------------------------------------------------------------------------------------------
    // Args [END]
    // ---------------------------------------------------------------------------------------------

    struct CmdArgs
    {
        union
        {
            RenderPassArgs  renderPass;
            DispatchArgs    dispatch;
            DrawArgs        draw;
            ImGuiRenderArgs imGuiRender;
        };

        CmdArgs() {}

        ~CmdArgs() {}
    };

    struct ActionCmd
    {
        Cmd     cmd  = CMD_UNKNOWN;
        CmdArgs args = {};

        uint32_t viewportStateIndex         = kInvalidStateIndex;
        uint32_t scissorStateIndex          = kInvalidStateIndex;
        uint32_t indexBuffereStateIndex     = kInvalidStateIndex;
        uint32_t vertexBufferStateIndex     = kInvalidStateIndex;
        uint32_t graphicsPipleineStateIndex = kInvalidStateIndex;

        ActionCmd(Cmd cmd_)
            : cmd(cmd_) {}

        ~ActionCmd() {}
    };

    StateStackT<ViewportState>         mViewportState;
    StateStackT<ScissorState>          mScissorState;
    StateStackT<IndexBufferState>      mIndexBufferState;
    StateStackT<VertexBufferState>     mVertexBuffersState;
    StateStackT<GraphicsPipelineState> mGraphicsPipelineStack;
    std::vector<ActionCmd>             mActionCmds;
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
