#include "ppx/grfx/dx11/dx11_command_list.h"

#include <algorithm>

namespace ppx::grfx::dx11 {

class ContextBoundState
{
public:
    ContextBoundState() {}
    ~ContextBoundState() {}

    bool VSGetBoundSRVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.VS.maxSlotSRV, (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.VS.SRVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool HSGetBoundSRVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.HS.maxSlotSRV, (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.HS.SRVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool DSGetBoundSRVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.DS.maxSlotSRV, (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.DS.SRVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool GSGetBoundSRVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.GS.maxSlotSRV, (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.GS.SRVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool PSGetBoundSRVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.PS.maxSlotSRV, (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.PS.SRVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool CSGetBoundSRVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.CS.maxSlotSRV, (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.CS.SRVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool CSGetBoundUAVSlot(const ID3D11Resource* pResource, UINT* pSlot) const
    {
        if (!IsNull(pResource)) {
            UINT n = std::min<UINT>(mBoundState.CS.maxSlotUAV, (D3D11_1_UAV_SLOT_COUNT - 1));
            for (UINT i = 0; i <= n; ++i) {
                if (mBoundState.CS.UAVs[i].Get() == pResource) {
                    *pSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    void VSSetBoundSRVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for SRV");
            return;
        }
        mBoundState.VS.SRVs[slot].Reset();
        mBoundState.VS.SRVs[slot] = resource;
        mBoundState.VS.maxSlotSRV = std::max<UINT>(mBoundState.VS.maxSlotSRV, slot);
    }

    void HSSetBoundSRVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for SRV");
            return;
        }
        mBoundState.HS.SRVs[slot].Reset();
        mBoundState.HS.SRVs[slot] = resource;
        mBoundState.HS.maxSlotSRV = std::max<UINT>(mBoundState.HS.maxSlotSRV, slot);
    }

    void DSSetBoundSRVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for SRV");
            return;
        }
        mBoundState.DS.SRVs[slot].Reset();
        mBoundState.DS.SRVs[slot] = resource;
        mBoundState.DS.maxSlotSRV = std::max<UINT>(mBoundState.DS.maxSlotSRV, slot);
    }

    void GSSetBoundSRVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for SRV");
            return;
        }
        mBoundState.GS.SRVs[slot].Reset();
        mBoundState.GS.SRVs[slot] = resource;
        mBoundState.GS.maxSlotSRV = std::max<UINT>(mBoundState.GS.maxSlotSRV, slot);
    }

    void PSSetBoundSRVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for SRV");
            return;
        }
        mBoundState.PS.SRVs[slot].Reset();
        mBoundState.PS.SRVs[slot] = resource;
        mBoundState.PS.maxSlotSRV = std::max<UINT>(mBoundState.CS.maxSlotSRV, slot);
    }

    void CSSetBoundSRVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for SRV");
            return;
        }
        mBoundState.CS.SRVs[slot].Reset();
        mBoundState.CS.SRVs[slot] = resource;
        mBoundState.CS.maxSlotSRV = std::max<UINT>(mBoundState.CS.maxSlotSRV, slot);
    }

    void CSSetBoundUAVSlot(UINT slot, const ComPtr<ID3D11Resource>& resource)
    {
        if (slot >= D3D11_1_UAV_SLOT_COUNT) {
            PPX_ASSERT_MSG(false, "invalid slot (" << slot << " for UAV");
            return;
        }
        mBoundState.CS.UAVs[slot].Reset();
        mBoundState.CS.UAVs[slot] = resource;
        mBoundState.CS.maxSlotUAV = std::max<UINT>(mBoundState.CS.maxSlotUAV, slot);
    }

private:
    struct ComputeShaderBoundState
    {
        UINT                   maxSlotSRV = 0;
        UINT                   maxSlotUAV = 0;
        ComPtr<ID3D11Resource> SRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
        ComPtr<ID3D11Resource> UAVs[D3D11_1_UAV_SLOT_COUNT];
    };

    struct GraphicsShaderBoundState
    {
        UINT                   maxSlotSRV = 0;
        ComPtr<ID3D11Resource> SRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
    };

    struct BoundState
    {
        GraphicsShaderBoundState VS = {};
        GraphicsShaderBoundState HS = {};
        GraphicsShaderBoundState DS = {};
        GraphicsShaderBoundState GS = {};
        GraphicsShaderBoundState PS = {};
        ComputeShaderBoundState  CS = {};
    };

    BoundState mBoundState = {};
};

static ContextBoundState sContextBoundState;

// -------------------------------------------------------------------------------------------------
// CommandList
// -------------------------------------------------------------------------------------------------

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
    mComputeSlotState.Reset();
    mGraphicsSlotState.Reset();
    mIndexBufferState.Reset();
    mVertexBufferState.Reset();
    mScissorState.Reset();
    mViewportState.Reset();
    mRTVDSVState.Reset();
    mPipelineState.Reset();

    mActions.clear();
}

static void UpdateConstantBuffers(
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

    UINT Index                      = Slots.NumBindings;
    UINT NumSlots                   = IsNull(ppConstantBuffers) ? 0 : NumBuffers;
    Slots.Bindings[Index].StartSlot = StartSlot;
    Slots.Bindings[Index].NumSlots  = NumSlots;
    Slots.NumBindings += 1;
}

static void UpdateShaderResourceViews(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews,
    ShaderResourceViewSlots&         Slots)
{
    for (UINT i = 0; i < NumViews; ++i) {
        UINT                      slot = i + StartSlot;
        ID3D11ShaderResourceView* view = ppShaderResourceViews[i];
        Slots.Views[slot]              = view;
    }

    UINT Index                      = Slots.NumBindings;
    Slots.Bindings[Index].StartSlot = StartSlot;
    Slots.Bindings[Index].NumSlots  = NumViews;
    Slots.NumBindings += 1;
}

static void UpdateSamplers(
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

    UINT Index                      = Slots.NumBindings;
    UINT NumSlots                   = IsNull(ppSamplers) ? 0 : NumSamplers;
    Slots.Bindings[Index].StartSlot = StartSlot;
    Slots.Bindings[Index].NumSlots  = NumSlots;
    Slots.NumBindings += 1;
}

static void UpdateUnorderedAccessViews(
    UINT                              StartSlot,
    UINT                              NumViews,
    ID3D11UnorderedAccessView* const* ppUnorderedAccessViews,
    UnorderedAccessViewSlots&         Slots)
{
    for (UINT i = 0; i < NumViews; ++i) {
        UINT                       slot = i + StartSlot;
        ID3D11UnorderedAccessView* view = ppUnorderedAccessViews[i];
        Slots.Views[slot]               = view;
    }

    UINT Index                      = Slots.NumBindings;
    Slots.Bindings[Index].StartSlot = StartSlot;
    Slots.Bindings[Index].NumSlots  = NumViews;
    Slots.NumBindings += 1;
}

void CommandList::CSSetConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers)
{
    ComputeSlotState* pState = mComputeSlotState.GetCurrent();
    UpdateConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers, pState->CS.ConstantBuffers);
}

void CommandList::CSSetShaderResources(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    ComputeSlotState* pState = mComputeSlotState.GetCurrent();
    UpdateShaderResourceViews(StartSlot, NumViews, ppShaderResourceViews, pState->CS.ShaderResourceViews);

    for (UINT i = 0; i < NumViews; ++i) {
        UINT            slot      = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppShaderResourceViews[i]->GetResource(&resource);
        sContextBoundState.CSSetBoundSRVSlot(slot, resource);
    }
}

void CommandList::CSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    ComputeSlotState* pState = mComputeSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->CS.Samplers);
}

void CommandList::CSSetUnorderedAccess(
    UINT                              StartSlot,
    UINT                              NumViews,
    ID3D11UnorderedAccessView* const* ppUnorderedAccessViews)
{
    ComputeSlotState* pState = mComputeSlotState.GetCurrent();
    UpdateUnorderedAccessViews(StartSlot, NumViews, ppUnorderedAccessViews, pState->CS.UnorderedAccessViews);

    for (UINT i = 0; i < NumViews; ++i) {
        UINT            slot      = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppUnorderedAccessViews[i]->GetResource(&resource);
        sContextBoundState.CSSetBoundUAVSlot(slot, resource);
    }
}

void CommandList::DSSetConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers, pState->DS.ConstantBuffers);
}

void CommandList::DSSetShaderResources(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateShaderResourceViews(StartSlot, NumViews, ppShaderResourceViews, pState->DS.ShaderResourceViews);

    for (UINT i = 0; i < NumViews; ++i) {
        UINT            slot      = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppShaderResourceViews[i]->GetResource(&resource);
        sContextBoundState.DSSetBoundSRVSlot(slot, resource);
    }
}

void CommandList::DSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->DS.Samplers);
}

void CommandList::GSSetConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers, pState->GS.ConstantBuffers);
}

void CommandList::GSSetShaderResources(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateShaderResourceViews(StartSlot, NumViews, ppShaderResourceViews, pState->GS.ShaderResourceViews);

    for (UINT i = 0; i < NumViews; ++i) {
        UINT            slot      = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppShaderResourceViews[i]->GetResource(&resource);
        sContextBoundState.GSSetBoundSRVSlot(slot, resource);
    }
}

void CommandList::GSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->GS.Samplers);
}

void CommandList::HSSetConstantBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers, pState->HS.ConstantBuffers);
}

void CommandList::HSSetShaderResources(
    UINT                             StartSlot,
    UINT                             NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateShaderResourceViews(StartSlot, NumViews, ppShaderResourceViews, pState->HS.ShaderResourceViews);

    for (UINT i = 0; i < NumViews; ++i) {
        UINT            slot      = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppShaderResourceViews[i]->GetResource(&resource);
        sContextBoundState.HSSetBoundSRVSlot(slot, resource);
    }
}

void CommandList::HSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->HS.Samplers);
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

    for (UINT i = 0; i < NumViews; ++i) {
        UINT                   slot = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppShaderResourceViews[i]->GetResource(&resource);
        sContextBoundState.PSSetBoundSRVSlot(slot, resource);
    }
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

    for (UINT i = 0; i < NumViews; ++i) {
        UINT            slot      = StartSlot + i;
        ComPtr<ID3D11Resource> resource;
        ppShaderResourceViews[i]->GetResource(&resource);
        sContextBoundState.VSSetBoundSRVSlot(slot, resource);
    }
}

void CommandList::VSSetSamplers(
    UINT                       StartSlot,
    UINT                       NumSamplers,
    ID3D11SamplerState* const* ppSamplers)
{
    GraphicsSlotState* pState = mGraphicsSlotState.GetCurrent();
    UpdateSamplers(StartSlot, NumSamplers, ppSamplers, pState->VS.Samplers);
}

void CommandList::IASetIndexBuffer(
    ID3D11Buffer* pIndexBuffer,
    DXGI_FORMAT   Format,
    UINT          Offset)
{
    IndexBufferState* pState = mIndexBufferState.GetCurrent();

    pState->pIndexBuffer = pIndexBuffer;
    pState->Format       = Format;
    pState->Offset       = Offset;
}

void CommandList::IASetVertexBuffers(
    UINT                 StartSlot,
    UINT                 NumBuffers,
    ID3D11Buffer* const* ppVertexBuffers,
    const UINT*          pStrides,
    const UINT*          pOffsets)
{
    VertexBufferState* pState = mVertexBufferState.GetCurrent();

    pState->StartSlot  = StartSlot;
    pState->NumBuffers = NumBuffers;
    for (UINT i = 0; i < NumBuffers; ++i) {
        pState->ppVertexBuffers[i] = ppVertexBuffers[i];
        pState->pStrides[i]        = pStrides[i];
        pState->pOffsets[i]        = pOffsets[i];
    }
}

void CommandList::RSSetScissorRects(
    UINT              NumRects,
    const D3D11_RECT* pRects)
{
    ScissorState* pState = mScissorState.GetCurrent();

    pState->NumRects = NumRects;
    for (UINT i = 0; i < NumRects; ++i) {
        memcpy(&pState->pRects[i], &pRects[i], sizeof(D3D11_RECT));
    }
}

void CommandList::RSSetViewports(
    UINT                  NumViewports,
    const D3D11_VIEWPORT* pViewports)
{
    ViewportState* pState = mViewportState.GetCurrent();

    pState->NumViewports = NumViewports;
    for (UINT i = 0; i < NumViewports; ++i) {
        memcpy(&pState->pViewports[i], &pViewports[i], sizeof(D3D11_VIEWPORT));
    }
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

void CommandList::SetPipelineState(const PipelineState* pPipelinestate)
{
    PipelineState* pState = mPipelineState.GetCurrent();

    size_t size = sizeof(PipelineState);
    std::memcpy(pState, pPipelinestate, size);
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

void CommandList::Nullify(
    ID3D11Resource* pResource,
    NullifyType     Type)
{
    Action& action = NewAction(CMD_NULLIFY);

    action.args.nullify.pResource = pResource;
    action.args.nullify.Type      = Type;
}

void CommandList::Dispatch(
    UINT ThreadGroupCountX,
    UINT ThreadGroupCountY,
    UINT ThreadGroupCountZ)
{
    Action& action = NewAction(CMD_DISPATCH);

    action.args.dispatch.computeSlotStateIndex = mComputeSlotState.Commit();
    action.args.dispatch.pipelineStateIndex    = mPipelineState.Commit();

    action.args.dispatch.ThreadGroupCountX = ThreadGroupCountX;
    action.args.dispatch.ThreadGroupCountY = ThreadGroupCountY;
    action.args.dispatch.ThreadGroupCountZ = ThreadGroupCountZ;
}

void CommandList::DrawInstanced(
    UINT VertexCountPerInstance,
    UINT InstanceCount,
    UINT StartVertexLocation,
    UINT StartInstanceLocation)
{
    Action& action = NewAction(CMD_DRAW);

    action.args.draw.graphicsSlotStateIndex = mGraphicsSlotState.Commit();
    action.args.draw.vertexBufferStateIndex = mVertexBufferState.Commit();
    action.args.draw.scissorStateIndex      = mScissorState.Commit();
    action.args.draw.viewportStateIndex     = mViewportState.Commit();
    action.args.draw.rtvDsvStateIndex       = mRTVDSVState.Commit();
    action.args.draw.pipelineStateIndex     = mPipelineState.Commit();

    action.args.draw.VertexCountPerInstance = VertexCountPerInstance;
    action.args.draw.InstanceCount          = InstanceCount;
    action.args.draw.StartVertexLocation    = StartVertexLocation;
    action.args.draw.StartInstanceLocation  = StartInstanceLocation;
}

void CommandList::DrawIndexedInstanced(
    UINT IndexCountPerInstance,
    UINT InstanceCount,
    UINT StartIndexLocation,
    INT  BaseVertexLocation,
    UINT StartInstanceLocation)
{
    Action& action = NewAction(CMD_DRAW_INDEXED);

    action.args.drawIndexed.graphicsSlotStateIndex = mGraphicsSlotState.Commit();
    action.args.drawIndexed.indexBufferStateIndex  = mIndexBufferState.Commit();
    action.args.drawIndexed.vertexBufferStateIndex = mVertexBufferState.Commit();
    action.args.drawIndexed.scissorStateIndex      = mScissorState.Commit();
    action.args.drawIndexed.viewportStateIndex     = mViewportState.Commit();
    action.args.drawIndexed.rtvDsvStateIndex       = mRTVDSVState.Commit();
    action.args.drawIndexed.pipelineStateIndex     = mPipelineState.Commit();

    action.args.drawIndexed.IndexCountPerInstance = IndexCountPerInstance;
    action.args.drawIndexed.InstanceCount         = InstanceCount;
    action.args.drawIndexed.StartIndexLocation    = StartIndexLocation;
    action.args.drawIndexed.BaseVertexLocation    = BaseVertexLocation;
    action.args.drawIndexed.StartInstanceLocation = StartInstanceLocation;
}

void CommandList::CopyBufferToBuffer(const args::CopyBufferToBuffer* pCopyArgs)
{
    Action& action = NewAction(CMD_COPY_BUFFER_TO_BUFFER);

    std::memcpy(&action.args.copyBufferToBuffer, pCopyArgs, sizeof(args::CopyBufferToBuffer));
}

void CommandList::CopyBufferToImage(const args::CopyBufferToImage* pCopyArgs)
{
    Action& action = NewAction(CMD_COPY_BUFFER_TO_IMAGE);

    std::memcpy(&action.args.copyBufferToImage, pCopyArgs, sizeof(args::CopyBufferToImage));
}

void CommandList::ImGuiRender(void (*pFn)(void))
{
    Action& action = NewAction(CMD_IMGUI_RENDER);

    action.args.imGuiRender.pRenderFn = pFn;
}

static bool ExecuteIndexChanged(uint32_t& execIndex, const uint32_t stateIndex)
{
    bool changed = (execIndex != stateIndex);
    if (changed) {
        execIndex = stateIndex;
    }
    return changed;
}

static void ExecuteSetConstantBufferSlots(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    void (D3D11DeviceContextPtr::InterfaceType::*SetConstantBuffersFn)(UINT, UINT, ID3D11Buffer* const*),
    const ConstantBufferSlots& slots)
{
    for (UINT i = 0; i < slots.NumBindings; ++i) {
        const SlotBindings&  binding   = slots.Bindings[i];
        ID3D11Buffer* const* ppBuffers = &slots.Buffers[binding.StartSlot];
        (pDeviceContext->*SetConstantBuffersFn)(binding.StartSlot, binding.NumSlots, ppBuffers);
    }
}

static void ExecuteSetShaderResourceViewSlots(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    void (D3D11DeviceContextPtr::InterfaceType::*SetShaderResourceViewsFn)(UINT, UINT, ID3D11ShaderResourceView* const*),
    const ShaderResourceViewSlots& slots)
{
    for (UINT i = 0; i < slots.NumBindings; ++i) {
        const SlotBindings&              binding               = slots.Bindings[i];
        ID3D11ShaderResourceView* const* ppShaderResourceViews = &slots.Views[binding.StartSlot];
        (pDeviceContext->*SetShaderResourceViewsFn)(binding.StartSlot, binding.NumSlots, ppShaderResourceViews);
    }
}

static void ExecuteSetSamplerSlots(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    void (D3D11DeviceContextPtr::InterfaceType::*SetSamplersFn)(UINT, UINT, ID3D11SamplerState* const*),
    const SamplerSlots& slots)
{
    for (UINT i = 0; i < slots.NumBindings; ++i) {
        const SlotBindings&        binding    = slots.Bindings[i];
        ID3D11SamplerState* const* ppSamplers = &slots.Samplers[binding.StartSlot];
        (pDeviceContext->*SetSamplersFn)(binding.StartSlot, binding.NumSlots, ppSamplers);
    }
}

static void ExecuteSetUnorderedAccessViewSlots(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    void (D3D11DeviceContextPtr::InterfaceType::*SetUnorderedAccessViewsFn)(UINT, UINT, ID3D11UnorderedAccessView* const*, const UINT*),
    const UnorderedAccessViewSlots& slots)
{
    for (UINT i = 0; i < slots.NumBindings; ++i) {
        const SlotBindings&               binding                = slots.Bindings[i];
        ID3D11UnorderedAccessView* const* ppUnorderedAccessViews = &slots.Views[binding.StartSlot];
        (pDeviceContext->*SetUnorderedAccessViewsFn)(binding.StartSlot, binding.NumSlots, ppUnorderedAccessViews, nullptr);
    }
}

static void ExecuteSetComputeSlotState(
    ExecutionState&         execState,
    const ComputeSlotState& state)
{
    ExecuteSetConstantBufferSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::CSSetConstantBuffers,
        state.CS.ConstantBuffers);
    ExecuteSetShaderResourceViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::CSSetShaderResources,
        state.CS.ShaderResourceViews);
    ExecuteSetSamplerSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::CSSetSamplers,
        state.CS.Samplers);
    ExecuteSetUnorderedAccessViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::CSSetUnorderedAccessViews,
        state.CS.UnorderedAccessViews);
}

static void ExecuteSetGraphicsSlotState(
    ExecutionState&          execState,
    const GraphicsSlotState& state)
{
    // VS
    ExecuteSetConstantBufferSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::VSSetConstantBuffers,
        state.VS.ConstantBuffers);
    ExecuteSetShaderResourceViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::VSSetShaderResources,
        state.VS.ShaderResourceViews);
    ExecuteSetSamplerSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::VSSetSamplers,
        state.VS.Samplers);

    // HS
    ExecuteSetConstantBufferSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::HSSetConstantBuffers,
        state.HS.ConstantBuffers);
    ExecuteSetShaderResourceViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::HSSetShaderResources,
        state.HS.ShaderResourceViews);
    ExecuteSetSamplerSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::HSSetSamplers,
        state.HS.Samplers);

    // DS
    ExecuteSetConstantBufferSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::DSSetConstantBuffers,
        state.DS.ConstantBuffers);
    ExecuteSetShaderResourceViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::DSSetShaderResources,
        state.DS.ShaderResourceViews);
    ExecuteSetSamplerSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::DSSetSamplers,
        state.DS.Samplers);

    // GS
    ExecuteSetConstantBufferSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::GSSetConstantBuffers,
        state.GS.ConstantBuffers);
    ExecuteSetShaderResourceViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::GSSetShaderResources,
        state.GS.ShaderResourceViews);
    ExecuteSetSamplerSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::GSSetSamplers,
        state.GS.Samplers);

    // PS
    ExecuteSetConstantBufferSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::PSSetConstantBuffers,
        state.PS.ConstantBuffers);
    ExecuteSetShaderResourceViewSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::PSSetShaderResources,
        state.PS.ShaderResourceViews);
    ExecuteSetSamplerSlots(
        execState.pDeviceContext,
        &D3D11DeviceContextPtr::InterfaceType::PSSetSamplers,
        state.PS.Samplers);
}

static void ExecuteIASetIndexBuffer(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const IndexBufferState&                        state)
{
    pDeviceContext->IASetIndexBuffer(
        state.pIndexBuffer,
        state.Format,
        state.Offset);
}

static void ExecuteIASetVertexBuffers(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const VertexBufferState&                       state)
{
    pDeviceContext->IASetVertexBuffers(
        state.StartSlot,
        state.NumBuffers,
        state.ppVertexBuffers,
        state.pStrides,
        state.pOffsets);
}

static void ExecuteRSSetScissorRects(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const ScissorState&                            state)
{
    pDeviceContext->RSSetScissorRects(
        state.NumRects,
        state.pRects);
}

static void ExecuteRSSetViewports(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const ViewportState&                           state)
{
    pDeviceContext->RSSetViewports(
        state.NumViewports,
        state.pViewports);
}

static void ExecuteOMSetRenderTargets(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const RTVDSVState&                             state)
{
    pDeviceContext->OMSetRenderTargets(
        state.NumViews,
        state.ppRenderTargetViews,
        state.pDepthStencilView);
}

static void ExecuteSetComputePipelines(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const PipelineState&                           state)
{
    pDeviceContext->CSSetShader(state.CS, nullptr, 0);
}

static void ExecuteSetGraphicsPipelines(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const PipelineState&                           state)
{
    // clang-format off
    pDeviceContext->VSSetShader(state.VS, nullptr, 0);
    if (!IsNull(state.HS)) pDeviceContext->HSSetShader(state.HS, nullptr, 0);
    if (!IsNull(state.DS)) pDeviceContext->DSSetShader(state.DS, nullptr, 0);
    if (!IsNull(state.GS)) pDeviceContext->GSSetShader(state.GS, nullptr, 0);
    if (!IsNull(state.PS)) pDeviceContext->PSSetShader(state.PS, nullptr, 0);
    // clang-format on

    pDeviceContext->IASetInputLayout(state.InputLayout);
    pDeviceContext->IASetPrimitiveTopology(state.PrimitiveTopology);
    pDeviceContext->RSSetState(state.RasterizerState);

    // @TODO: Figure out how to properly determine the StencilRef value
    //
    const UINT stencilRef = 1;
    pDeviceContext->OMSetDepthStencilState(state.DepthStencilState, stencilRef);
}

static void ExecuteNullify(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const args::Nullify&                           args)
{
    ID3D11ShaderResourceView*  nullSRV[1] = {nullptr};
    ID3D11UnorderedAccessView* nullUAV[1] = {nullptr};

    if (args.Type == NULLIFY_TYPE_SRV) {
        UINT slot = UINT32_MAX;
        if (sContextBoundState.VSGetBoundSRVSlot(args.pResource, &slot)) {
            pDeviceContext->VSSetShaderResources(slot, 1, nullSRV);
            sContextBoundState.VSSetBoundSRVSlot(slot, ComPtr<ID3D11Resource>());
        }
        if (sContextBoundState.HSGetBoundSRVSlot(args.pResource, &slot)) {
            pDeviceContext->HSSetShaderResources(slot, 1, nullSRV);
            sContextBoundState.HSSetBoundSRVSlot(slot, ComPtr<ID3D11Resource>());
        }
        if (sContextBoundState.DSGetBoundSRVSlot(args.pResource, &slot)) {
            pDeviceContext->DSSetShaderResources(slot, 1, nullSRV);
            sContextBoundState.DSSetBoundSRVSlot(slot, ComPtr<ID3D11Resource>());
        }
        if (sContextBoundState.GSGetBoundSRVSlot(args.pResource, &slot)) {
            pDeviceContext->GSSetShaderResources(slot, 1, nullSRV);
            sContextBoundState.GSSetBoundSRVSlot(slot, ComPtr<ID3D11Resource>());
        }
        if (sContextBoundState.PSGetBoundSRVSlot(args.pResource, &slot)) {
            pDeviceContext->PSSetShaderResources(slot, 1, nullSRV);
            sContextBoundState.PSSetBoundSRVSlot(slot, ComPtr<ID3D11Resource>());
        }
        if (sContextBoundState.CSGetBoundSRVSlot(args.pResource, &slot)) {
            pDeviceContext->CSSetShaderResources(slot, 1, nullSRV);
            sContextBoundState.CSSetBoundSRVSlot(slot, ComPtr<ID3D11Resource>());
        }
    }
    else if (args.Type == NULLIFY_TYPE_UAV) {
        UINT slot = UINT32_MAX;
        if (sContextBoundState.CSGetBoundUAVSlot(args.pResource, &slot)) {
            pDeviceContext->CSSetUnorderedAccessViews(slot, 1, nullUAV, nullptr);
            sContextBoundState.CSSetBoundUAVSlot(slot, ComPtr<ID3D11Resource>());
        }
    }
}

static void ExecuteCopyBufferToBuffer(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const args::CopyBufferToBuffer&                args)
{
    D3D11_BOX srcBox = {};
    srcBox.left      = static_cast<UINT>(args.srcBufferOffset);
    srcBox.top       = 0;
    srcBox.front     = 0;
    srcBox.right     = static_cast<UINT>(args.dstBufferOffset + args.size);
    srcBox.bottom    = 1;
    srcBox.back      = 1;

    pDeviceContext->CopySubresourceRegion1(
        args.pDstResource,
        0,
        static_cast<UINT>(args.dstBufferOffset),
        0,
        0,
        args.pSrcResource,
        0,
        &srcBox,
        D3D11_COPY_DISCARD);
}

static void ExecuteCopyBufferToImage(
    typename D3D11DeviceContextPtr::InterfaceType* pDeviceContext,
    const args::CopyBufferToImage&                 args)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubres = {};
    HRESULT                  hr           = pDeviceContext->Map(args.pSrcResource, 0, args.mapType, 0, &mappedSubres);
    if (FAILED(hr)) {
        PPX_ASSERT_MSG(false, "could not map src buffer memory");
    }

    const char* pMappedAddress = static_cast<const char*>(mappedSubres.pData);
    for (uint32_t i = 0; i < args.dstImage.arrayLayerCount; ++i) {
        uint32_t arrayLayer       = args.dstImage.arrayLayer + i;
        UINT     subresourceIndex = static_cast<UINT>((args.dstImage.arrayLayer * args.mipSpan) + args.dstImage.mipLevel);

        D3D11_BOX dstBox = {};
        dstBox.left      = args.dstImage.x;
        dstBox.top       = args.dstImage.y;
        dstBox.front     = args.dstImage.z;
        dstBox.right     = args.dstImage.x + args.dstImage.width;
        dstBox.bottom    = args.dstImage.y + args.dstImage.height;
        dstBox.back      = args.dstImage.z + args.dstImage.depth;

        const char* pSrcData = pMappedAddress + args.srcBuffer.footprintOffset;

        UINT srcRowPitch   = static_cast<UINT>(args.srcBuffer.imageRowStride);
        UINT srcDepthPitch = static_cast<UINT>(args.srcBuffer.imageRowStride * args.srcBuffer.imageHeight);

        D3D11_COPY_FLAGS copyFlags = D3D11_COPY_DISCARD;
        if (args.isCube) {
            copyFlags = D3D11_COPY_NO_OVERWRITE;
        }

        pDeviceContext->UpdateSubresource1(
            args.pDstResource,
            subresourceIndex,
            &dstBox,
            pSrcData,
            srcRowPitch,
            srcDepthPitch,
            copyFlags);
    }
}

void CommandList::ExecuteClearDSV(ExecutionState& execState, const Action& action) const
{
    const args::ClearDSV& args = action.args.clearDSV;

    if (ExecuteIndexChanged(execState.rtvDsvStateIndex, args.rtvDsvStateIndex)) {
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

    if (ExecuteIndexChanged(execState.rtvDsvStateIndex, args.rtvDsvStateIndex)) {
        const RTVDSVState& state = mRTVDSVState.At(execState.rtvDsvStateIndex);
        ExecuteOMSetRenderTargets(execState.pDeviceContext, state);
    }

    execState.pDeviceContext->ClearRenderTargetView(
        args.pRenderTargetView,
        args.ColorRGBA);
}

void CommandList::ExecuteDispatch(ExecutionState& execState, const Action& action) const
{
    const args::Dispatch& args = action.args.dispatch;

    if (ExecuteIndexChanged(execState.computeSlotStateIndex, args.computeSlotStateIndex)) {
        const ComputeSlotState& state = mComputeSlotState.At(execState.computeSlotStateIndex);
        ExecuteSetComputeSlotState(execState, state);
    }
    if (ExecuteIndexChanged(execState.pipelineStateIndex, args.pipelineStateIndex)) {
        const PipelineState& state = mPipelineState.At(execState.pipelineStateIndex);
        ExecuteSetComputePipelines(execState.pDeviceContext, state);
    }

    execState.pDeviceContext->Dispatch(
        args.ThreadGroupCountX,
        args.ThreadGroupCountY,
        args.ThreadGroupCountZ);
}

void CommandList::ExecuteDraw(ExecutionState& execState, const Action& action) const
{
    const args::Draw& args = action.args.draw;
    if (ExecuteIndexChanged(execState.graphicsSlotStateIndex, args.graphicsSlotStateIndex)) {
        const GraphicsSlotState& state = mGraphicsSlotState.At(execState.graphicsSlotStateIndex);
        ExecuteSetGraphicsSlotState(execState, state);
    }
    if (ExecuteIndexChanged(execState.vertexBufferStateIndex, args.vertexBufferStateIndex)) {
        const VertexBufferState& state = mVertexBufferState.At(execState.vertexBufferStateIndex);
        ExecuteIASetVertexBuffers(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.scissorStateIndex, args.scissorStateIndex)) {
        const ScissorState& state = mScissorState.At(execState.scissorStateIndex);
        ExecuteRSSetScissorRects(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.viewportStateIndex, args.viewportStateIndex)) {
        const ViewportState& state = mViewportState.At(execState.viewportStateIndex);
        ExecuteRSSetViewports(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.rtvDsvStateIndex, args.rtvDsvStateIndex)) {
        const RTVDSVState& state = mRTVDSVState.At(execState.rtvDsvStateIndex);
        ExecuteOMSetRenderTargets(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.pipelineStateIndex, args.pipelineStateIndex)) {
        const PipelineState& state = mPipelineState.At(execState.pipelineStateIndex);
        ExecuteSetGraphicsPipelines(execState.pDeviceContext, state);
    }

    execState.pDeviceContext->DrawInstanced(
        args.VertexCountPerInstance,
        args.InstanceCount,
        args.StartVertexLocation,
        args.StartInstanceLocation);
}

void CommandList::ExecuteDrawIndexed(ExecutionState& execState, const Action& action) const
{
    const args::DrawIndexed& args = action.args.drawIndexed;
    if (ExecuteIndexChanged(execState.graphicsSlotStateIndex, args.graphicsSlotStateIndex)) {
        const GraphicsSlotState& state = mGraphicsSlotState.At(execState.graphicsSlotStateIndex);
        ExecuteSetGraphicsSlotState(execState, state);
    }
    if (ExecuteIndexChanged(execState.indexBufferStateIndex, args.indexBufferStateIndex)) {
        const IndexBufferState& state = mIndexBufferState.At(execState.indexBufferStateIndex);
        ExecuteIASetIndexBuffer(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.vertexBufferStateIndex, args.vertexBufferStateIndex)) {
        const VertexBufferState& state = mVertexBufferState.At(execState.vertexBufferStateIndex);
        ExecuteIASetVertexBuffers(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.scissorStateIndex, args.scissorStateIndex)) {
        const ScissorState& state = mScissorState.At(execState.scissorStateIndex);
        ExecuteRSSetScissorRects(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.viewportStateIndex, args.viewportStateIndex)) {
        const ViewportState& state = mViewportState.At(execState.viewportStateIndex);
        ExecuteRSSetViewports(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.rtvDsvStateIndex, args.rtvDsvStateIndex)) {
        const RTVDSVState& state = mRTVDSVState.At(execState.rtvDsvStateIndex);
        ExecuteOMSetRenderTargets(execState.pDeviceContext, state);
    }
    if (ExecuteIndexChanged(execState.pipelineStateIndex, args.pipelineStateIndex)) {
        const PipelineState& state = mPipelineState.At(execState.pipelineStateIndex);
        ExecuteSetGraphicsPipelines(execState.pDeviceContext, state);
    }

    execState.pDeviceContext->DrawIndexedInstanced(
        args.IndexCountPerInstance,
        args.InstanceCount,
        args.StartIndexLocation,
        args.BaseVertexLocation,
        args.StartInstanceLocation);
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

            case CMD_NULLIFY: {
                ExecuteNullify(pDeviceContext, action.args.nullify);
            } break;

            case CMD_DISPATCH: {
                ExecuteDispatch(execState, action);
            } break;

            case CMD_DRAW: {
                ExecuteDraw(execState, action);
            } break;

            case CMD_DRAW_INDEXED: {
                ExecuteDrawIndexed(execState, action);
            } break;

            case CMD_COPY_BUFFER_TO_BUFFER: {
                ExecuteCopyBufferToBuffer(pDeviceContext, action.args.copyBufferToBuffer);

            } break;

            case CMD_COPY_BUFFER_TO_IMAGE: {
                ExecuteCopyBufferToImage(pDeviceContext, action.args.copyBufferToImage);
            } break;

            case CMD_IMGUI_RENDER: {
                void (*pRenderFn)(void) = action.args.imGuiRender.pRenderFn;
                pRenderFn();
            } break;
        }
    }
}

} // namespace ppx::grfx::dx11
