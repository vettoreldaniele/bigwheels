#ifndef ppx_grfx_dx11_gpu_h
#define ppx_grfx_dx11_gpu_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_gpu.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class Gpu
    : public grfx::Gpu
{
public:
    Gpu() {}
    virtual ~Gpu() {}

    typename DXGIAdapterPtr::InterfaceType* GetDxAdapter() const { return mGpu.Get(); }

    D3D_FEATURE_LEVEL GetFeatureLevel() const { return static_cast<D3D_FEATURE_LEVEL>(mCreateInfo.featureLevel); }

    virtual uint32_t GetGraphicsQueueCount() const override;
    virtual uint32_t GetComputeQueueCount() const override;
    virtual uint32_t GetTransferQueueCount() const override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::GpuCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    DXGIAdapterPtr mGpu;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_gpu_h
