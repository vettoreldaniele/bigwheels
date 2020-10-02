#ifndef ppx_render_renderer_h
#define ppx_render_renderer_h

#include "ppx/000_config.h"
#include "ppx/render/color_pass.h"
#include "ppx/render/depth_pass.h"
#include "ppx/render/index_buffer.h"
#include "ppx/render/renderer.h"
#include "ppx/render/storage_buffer.h"
#include "ppx/render/texture.h"
#include "ppx/render/vertex_buffer.h"

namespace ppx {
namespace render {

class Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    bool Initialize(grfx::Device* pDevice);
    void Shutdown();

    bool CreateVertexBuffer(const VertexBufferCreateInfo* pCreateInfo, VertexBuffer** ppBuffer);
    void DestroyVertexBuffer(const VertexBuffer* pBuffer);

    bool AcquireNextImage(grfx::RenderPass** pRenderPass);

private:
    ObjPtr<Device>               mDevice;
    ObjPtr<Swapchain>            mSwapchain;
    std::vector<ObjPtr<ColorPass>>     mColorPasses;
    std::vector<ObjPtr<DepthPass>>     mDepthPasses;
    std::vector<ObjPtr<IndexBuffer>>   mIndexBuffers;
    std::vector<ObjPtr<StorageBuffer>> mStorageBuffers;
    std::vector<ObjPtr<Texture>>       mTextures;
    std::vector<ObjPtr<VertexBuffer>>  mVertexBuffers;
};

} // namespace render
} // namespace ppx

#endif // ppx_render_renderer_h
