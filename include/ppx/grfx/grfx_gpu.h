#ifndef ppx_grfx_gpu_h
#define ppx_grfx_gpu_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

namespace internal {

struct GpuCreateInfo
{
    int32_t featureLevel = ppx::InvalidValue<int32_t>(); // D3D12
    void*   pApiObject   = nullptr;
};

} // namespace internal

class Gpu
    : public grfx::InstanceObject<grfx::internal::GpuCreateInfo>
{
public:
    Gpu() {}
    virtual ~Gpu() {}

    const char*    GetDeviceName() const { return mDeviceName.c_str(); }
    grfx::VendorId GetDeviceVendorId() const { return mDeviceVendorId; }

    virtual uint32_t GetGraphicsQueueCount() const = 0;
    virtual uint32_t GetComputeQueueCount() const  = 0;
    virtual uint32_t GetTransferQueueCount() const = 0;

protected:
    std::string    mDeviceName;
    grfx::VendorId mDeviceVendorId = grfx::VENDOR_ID_UNKNOWN;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_gpu_h