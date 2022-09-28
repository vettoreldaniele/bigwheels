#ifndef ppx_imgui_impl_h
#define ppx_imgui_impl_h

#include "imgui.h"

#include "grfx/grfx_config.h"

#if defined(PPX_D3D11)
struct ID3D11DeviceContext;
#endif // defined(PPX_D3D11)

#if defined(PPX_D3D12)
struct ID3D12DescriptorHeap;
#endif // defined(PPX_D3D12)

namespace ppx {

class Application;

class ImGuiImpl
{
public:
    ImGuiImpl() {}
    virtual ~ImGuiImpl() {}

    virtual Result Init(ppx::Application* pApp);
    virtual void   Shutdown(ppx::Application* pApp) = 0;
    virtual void   NewFrame();
    virtual void   Render(grfx::CommandBuffer* pCommandBuffer) = 0;

protected:
    virtual Result InitApiObjects(ppx::Application* pApp) = 0;
    void           SetColorStyle();
    virtual void   NewFrameApi() = 0;
};

class ImGuiImplVk
    : public ImGuiImpl
{
public:
    ImGuiImplVk() {}
    virtual ~ImGuiImplVk() {}

    virtual void Shutdown(ppx::Application* pApp) override;
    virtual void Render(grfx::CommandBuffer* pCommandBuffer) override;

protected:
    virtual Result InitApiObjects(ppx::Application* pApp) override;
    virtual void   NewFrameApi() override;

private:
    grfx::DescriptorPoolPtr mPool;
};

#if defined(PPX_D3D11)
class ImGuiImplDx11
    : public ImGuiImpl
{
public:
    ImGuiImplDx11() {}
    virtual ~ImGuiImplDx11() {}

    virtual void Shutdown(ppx::Application* pApp) override;
    virtual void Render(grfx::CommandBuffer* pCommandBuffer) override;

protected:
    virtual Result InitApiObjects(ppx::Application* pApp) override;
    virtual void   NewFrameApi() override;

private:
    ID3D11DeviceContext* mDeviceContext = nullptr;
};
#endif // defined(PPX_D3D11)

#if defined(PPX_D3D12)
class ImGuiImplDx12
    : public ImGuiImpl
{
public:
    ImGuiImplDx12() {}
    virtual ~ImGuiImplDx12() {}

    virtual void Shutdown(ppx::Application* pApp) override;
    virtual void Render(grfx::CommandBuffer* pCommandBuffer) override;

protected:
    virtual Result InitApiObjects(ppx::Application* pApp) override;
    virtual void   NewFrameApi() override;

private:
    ID3D12DescriptorHeap* mHeapCBVSRVUAV = nullptr;
};
#endif // defined(PPX_D3D12)

} // namespace ppx

#endif // ppx_imgui_impl_h
