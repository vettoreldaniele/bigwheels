#ifndef ppx_grfx_instance_h
#define ppx_grfx_instance_h

#include "ppx/grfx/grfx_config.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_gpu.h"

namespace ppx {
namespace grfx {

struct InstanceCreateInfo
{
    grfx::Api                api                 = grfx::API_UNDEFINED; // Direct3D or Vulkan.
    bool                     createDevices       = false;               // Create grfx::Device objects with default options.
    bool                     enableDebug         = false;               // Enable graphics API debug layers.
    bool                     enableSwapchain     = true;                // Enable support for swapchain.
    bool                     useSoftwareRenderer = false;               // Use a software renderer instead of a hardware device (WARP on DirectX).
    std::string              applicationName;                           // [OPTIONAL] Application name.
    std::string              engineName;                                // [OPTIONAL] Engine name.
    bool                     forceDxDiscreteAllocations = false;        // [OPTIONAL] Forces D3D12 to make discrete allocations for resources.
    std::vector<std::string> vulkanLayers;                              // [OPTIONAL] Additional instance layers.
    std::vector<std::string> vulkanExtensions;                          // [OPTIONAL] Additional instance extensions.
};

class Instance
{
public:
    Instance() {}
    virtual ~Instance() {}

    bool IsDebugEnabled() const { return mCreateInfo.enableDebug; }
    bool IsSwapchainEnabled() const { return mCreateInfo.enableSwapchain; }
    bool ForceDxDiscreteAllocations() const { return mCreateInfo.forceDxDiscreteAllocations; }

    grfx::Api GetApi() const { return mCreateInfo.api; }

    uint32_t GetGpuCount() const { return CountU32(mGpus); }
    Result   GetGpu(uint32_t index, grfx::Gpu** ppGpu) const;

    uint32_t GetDeviceCount() const { return CountU32(mDevices); }
    Result   GetDevice(uint32_t index, grfx::Device** ppDevice) const;

    Result CreateDevice(const grfx::DeviceCreateInfo* pCreateInfo, grfx::Device** ppDevice);
    void   DestroyDevice(const grfx::Device* pDevice);

    Result CreateSurface(const grfx::SurfaceCreateInfo* pCreateInfo, grfx::Surface** ppSurface);
    void   DestroySurface(const grfx::Surface* pSurface);

protected:
    Result CreateGpu(const grfx::internal::GpuCreateInfo* pCreateInfo, grfx::Gpu** ppGpu);
    void   DestroyGpu(const grfx::Gpu* pGpu);

    virtual Result AllocateObject(grfx::Device** ppDevice)   = 0;
    virtual Result AllocateObject(grfx::Gpu** ppGpu)         = 0;
    virtual Result AllocateObject(grfx::Surface** ppSurface) = 0;

    template <
        typename ObjectT,
        typename CreateInfoT,
        typename ContainerT = std::vector<ObjPtr<ObjectT>>>
    Result CreateObject(const CreateInfoT* pCreateInfo, ContainerT& container, ObjectT** ppObject);

    template <
        typename ObjectT,
        typename ContainerT = std::vector<ObjPtr<ObjectT>>>
    void DestroyObject(ContainerT& container, const ObjectT* pObject);

    template <typename ObjectT>
    void DestroyAllObjects(std::vector<ObjPtr<ObjectT>>& container);

protected:
    virtual Result CreateApiObjects(const grfx::InstanceCreateInfo* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                                           = 0;
    friend Result  CreateInstance(const grfx::InstanceCreateInfo* pCreateInfo, grfx::Instance** ppInstance);
    friend void    DestroyInstance(const grfx::Instance* pInstance);

private:
    virtual Result Create(const grfx::InstanceCreateInfo* pCreateInfo);
    virtual void   Destroy();
    friend Result  CreateInstance(const grfx::InstanceCreateInfo* pCreateInfo, grfx::Instance** ppInstance);
    friend void    DestroyInstance(const grfx::Instance* pInstance);

protected:
    grfx::InstanceCreateInfo      mCreateInfo = {};
    std::vector<grfx::GpuPtr>     mGpus;
    std::vector<grfx::DevicePtr>  mDevices;
    std::vector<grfx::SurfacePtr> mSurfaces;
};

Result CreateInstance(const grfx::InstanceCreateInfo* pCreateInfo, grfx::Instance** ppInstance);
void   DestroyInstance(const grfx::Instance* pInstance);

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_instance_h
