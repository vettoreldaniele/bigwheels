#include "ppx/grfx/vk/vk_device.h"
#include "ppx/grfx/vk/vk_buffer.h"
#include "ppx/grfx/vk/vk_command.h"
#include "ppx/grfx/vk/vk_descriptor.h"
#include "ppx/grfx/vk/vk_gpu.h"
#include "ppx/grfx/vk/vk_image.h"
#include "ppx/grfx/vk/vk_instance.h"
#include "ppx/grfx/vk/vk_pipeline.h"
#include "ppx/grfx/vk/vk_queue.h"
#include "ppx/grfx/vk/vk_render_pass.h"
#include "ppx/grfx/vk/vk_shader.h"
#include "ppx/grfx/vk/vk_swapchain.h"
#include "ppx/grfx/vk/vk_sync.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace ppx {
namespace grfx {
namespace vk {

Result Device::ConfigureQueueInfo(const grfx::DeviceCreateInfo* pCreateInfo, std::vector<float>& queuePriorities, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos)
{
    VkPhysicalDevicePtr gpu = ToApi(pCreateInfo->pGpu)->GetVkGpu();

    // Queue priorities
    {
        uint32_t maxQueueCount = std::max(pCreateInfo->pGpu->GetGraphicsQueueCount(), pCreateInfo->pGpu->GetComputeQueueCount());
        maxQueueCount          = std::max(maxQueueCount, pCreateInfo->pGpu->GetTransferQueueCount());

        for (uint32_t i = 0; i < maxQueueCount; ++i) {
            queuePriorities.push_back(1.0f);
        }
    }

    // Queues
    {
        // Graphics
        uint32_t queueCount = pCreateInfo->pGpu->GetGraphicsQueueCount();
        if (queueCount > 0) {
            VkDeviceQueueCreateInfo vkci = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
            vkci.queueFamilyIndex        = ToApi(pCreateInfo->pGpu)->GetGraphicsQueueFamilyIndex();
            vkci.queueCount              = queueCount;
            vkci.pQueuePriorities        = DataPtr(queuePriorities);
            queueCreateInfos.push_back(vkci);
        }
        // Compute
        queueCount = pCreateInfo->pGpu->GetComputeQueueCount();
        if (queueCount > 0) {
            VkDeviceQueueCreateInfo vkci = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
            vkci.queueFamilyIndex        = ToApi(pCreateInfo->pGpu)->GetComputeQueueFamilyIndex();
            vkci.queueCount              = queueCount;
            vkci.pQueuePriorities        = DataPtr(queuePriorities);
            queueCreateInfos.push_back(vkci);
        }
        // Transfer
        queueCount = pCreateInfo->pGpu->GetTransferQueueCount();
        if (queueCount > 0) {
            VkDeviceQueueCreateInfo vkci = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
            vkci.queueFamilyIndex        = ToApi(pCreateInfo->pGpu)->GetTransferQueueFamilyIndex();
            vkci.queueCount              = queueCount;
            vkci.pQueuePriorities        = DataPtr(queuePriorities);
            queueCreateInfos.push_back(vkci);
        }
    }

    return ppx::SUCCESS;
}

Result Device::ConfigureExtensions(const grfx::DeviceCreateInfo* pCreateInfo)
{
    VkPhysicalDevicePtr gpu = ToApi(pCreateInfo->pGpu)->GetVkGpu();

    // Enumerate extensions
    uint32_t count = 0;
    VkResult vkres = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    if (vkres != VK_SUCCESS) {
        PPX_ASSERT_MSG((vkres == VK_SUCCESS), "vkEnumerateDeviceExtensionProperties(0) failed");
        return ppx::ERROR_API_FAILURE;
    }

    if (count > 0) {
        std::vector<VkExtensionProperties> properties(count);
        vkres = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, properties.data());
        if (vkres != VK_SUCCESS) {
            PPX_ASSERT_MSG((vkres == VK_SUCCESS), "vkEnumerateDeviceExtensionProperties(1) failed");
            return ppx::ERROR_API_FAILURE;
        }

        for (auto& elem : properties) {
            mFoundExtensions.push_back(elem.extensionName);
        }
        Unique(mFoundExtensions);
    }

    // Swapchains extension
    if (GetInstance()->IsSwapchainEnabled()) {
        mExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    // Add Vulkan 1.1 extensions:
    //   - VK_EXT_descriptor_indexing (promoted to core in 1.2)
    //   - VK_KHR_timeline_semaphore (promoted to core in 1.2)
    //
    if (GetInstance()->GetApi() == grfx::API_VK_1_1) {
        // Descriptor indexing
        mExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

        // Timeline semaphore - if present
        if (ElementExists(std::string(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME), mFoundExtensions)) {
            mExtensions.push_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
        }
    }

#if defined(PPX_VK_EXTENDED_DYNAMIC_STATE)
    if (ElementExists(std::string(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME), mFoundExtensions)) {
        mExtensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
    }
#endif // defined(PPX_VK_EXTENDED_DYNAMIC_STATE)

    // Depth clip
    if (ElementExists(std::string(VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME), mFoundExtensions)) {
        mExtensions.push_back(VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME);
    }


    // Add additional extensions and uniquify
    AppendElements(pCreateInfo->vulkanExtensions, mExtensions);
    Unique(mExtensions);

    return ppx::SUCCESS;
}

Result Device::ConfigurFeatures(const grfx::DeviceCreateInfo* pCreateInfo, VkPhysicalDeviceFeatures& features)
{
    // Default device faetures
    features                    = {};
    features.geometryShader     = VK_TRUE;
    features.tessellationShader = VK_TRUE;

    // Select between default or custom features
    if (!IsNull(pCreateInfo->pVulkanDeviceFeatures)) {
        const VkPhysicalDeviceFeatures* pFeatures = static_cast<const VkPhysicalDeviceFeatures*>(pCreateInfo->pVulkanDeviceFeatures);
        features                                  = *pFeatures;
    }

    return ppx::SUCCESS;
}

Result Device::CreateQueues(const grfx::DeviceCreateInfo* pCreateInfo)
{
    if (pCreateInfo->graphicsQueueCount > 0) {
        uint32_t queueFamilyIndex = ToApi(pCreateInfo->pGpu)->GetGraphicsQueueFamilyIndex();
        for (uint32_t queueIndex = 0; queueIndex < pCreateInfo->graphicsQueueCount; ++queueIndex) {
            grfx::internal::QueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.queueFamilyIndex                = queueFamilyIndex;
            queueCreateInfo.queueIndex                      = queueIndex;

            grfx::QueuePtr tmpQueue;
            Result         ppxres = CreateGraphicsQueue(&queueCreateInfo, &tmpQueue);
            if (Failed(ppxres)) {
                return ppxres;
            }
        }
    }

    if (pCreateInfo->computeQueueCount > 0) {
        uint32_t queueFamilyIndex = ToApi(pCreateInfo->pGpu)->GetComputeQueueFamilyIndex();
        for (uint32_t queueIndex = 0; queueIndex < pCreateInfo->computeQueueCount; ++queueIndex) {
            grfx::internal::QueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.queueFamilyIndex                = queueFamilyIndex;
            queueCreateInfo.queueIndex                      = queueIndex;

            grfx::QueuePtr tmpQueue;
            Result         ppxres = CreateComputeQueue(&queueCreateInfo, &tmpQueue);
            if (Failed(ppxres)) {
                return ppxres;
            }
        }
    }

    if (pCreateInfo->transferQueueCount > 0) {
        uint32_t queueFamilyIndex = ToApi(pCreateInfo->pGpu)->GetTransferQueueFamilyIndex();
        for (uint32_t queueIndex = 0; queueIndex < pCreateInfo->transferQueueCount; ++queueIndex) {
            grfx::internal::QueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.queueFamilyIndex                = queueFamilyIndex;
            queueCreateInfo.queueIndex                      = queueIndex;

            grfx::QueuePtr tmpQueue;
            Result         ppxres = CreateTransferQueue(&queueCreateInfo, &tmpQueue);
            if (Failed(ppxres)) {
                return ppxres;
            }
        }
    }

    return ppx::SUCCESS;
}

Result Device::CreateApiObjects(const grfx::DeviceCreateInfo* pCreateInfo)
{
    std::vector<float>                   queuePriorities;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    VkPhysicalDeviceFeatures             features;

    Result ppxres = ConfigureQueueInfo(pCreateInfo, queuePriorities, queueCreateInfos);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = ConfigureExtensions(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = ConfigurFeatures(pCreateInfo, features);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Get C strings
    std::vector<const char*> extensions = GetCStrings(mExtensions);

    VkDeviceCreateInfo vkci      = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    vkci.flags                   = 0;
    vkci.queueCreateInfoCount    = CountU32(queueCreateInfos);
    vkci.pQueueCreateInfos       = DataPtr(queueCreateInfos);
    vkci.enabledLayerCount       = 0;
    vkci.ppEnabledLayerNames     = nullptr;
    vkci.enabledExtensionCount   = CountU32(extensions);
    vkci.ppEnabledExtensionNames = DataPtr(extensions);
    vkci.pEnabledFeatures        = &features;

    // Log layers and extensions
    {
        PPX_LOG_INFO("Loading " << vkci.enabledExtensionCount << " Vulkan instance extensions");
        for (uint32_t i = 0; i < vkci.enabledExtensionCount; ++i) {
            PPX_LOG_INFO("   " << i << " : " << vkci.ppEnabledExtensionNames[i]);
        }
    }

    VkResult vkres = vkCreateDevice(ToApi(pCreateInfo->pGpu)->GetVkGpu(), &vkci, nullptr, &mDevice);
    if (vkres != VK_SUCCESS) {
        // clang-format off
        std::stringstream ss;
        ss << "vkCreateInstance failed: " << ToString(vkres);     
        if (vkres == VK_ERROR_EXTENSION_NOT_PRESENT) {
            std::vector<std::string> missing = GetNotFound(mExtensions, mFoundExtensions);
            ss << PPX_LOG_ENDL;
            ss << "  " << "Extension(s) not found:" << PPX_LOG_ENDL;
            for (auto& elem : missing) {
                ss << "  " << "  " << elem << PPX_LOG_ENDL;
            }
        }
        // clang-format on

        PPX_ASSERT_MSG(false, ss.str());
        return ppx::ERROR_API_FAILURE;
    }

    //
    // Timeline semaphore is in core start in Vulkan 1.2
    //
    // If this is a Vulkan 1.1 device:
    //   - Enable timeline semaphore if extension was loaded
    //
    if (GetInstance()->GetApi() == grfx::API_VK_1_1) {
        mHasTimelineSemaphore = ElementExists(std::string(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME), mExtensions);
    }
    else {
        mHasTimelineSemaphore = true;
    }
    PPX_LOG_INFO("Vulkan timeline semaphore is present: " << mHasTimelineSemaphore);

#if defined(PPX_VK_EXTENDED_DYNAMIC_STATE)
    mExtendedDynamicStateAvailable = ElementExists(std::string(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME), mFoundExtensions));
#endif // defined(PPX_VK_EXTENDED_DYNAMIC_STATE)

    // Depth clip enabled
    mHasUnrestrictedDepthRange = ElementExists(std::string(VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME), mExtensions);

    // VMA
    {
        VmaAllocatorCreateInfo vmaCreateInfo = {};
        vmaCreateInfo.physicalDevice         = ToApi(pCreateInfo->pGpu)->GetVkGpu();
        vmaCreateInfo.device                 = mDevice;
        vmaCreateInfo.instance               = ToApi(GetInstance())->GetVkInstance();

        vkres = vmaCreateAllocator(&vmaCreateInfo, &mVmaAllocator);
        if (vkres != VK_SUCCESS) {
            PPX_ASSERT_MSG(false, "vmaCreateAllocator failed: " << ToString(vkres));
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Create queues
    ppxres = CreateQueues(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Device::DestroyApiObjects()
{
    if (mVmaAllocator) {
        vmaDestroyAllocator(mVmaAllocator);
        mVmaAllocator.Reset();
    }

    if (mDevice) {
        vkDestroyDevice(mDevice, nullptr);
        mDevice.Reset();
    }
}

Result Device::AllocateObject(grfx::Buffer** ppObject)
{
    vk::Buffer* pObject = new vk::Buffer();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::CommandBuffer** ppObject)
{
    vk::CommandBuffer* pObject = new vk::CommandBuffer();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::CommandPool** ppObject)
{
    vk::CommandPool* pObject = new vk::CommandPool();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::ComputePipeline** ppObject)
{
    vk::ComputePipeline* pObject = new vk::ComputePipeline();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DepthStencilView** ppObject)
{
    vk::DepthStencilView* pObject = new vk::DepthStencilView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DescriptorPool** ppObject)
{
    vk::DescriptorPool* pObject = new vk::DescriptorPool();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DescriptorSet** ppObject)
{
    vk::DescriptorSet* pObject = new vk::DescriptorSet();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DescriptorSetLayout** ppObject)
{
    vk::DescriptorSetLayout* pObject = new vk::DescriptorSetLayout();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Fence** ppObject)
{
    vk::Fence* pObject = new vk::Fence();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::GraphicsPipeline** ppObject)
{
    vk::GraphicsPipeline* pObject = new vk::GraphicsPipeline();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Image** ppObject)
{
    vk::Image* pObject = new vk::Image();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::PipelineInterface** ppObject)
{
    vk::PipelineInterface* pObject = new vk::PipelineInterface();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Queue** ppObject)
{
    vk::Queue* pObject = new vk::Queue();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::RenderPass** ppObject)
{
    vk::RenderPass* pObject = new vk::RenderPass();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::RenderTargetView** ppObject)
{
    vk::RenderTargetView* pObject = new vk::RenderTargetView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::SampledImageView** ppObject)
{
    vk::SampledImageView* pObject = new vk::SampledImageView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Sampler** ppObject)
{
    vk::Sampler* pObject = new vk::Sampler();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Semaphore** ppObject)
{
    vk::Semaphore* pObject = new vk::Semaphore();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::ShaderModule** ppObject)
{
    vk::ShaderModule* pObject = new vk::ShaderModule();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::ShaderProgram** ppObject)
{
    return ppx::ERROR_ALLOCATION_FAILED;
}

Result Device::AllocateObject(grfx::Swapchain** ppObject)
{
    vk::Swapchain* pObject = new vk::Swapchain();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::WaitIdle()
{
    VkResult vkres = vkDeviceWaitIdle(mDevice);
    if (vkres != VK_SUCCESS) {
        return ppx::ERROR_API_FAILURE;
    }
    return ppx::SUCCESS;
}

} // namespace vk
} // namespace grfx
} // namespace ppx