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
    CMD_TRANSITION_IMAGE_LAYOUT,
    CMD_SET_VIEWPORTS,
    CMD_SET_SCISSORS,
    CMD_BIND_INDEX_BUFFER,
    CMD_BIND_VERTEX_BUFFERS,
    CMD_DRAW,
    CMD_DRAW_INDEXED,
    CMD_DISPATCH,
    CMD_COPY_BUFFER_TO_BUFFER,
    CMD_COPY_BUFFER_TO_IMAGE,
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
        grfx::Buffer*                       pSrcBuffer,
        grfx::Buffer*                       pDstBuffer) override;

    virtual void CopyBufferToImage(
        const grfx::BufferToImageCopyInfo* pCopyInfo,
        grfx::Buffer*                      pSrcBuffer,
        grfx::Image*                       pDstImage) override;

    virtual void CopyImageToBuffer(
        const grfx::ImageToBufferCopyInfo* pCopyInfo,
        grfx::Image*                       pSrcImage,
        grfx::Buffer*                      pDstBuffer) override;

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
            mCurrent->Reset();
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
        UINT8 stencil;
    };

    struct ResourceBinding
    {
        grfx::D3DDescriptorType descriptorType = grfx::D3D_DESCRIPTOR_TYPE_UNDEFINED;
        UINT                    startSlot      = 0;
        std::vector<void*>      resources;

        void Reset()
        {
            resources.clear();
        }
    };

    struct IndexBufferState
    {
        ID3D11Buffer* buffer = nullptr;
        DXGI_FORMAT   format = DXGI_FORMAT_UNKNOWN;
        UINT          offset = 0;

        void Reset()
        {
            buffer = nullptr;
            format = DXGI_FORMAT_UNKNOWN;
            offset = 0;
        }
    };

    struct VertexBufferState
    {
        UINT                                               startSlot  = 0;
        UINT                                               numBuffers = 0;
        std::array<ID3D11Buffer*, PPX_MAX_VERTEX_BINDINGS> buffers    = {nullptr};
        std::array<UINT, PPX_MAX_VERTEX_BINDINGS>          strides    = {0};
        std::array<UINT, PPX_MAX_VERTEX_BINDINGS>          offsets    = {0};

        void Reset()
        {
            startSlot  = 0;
            numBuffers = 0;
        }
    };

    struct ViewportState
    {
        UINT                                          numViewports = 0;
        std::array<D3D11_VIEWPORT, PPX_MAX_VIEWPORTS> viewports    = {};

        void Reset()
        {
            numViewports = 0;
        }
    };

    struct ScissorState
    {
        UINT                                      numRects = 0;
        std::array<D3D11_RECT, PPX_MAX_VIEWPORTS> rects    = {};

        void Reset()
        {
            numRects = 0;
        }
    };

    struct ComputePipelineState
    {
        ID3D11ComputeShader* CS = nullptr;

        void Reset()
        {
            CS = nullptr;
        }
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
        ID3D11DepthStencilState* depthStencilState = nullptr;

        void Reset()
        {
            VS                = nullptr;
            HS                = nullptr;
            DS                = nullptr;
            GS                = nullptr;
            PS                = nullptr;
            inputLayout       = nullptr;
            primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
            rasterizerState   = nullptr;
        }
    };

    struct ComputeDescriptorState
    {
        std::vector<ResourceBinding> CS;

        void Reset()
        {
            //for (auto& elem : CS) {
            //    elem.Reset();
            //}
            CS.clear();
        }
    };

    struct GraphicsDescriptorState
    {
        std::vector<ResourceBinding> VS;
        std::vector<ResourceBinding> HS;
        std::vector<ResourceBinding> DS;
        std::vector<ResourceBinding> GS;
        std::vector<ResourceBinding> PS;

        void Reset()
        {
            VS.clear();
            HS.clear();
            DS.clear();
            GS.clear();
            PS.clear();

            //for (auto& elem : VS) {
            //    elem.Reset();
            //}
            //
            //for (auto& elem : HS) {
            //    elem.Reset();
            //}
            //
            //for (auto& elem : DS) {
            //    elem.Reset();
            //}
            //for (auto& elem : HS) {
            //    elem.Reset();
            //}
            //for (auto& elem : PS) {
            //    elem.Reset();
            //}
        }
    };

    static void CopyDescriptors(const DescriptorResourceBinding& srcBinding, std::vector<ResourceBinding>& dstBindings);

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
            ID3D11DepthStencilView* pView         = nullptr;
            grfx::AttachmentLoadOp  depthLoadOp   = grfx::ATTACHMENT_LOAD_OP_LOAD;
            grfx::AttachmentLoadOp  stencilLoadOp = grfx::ATTACHMENT_LOAD_OP_LOAD;
            DSVClearValue           clearValue;
        } dsv;
    };

    struct TransitionArgs
    {
        void*               resource    = nullptr;
        grfx::ResourceState beforeState = grfx::RESOURCE_STATE_UNDEFINED;
        grfx::ResourceState afterState  = grfx::RESOURCE_STATE_UNDEFINED;
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

    struct DrawIndexedArgs
    {
        UINT indexCountPerInstance = 0;
        UINT instanceCount         = 0;
        UINT startIndexLocation    = 0;
        INT  baseVertexLocation    = 0;
        UINT startInstanceLocation = 0;
    };

    struct CopyBufferToBufferArgs
    {
        grfx::BufferToBufferCopyInfo copyInfo   = {};
        grfx::Buffer*                pSrcBuffer = nullptr;
        grfx::Buffer*                pDstBuffer = nullptr;
    };

    struct CopyBufferToImageArgs
    {
        grfx::BufferToImageCopyInfo copyInfo   = {};
        grfx::Buffer*               pSrcBuffer = nullptr;
        grfx::Image*                pDstImage  = nullptr;

        //ID3D11Resource* pDstResource;
        //UINT            dstSubresource;
        //UINT            dstX;
        //UINT            dstY;
        //UINT            dstZ;
        //ID3D11Resource* pSrcResource;
        //UINT            srcSubresource;
    }; //

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
            RenderPassArgs         renderPass;
            TransitionArgs         transition;
            DispatchArgs           dispatch;
            DrawArgs               draw;
            DrawIndexedArgs        drawIndexed;
            CopyBufferToBufferArgs copyBufferToBuffer;
            CopyBufferToImageArgs  copyBufferToImage;
            ImGuiRenderArgs        imGuiRender;
        };

        CmdArgs() {}

        ~CmdArgs() {}
    };

    struct ActionCmd
    {
        Cmd     cmd  = CMD_UNKNOWN;
        CmdArgs args = {};

        uint32_t viewportStateIndex           = kInvalidStateIndex;
        uint32_t scissorStateIndex            = kInvalidStateIndex;
        uint32_t indexBuffereStateIndex       = kInvalidStateIndex;
        uint32_t vertexBufferStateIndex       = kInvalidStateIndex;
        uint32_t graphicsPipleineStateIndex   = kInvalidStateIndex;
        uint32_t graphicsDescriptorstateIndex = kInvalidStateIndex;
        uint32_t computePipleineStateIndex    = kInvalidStateIndex;
        uint32_t computeDescriptorstateIndex  = kInvalidStateIndex;

        ActionCmd(Cmd cmd_)
            : cmd(cmd_) {}

        ~ActionCmd() {}
    };

    StateStackT<ViewportState>           mViewportState;
    StateStackT<ScissorState>            mScissorState;
    StateStackT<IndexBufferState>        mIndexBufferState;
    StateStackT<VertexBufferState>       mVertexBuffersState;
    StateStackT<ComputePipelineState>    mComputePipelineState;
    StateStackT<GraphicsPipelineState>   mGraphicsPipelineState;
    StateStackT<ComputeDescriptorState>  mComputeDescriptorState;
    StateStackT<GraphicsDescriptorState> mGraphicsDescriptorState;
    std::vector<ActionCmd>               mActionCmds;
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
