#include "ppx/grfx/dx11/dx11_command_list.h"

namespace ppx::grfx::dx11 {

CommandList::CommandList()
{
    mActions.reserve(32);
}

CommandList::~CommandList()
{
}

Action& CommandList::NewAction(Cmd cmd)
{
    uint32_t id = CountU32(mActions);
    mActions.emplace_back(Action{id, cmd});
    Action& action = mActions.back();
    return action;
}

void CommandList::Reset()
{
    mRTVDSVState.Reset();

    mActions.clear();
}

void CommandList::ClearDepthStencilView(
    ID3D11DepthStencilView* pDepthStencilView,
    UINT                    ClearFlags,
    FLOAT                   Depth,
    UINT8                   Stencil)
{
    Action& action = NewAction(CMD_CLEAR_DSV);

    action.args.clearDSV.rtvDsvStateIndex  = mRTVDSVState.Commit();
    action.args.clearDSV.pDepthStencilView = pDepthStencilView;
    action.args.clearDSV.ClearFlags        = ClearFlags;
    action.args.clearDSV.Depth             = Depth;
    action.args.clearDSV.Stencil           = Stencil;
}

void CommandList::ClearRenderTargetView(
    ID3D11RenderTargetView* pRenderTargetView,
    const FLOAT             ColorRGBA[4])
{
    Action& action = NewAction(CMD_CLEAR_RTV);

    action.args.clearRTV.rtvDsvStateIndex  = mRTVDSVState.Commit();
    action.args.clearRTV.pRenderTargetView = pRenderTargetView;
    action.args.clearRTV.ColorRGBA[0]      = ColorRGBA[0];
    action.args.clearRTV.ColorRGBA[1]      = ColorRGBA[1];
    action.args.clearRTV.ColorRGBA[2]      = ColorRGBA[2];
    action.args.clearRTV.ColorRGBA[3]      = ColorRGBA[3];
}

void CommandList::OMSetRenderTargets(
    UINT                           NumViews,
    ID3D11RenderTargetView* const* ppRenderTargetViews,
    ID3D11DepthStencilView*        pDepthStencilView)
{
    if (NumViews > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) {
        PPX_ASSERT_MSG(false, "NumViews (" << NumViews << ") exceeds D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT (" << D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT << ")");
    }

    NumViews = std::min<UINT>(NumViews, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

    // Update state
    RTVDSVState* pState = mRTVDSVState.GetCurrent();
    {
        // Number of views
        pState->NumViews = NumViews;
        // Copy RTVs
        size_t size = NumViews * sizeof(ID3D11RenderTargetView*);
        std::memcpy(pState->ppRenderTargetViews, ppRenderTargetViews, size);
        // Copy DSV
        pState->pDepthStencilView = pDepthStencilView;
    }
}

void UpdateConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers,
    ConstantBufferSlots& Slots)
{
    for (UINT i = 0; i < NumBuffers; ++i) {
        UINT          slot   = i + StartSlot;
        ID3D11Buffer* buffer = IsNull(ppConstantBuffers) ? nullptr : ppConstantBuffers[i];
        Slots.Buffers[slot]  = buffer;
    }

    UINT NumSlots                       = IsNull(ppConstantBuffers) ? 0 : NumBuffers;
    Slots.Bindings[StartSlot].StartSlot = StartSlot;
    Slots.Bindings[StartSlot].NumSlots  = NumSlots;
}

void UpdateShaderResourceViews(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews,
    ShaderResourceViewSlots&         Slots)
{
    for (UINT i = 0; i < NumViews; ++i) {
        UINT                      slot = i + StartSlot;
        ID3D11ShaderResourceView* view = IsNull(ppShaderResourceViews) ? nullptr : ppShaderResourceViews[i];
        Slots.Views[slot]              = view;
    }

    UINT NumSlots                       = IsNull(ppShaderResourceViews) ? 0 : NumViews;
    Slots.Bindings[StartSlot].StartSlot = StartSlot;
    Slots.Bindings[StartSlot].NumSlots  = NumSlots;
}

void UpdateSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers,
    SamplerSlots&              Slots)
{
    for (UINT i = 0; i < NumSamplers; ++i) {
        UINT                slot    = i + StartSlot;
        ID3D11SamplerState* sampler = IsNull(ppSamplers) ? nullptr : ppSamplers[i];
        Slots.Samplers[slot]        = sampler;
    }

    UINT NumSlots                       = IsNull(ppSamplers) ? 0 : NumSamplers;
    Slots.Bindings[StartSlot].StartSlot = StartSlot;
    Slots.Bindings[StartSlot].NumSlots  = NumSlots;
}

void UpdateUnorderedAccessViews(
    UINT                              StartSlot,
    UINT                              NumViews,
    ID3D11UnorderedAccessView* const* ppUnorderedAccessViews,
    UnorderedAccessViewSlots&         Slots)
{
    for (UINT i = 0; i < NumViews; ++i) {
        UINT                       slot = i + StartSlot;
        ID3D11UnorderedAccessView* view = IsNull(ppUnorderedAccessViews) ? nullptr : ppUnorderedAccessViews[i];
        Slots.Views[slot]               = view;
    }

    UINT NumSlots                       = IsNull(ppUnorderedAccessViews) ? 0 : NumViews;
    Slots.Bindings[StartSlot].StartSlot = StartSlot;
    Slots.Bindings[StartSlot].NumSlots  = NumSlots;
}

void CommandList::PSSetConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers, pState->PS.ConstantBuffers);
}

void CommandList::PSSetShaderResources(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateShaderResourceViews(StartSlot, NumViews, ppShaderResourceViews, pState->PS.ShaderResourceViews);
}

void CommandList::PSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->PS.Samplers);
}

void CommandList::VSSetConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers, pState->VS.ConstantBuffers);
}

void CommandList::VSSetShaderResources(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateShaderResourceViews(StartSlot, NumViews, ppShaderResourceViews, pState->VS.ShaderResourceViews);
}

void CommandList::VSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->VS.Samplers);
}

static bool IndexChanged(uint32_t& execIndex, const uint32_t stateIndex)
{
    bool changed = (execIndex != stateIndex);
    if (changed) {
        execIndex = stateIndex;
    }
    return changed;
}

void CommandList::ExecuteOMSetRenderTargets(typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext, const RTVDSVState& state) const
{
    pDeviceContext->OMSetRenderTargets(
        state.NumViews,
        state.ppRenderTargetViews,
        state.pDepthStencilView);
}

void CommandList::ExecuteClearDSV(ExecutionState& execState, const Action& action) const
{
    const args::ClearDSV& args = action.args.clearDSV;
    if (IndexChanged(execState.rtvDsvStateIndex, args.rtvDsvStateIndex)) {
        const RTVDSVState& state = mRTVDSVState.At(execState.rtvDsvStateIndex);
        ExecuteOMSetRenderTargets(execState.pDeviceContext, state);
    }
    execState.pDeviceContext->ClearDepthStencilView(
        args.pDepthStencilView,
        args.ClearFlags,
        args.Depth,
        args.Stencil);
}

void CommandList::ExecuteClearRTV(ExecutionState& execState, const Action& action) const
{
    const args::ClearRTV& args = action.args.clearRTV;
    if (IndexChanged(execState.rtvDsvStateIndex, args.rtvDsvStateIndex)) {
        const RTVDSVState& state = mRTVDSVState.At(execState.rtvDsvStateIndex);
        ExecuteOMSetRenderTargets(execState.pDeviceContext, state);
    }
    execState.pDeviceContext->ClearRenderTargetView(
        args.pRenderTargetView,
        args.ColorRGBA);
}

void CommandList::Execute(typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext) const
{
    ExecutionState execState = {pDeviceContext};

    for (auto& action : mActions) {
        switch (action.cmd) {
            default: {
                PPX_ASSERT_MSG(false, "unrecognized command id: " << action.cmd);
            } break;

            case CMD_CLEAR_DSV: {
                ExecuteClearDSV(execState, action);
            } break;

            case CMD_CLEAR_RTV: {
                ExecuteClearRTV(execState, action);
            } break;
        }
    }
}

} // namespace ppx::grfx::dx11
