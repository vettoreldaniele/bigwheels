#ifndef ppx_grfx_scope_h
#define ppx_grfx_scope_h

#include "ppx/grfx/grfx_config.h"

namespace ppx {
namespace grfx {

class ScopeDestroyer
{
public:
    ScopeDestroyer(grfx::Device* pDevice);
    ~ScopeDestroyer();

    Result AddObject(grfx::Image* pObject);
    Result AddObject(grfx::Buffer* pObject);
    Result AddObject(grfx::Mesh* pObject);
    Result AddObject(grfx::Texture* pObject);
    Result AddObject(grfx::Queue* pParent, grfx::CommandBuffer* pObject);

private:
    grfx::DevicePtr                                                mDevice;
    std::vector<grfx::ImagePtr>                                    mImages;
    std::vector<grfx::BufferPtr>                                   mBuffers;
    std::vector<grfx::MeshPtr>                                     mMeshes;
    std::vector<grfx::TexturePtr>                                  mTextures;
    std::vector<std::pair<grfx::QueuePtr, grfx::CommandBufferPtr>> mTransientCommandBuffers;
};

} // namespace grfx
} // namespace ppx

#endif //  ppx_grfx_scope_h
