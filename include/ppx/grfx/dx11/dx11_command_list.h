#ifndef dx11_command_list_hpp
#define dx11_command_list_hpp

#include "ppx/grfx/dx11/000_dx11_config.h"

namespace ppx::grfx::dx11 {

// -------------------------------------------------------------------------------------------------
// Commands
// -------------------------------------------------------------------------------------------------

enum Cmd
{
    CMD_UNDEFINED = 0,
    CMD_CLEAR_DSV,
    CMD_CLEAR_RTV,
};

// -------------------------------------------------------------------------------------------------
// Helpers
// -------------------------------------------------------------------------------------------------

struct SlotBindings
{
    UINT StartSlot;
    UINT NumSlots;
};

struct ConstantBufferSlots
{
    ID3D11Buffer* Buffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
    SlotBindings  Bindings[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
};

struct ShaderResourceViewSlots
{
    ID3D11ShaderResourceView* Views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
    SlotBindings              Bindings[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
};

struct SamplerSlots
{
    ID3D11SamplerState* Samplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
    SlotBindings        Bindings[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
};

struct UnorderedAccessViewSlots
{
    ID3D11UnorderedAccessView* Views[D3D11_1_UAV_SLOT_COUNT];
    SlotBindings               Bindings[D3D11_1_UAV_SLOT_COUNT];
};

struct ComputeShaderSlots
{
    ConstantBufferSlots      ConstantBuffers;
    ShaderResourceViewSlots  ShaderResourceViews;
    SamplerSlots             Samplers;
    UnorderedAccessViewSlots UnorderedAccessViews;
};

struct GraphicsShaderSlot
{
    ConstantBufferSlots     ConstantBuffers;
    ShaderResourceViewSlots ShaderResourceViews;
    SamplerSlots            Samplers;
};

// -------------------------------------------------------------------------------------------------
// States
// -------------------------------------------------------------------------------------------------

struct RTVDSVState
{
    UINT                    NumViews;
    ID3D11RenderTargetView* ppRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    ID3D11DepthStencilView* pDepthStencilView;

    void Reset()
    {
        NumViews          = 0;
        pDepthStencilView = nullptr;

        size_t size = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT * sizeof(ID3D11RenderTargetView*);
        std::memset(ppRenderTargetViews, 0, size);
    }
};
struct ComputeSlotState
{
    ComputeShaderSlots CS;

    void Reset()
    {
        std::memset(&CS, 0, sizeof(ComputeShaderSlots));
    }
};

struct GraphicsSlotState
{
    GraphicsShaderSlot VS;
    GraphicsShaderSlot HS;
    GraphicsShaderSlot DS;
    GraphicsShaderSlot GS;
    GraphicsShaderSlot PS;

    void Reset()
    {
        std::memset(&VS, 0, sizeof(GraphicsShaderSlot));
        std::memset(&HS, 0, sizeof(GraphicsShaderSlot));
        std::memset(&DS, 0, sizeof(GraphicsShaderSlot));
        std::memset(&GS, 0, sizeof(GraphicsShaderSlot));
        std::memset(&PS, 0, sizeof(GraphicsShaderSlot));
    }
};

struct ExecutionState
{
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext   = nullptr;
    uint32_t                                       rtvDsvStateIndex = dx11::kInvalidStateIndex;
};

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
        const uint32_t committedIndex = CountU32(mStack) - 1;
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

// -------------------------------------------------------------------------------------------------
// Args
// -------------------------------------------------------------------------------------------------

namespace args {

struct ClearDSV
{
    uint32_t                rtvDsvStateIndex;
    ID3D11DepthStencilView* pDepthStencilView;
    UINT                    ClearFlags;
    FLOAT                   Depth;
    UINT8                   Stencil;
};

struct ClearRTV
{
    uint32_t                rtvDsvStateIndex;
    ID3D11RenderTargetView* pRenderTargetView;
    FLOAT                   ColorRGBA[4];
};

} // namespace args

// -------------------------------------------------------------------------------------------------
// Command List
// -------------------------------------------------------------------------------------------------

struct Action
{
    uint32_t id  = dx11::kInvalidStateIndex;
    Cmd      cmd = CMD_UNDEFINED;

    struct
    {
        union
        {
            args::ClearDSV clearDSV;
            args::ClearRTV clearRTV;
        };
    } args;
};

class CommandList
{
public:
    CommandList();
    ~CommandList();

    void Reset();

    void ClearDepthStencilView(
        ID3D11DepthStencilView* pDepthStencilView,
        UINT                    ClearFlags,
        FLOAT                   Depth,
        UINT8                   Stencil);

    void ClearRenderTargetView(
        ID3D11RenderTargetView* pRenderTargetView,
        const FLOAT             ColorRGBA[4]);

    void OMSetRenderTargets(
        UINT                           NumViews,
        ID3D11RenderTargetView* const* ppRenderTargetViews,
        ID3D11DepthStencilView*        pDepthStencilView);

    void PSSetConstantBuffers(
        UINT                 StartSlot,
        UINT                 NumBuffers,
        ID3D11Buffer* const* ppConstantBuffers);

    void PSSetShaderResources(
        UINT                             StartSlot,
        UINT                             NumViews,
        ID3D11ShaderResourceView* const* ppShaderResourceViews);

    void PSSetSamplers(
        UINT                       StartSlot,
        UINT                       NumSamplers,
        ID3D11SamplerState* const* ppSamplers);

    void VSSetConstantBuffers(
        UINT                 StartSlot,
        UINT                 NumBuffers,
        ID3D11Buffer* const* ppConstantBuffers);

    void VSSetShaderResources(
        UINT                             StartSlot,
        UINT                             NumViews,
        ID3D11ShaderResourceView* const* ppShaderResourceViews);

    void VSSetSamplers(
        UINT                       StartSlot,
        UINT                       NumSamplers,
        ID3D11SamplerState* const* ppSamplers);

    void Execute(typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext) const;

private:
    Action& NewAction(Cmd cmd);

    void ExecuteOMSetRenderTargets(typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext, const RTVDSVState& state) const;
    void ExecuteClearDSV(ExecutionState& execState, const Action& action) const;
    void ExecuteClearRTV(ExecutionState& execState, const Action& action) const;

private:
    StateStackT<RTVDSVState>       mRTVDSVState;
    StateStackT<ComputeSlotState>  mComputeSlotState;
    StateStackT<GraphicsSlotState> mGraphicsSlotState;
    std::vector<Action>            mActions;
    ExecutionState                 mExecutionState;
};

} // namespace ppx::grfx::dx11

#endif //  dx11_command_list_hpp
