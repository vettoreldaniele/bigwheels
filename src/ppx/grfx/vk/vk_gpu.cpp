#include "ppx/grfx/vk/vk_gpu.h"

namespace ppx {
namespace grfx {
namespace vk {

Result Gpu::CreateApiObjects(const grfx::internal::GpuCreateInfo* pCreateInfo)
{
    if (IsNull(pCreateInfo->pApiObject)) {
        return ppx::ERROR_UNEXPECTED_NULL_ARGUMENT;
    }

    mGpu = static_cast<VkPhysicalDevice>(pCreateInfo->pApiObject);

    vkGetPhysicalDeviceProperties(mGpu, &mGpuProperties);

    vkGetPhysicalDeviceFeatures(mGpu, &mGpuFeatures);

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mGpu, &count, nullptr);
    if (count > 0) {
        mQueueFamilies.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(mGpu, &count, mQueueFamilies.data());
    }

    mDeviceName     = mGpuProperties.deviceName;
    mDeviceVendorId = static_cast<grfx::VendorId>(mGpuProperties.deviceID);

    return ppx::SUCCESS;
}

void Gpu::DestroyApiObjects()
{
    if (mGpu) {
        mGpu.Reset();
    }
}

float Gpu::GetTimestampPeriod() const
{
    return mGpuProperties.limits.timestampPeriod;
}

uint32_t Gpu::GetQueueFamilyCount() const
{
    uint32_t count = CountU32(mQueueFamilies);
    return count;

}

uint32_t Gpu::GetGraphicsQueueFamilyIndex() const
{
    uint32_t queueFamilyIndex = PPX_VALUE_IGNORED;
    uint32_t count            = CountU32(mQueueFamilies);
    for (uint32_t i = 0; i < count; ++i) {
        if ((mQueueFamilies[i].queueFlags & kAllQueueMask) == kGraphicsQueueMask) {
            queueFamilyIndex = i;
            break;
        }
    }
    return queueFamilyIndex;
}

uint32_t Gpu::GetComputeQueueFamilyIndex() const
{
    uint32_t queueFamilyIndex = PPX_VALUE_IGNORED;
    uint32_t count            = CountU32(mQueueFamilies);
    for (uint32_t i = 0; i < count; ++i) {
        if ((mQueueFamilies[i].queueFlags & kAllQueueMask) == kComputeQueueMask) {
            queueFamilyIndex = i;
            break;
        }
    }
    return queueFamilyIndex;
}

uint32_t Gpu::GetTransferQueueFamilyIndex() const
{
    uint32_t queueFamilyIndex = PPX_VALUE_IGNORED;
    uint32_t count            = CountU32(mQueueFamilies);
    for (uint32_t i = 0; i < count; ++i) {
        if ((mQueueFamilies[i].queueFlags & kAllQueueMask) == kTransferQueueMask) {
            queueFamilyIndex = i;
            break;
        }
    }
    return queueFamilyIndex;
}

uint32_t Gpu::GetGraphicsQueueCount() const
{
    uint32_t count = 0;
    uint32_t index = GetGraphicsQueueFamilyIndex();
    if (index != PPX_VALUE_IGNORED) {
        count = mQueueFamilies[index].queueCount;
    }
    return count;
}

uint32_t Gpu::GetComputeQueueCount() const
{
    uint32_t count = 0;
    uint32_t index = GetComputeQueueFamilyIndex();
    if (index != PPX_VALUE_IGNORED) {
        count = mQueueFamilies[index].queueCount;
    }
    return count;
}

uint32_t Gpu::GetTransferQueueCount() const
{
    uint32_t count = 0;
    uint32_t index = GetTransferQueueFamilyIndex();
    if (index != PPX_VALUE_IGNORED) {
        count = mQueueFamilies[index].queueCount;
    }
    return count;
}

} // namespace vk
} // namespace grfx
} // namespace ppx
