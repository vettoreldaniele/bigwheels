#ifndef ppx_grfx_dx11_instance_h
#define ppx_grfx_dx11_instance_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_instance.h"

namespace ppx {
namespace grfx {
namespace dx11{

class Instance
    : public grfx::Instance
{
public:
    Instance() {}
    virtual ~Instance() {}

#if ! defined(PPX_DXVK)
    typename DXGIDebugPtr::InterfaceType*   GetDxgiDebug() const { return mDXGIDebug.Get(); }
#endif // ! defined(PPX_DXVK

    typename DXGIFactoryPtr::InterfaceType* GetDxFactory() const { return mFactory.Get(); }

protected:
    virtual Result AllocateObject(grfx::Device** ppDevice) override;
    virtual Result AllocateObject(grfx::Gpu** ppGpu) override;
    virtual Result AllocateObject(grfx::Surface** ppSurface) override;

protected:
    virtual Result CreateApiObjects(const grfx::InstanceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result EnumerateAndCreateGpus(D3D_FEATURE_LEVEL featureLevel, bool enableDebug);

private:
#if ! defined(PPX_DXVK)
    DXGIDebugPtr     mDXGIDebug;
    DXGIInfoQueuePtr mDXGIInfoQueue;
#endif // ! defined(PPX_DXVK

    DXGIFactoryPtr   mFactory;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx11_instance_h
