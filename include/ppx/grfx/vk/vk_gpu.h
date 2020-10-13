#ifndef ppx_grfx_vk_gpu_h
#define ppx_grfx_vk_gpu_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_gpu.h"

namespace ppx {
namespace grfx {
namespace vk {

class Gpu
    : public grfx::Gpu
{
public:
    Gpu() {}
    virtual ~Gpu() {}

    VkPhysicalDevicePtr GetVkGpu() const { return mGpu; }

    uint32_t GetQueueFamilyCount() const;

    uint32_t GetGraphicsQueueFamilyIndex() const;
    uint32_t GetComputeQueueFamilyIndex() const;
    uint32_t GetTransferQueueFamilyIndex() const;

    virtual uint32_t GetGraphicsQueueCount() const override;
    virtual uint32_t GetComputeQueueCount() const override;
    virtual uint32_t GetTransferQueueCount() const override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::GpuCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkPhysicalDevicePtr                  mGpu;
    VkPhysicalDeviceProperties           mGpuProperties;
    VkPhysicalDeviceFeatures             mGpuFeatures;
    std::vector<VkQueueFamilyProperties> mQueueFamilies;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_gpu_h
