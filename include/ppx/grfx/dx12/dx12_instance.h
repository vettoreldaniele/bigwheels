#ifndef ppx_grfx_dx12_instance_h
#define ppx_grfx_dx12_instance_h

#include "ppx/grfx/dx12/000_dx12_config.h"
#include "ppx/grfx/grfx_instance.h"

namespace ppx {
namespace grfx {
namespace dx12 {

class Instance
    : public grfx::Instance
{
public:
    Instance() {}
    virtual ~Instance() {}

#if !defined(PPX_DXIIVK)
    typename DXGIDebugPtr::InterfaceType* GetDxgiDebug() const
    {
        return mDXGIDebug.Get();
    }
#endif // ! defined(PPX_DXIIVK)
    typename DXGIFactoryPtr::InterfaceType* GetDxFactory() const
    {
        return mFactory.Get();
    }

protected:
    virtual Result AllocateObject(grfx::Device** ppDevice) override;
    virtual Result AllocateObject(grfx::Gpu** ppGpu) override;
    virtual Result AllocateObject(grfx::Surface** ppSurface) override;

protected:
    virtual Result CreateApiObjects(const grfx::InstanceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result EnumerateAndCreateGpus(D3D_FEATURE_LEVEL featureLevel);

private:
#if !defined(PPX_DXIIVK)
    DXGIDebugPtr     mDXGIDebug;
    DXGIInfoQueuePtr mDXGIInfoQueue;
    D3D12DebugPtr    mD3D12Debug;
#endif // ! defined(PPX_DXIIVK)
    DXGIFactoryPtr mFactory;
};

} // namespace dx12

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx12_instance_h
