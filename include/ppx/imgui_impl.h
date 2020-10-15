#ifndef ppx_imgui_impl_h
#define ppx_imgui_impl_h

#include "imgui.h"

#include "grfx/000_grfx_config.h"

namespace ppx {

class Application;

class ImGuiImpl
{
public:
    ImGuiImpl() {}
    virtual ~ImGuiImpl() {}

    virtual Result Init(ppx::Application* pApp)                = 0;
    virtual void   Shutdown(ppx::Application* pApp)            = 0;
    virtual void   NewFrame()                                  = 0;
    virtual void   Render(grfx::CommandBuffer* pCommandBuffer) = 0;

protected:
    void SetColorStyle();
};

class ImGuiImplVk
    : public ImGuiImpl
{
public:
    ImGuiImplVk() {}
    virtual ~ImGuiImplVk() {}

    virtual Result Init(ppx::Application* pApp) override;
    virtual void   Shutdown(ppx::Application* pApp) override;
    virtual void   NewFrame() override;
    virtual void   Render(grfx::CommandBuffer* pCommandBuffer) override;

private:
    grfx::DescriptorPoolPtr mDescriptorPool;
};

#if defined(PPX_D3D12)
class ImGuiImplDx
    : public ImGuiImpl
{
public:
    ImGuiImplDx() {}
    virtual ~ImGuiImplDx() {}

    virtual Result Init(ppx::Application* pApp) override;
    virtual void   Shutdown(ppx::Application* pApp) override;
    virtual void   NewFrame() override;
    virtual void   Render(grfx::CommandBuffer* pCommandBuffer) override;

private:
};
#endif // defined(PPX_D3D12)

} // namespace ppx

#endif // ppx_imgui_impl_h
