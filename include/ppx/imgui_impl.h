#ifndef ppx_imgui_impl_h
#define ppx_imgui_impl_h

#include "ppx/grfx/grfx_device.h"

#include "imgui.h"

namespace ppx {

class ImguiImpl
{
public:
    ImguiImpl() {}
    ~ImguiImpl() {}

    Result Init();
    void   Shutdown();
    void   NewFrame();
    void   RenderDrawData(ImDrawData* pDrawData, grfx::CommandBuffer* pCommandBuffer);
   
private:
    grfx::DevicePtr mDevice;    
};

} // namespace ppx

#endif // ppx_imgui_impl_h
