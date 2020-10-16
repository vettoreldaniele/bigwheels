#include "ppx/grfx/dx/dx_device.h"
#include "ppx/grfx/dx/dx_buffer.h"
#include "ppx/grfx/dx/dx_command.h"
#include "ppx/grfx/dx/dx_descriptor.h"
#include "ppx/grfx/dx/dx_gpu.h"
#include "ppx/grfx/dx/dx_image.h"
#include "ppx/grfx/dx/dx_instance.h"
#include "ppx/grfx/dx/dx_pipeline.h"
#include "ppx/grfx/dx/dx_queue.h"
#include "ppx/grfx/dx/dx_render_pass.h"
#include "ppx/grfx/dx/dx_shader.h"
#include "ppx/grfx/dx/dx_swapchain.h"
#include "ppx/grfx/dx/dx_sync.h"

namespace ppx {
namespace grfx {
namespace dx {

void Device::LoadRootSignatureFunctions()
{
    // Load root signature version 1.1 functions
    {
        HMODULE module = ::GetModuleHandle(TEXT("d3d12.dll"));

        mFnD3D12CreateRootSignatureDeserializer = (PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER)GetProcAddress(
            module,
            "D3D12SerializeVersionedRootSignature");

        mFnD3D12SerializeVersionedRootSignature = (PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE)GetProcAddress(
            module,
            "D3D12SerializeVersionedRootSignature");

        mFnD3D12CreateVersionedRootSignatureDeserializer = (PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER)GetProcAddress(
            module,
            "D3D12CreateVersionedRootSignatureDeserializer");
    }
}

Result Device::CreateQueues(const grfx::DeviceCreateInfo* pCreateInfo)
{
    // Graphics
    for (uint32_t queueIndex = 0; queueIndex < pCreateInfo->graphicsQueueCount; ++queueIndex) {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask                 = 0;

        D3D12CommandQueuePtr queue;
        HRESULT              hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateCommandQueue(compute) failed");
            return ppx::ERROR_API_FAILURE;
        }

        grfx::internal::QueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.commandType                     = grfx::COMMAND_TYPE_GRAPHICS;
        queueCreateInfo.pApiObject                      = queue.Get();

        grfx::QueuePtr tmpQueue;
        Result         ppxres = CreateGraphicsQueue(&queueCreateInfo, &tmpQueue);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Compute
    for (uint32_t queueIndex = 0; queueIndex < pCreateInfo->computeQueueCount; ++queueIndex) {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type                     = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask                 = 0;

        D3D12CommandQueuePtr queue;
        HRESULT              hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateCommandQueue(transfer) failed");
            return ppx::ERROR_API_FAILURE;
        }

        grfx::internal::QueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.commandType                     = grfx::COMMAND_TYPE_COMPUTE;
        queueCreateInfo.pApiObject                      = queue.Get();

        grfx::QueuePtr tmpQueue;
        Result         ppxres = CreateComputeQueue(&queueCreateInfo, &tmpQueue);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Transfer
    for (uint32_t queueIndex = 0; queueIndex < pCreateInfo->transferQueueCount; ++queueIndex) {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type                     = D3D12_COMMAND_LIST_TYPE_COPY;
        desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask                 = 0;

        D3D12CommandQueuePtr queue;
        HRESULT              hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "ID3D12Device::CreateCommandQueue(copy) failed");
            return ppx::ERROR_API_FAILURE;
        }

        grfx::internal::QueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.commandType                     = grfx::COMMAND_TYPE_TRANSFER;
        queueCreateInfo.pApiObject                      = queue.Get();

        grfx::QueuePtr tmpQueue;
        Result         ppxres = CreateTransferQueue(&queueCreateInfo, &tmpQueue);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

Result Device::CreateApiObjects(const grfx::DeviceCreateInfo* pCreateInfo)
{
    // Feature level
    D3D_FEATURE_LEVEL featureLevel = ToApi(pCreateInfo->pGpu)->GetFeatureLevel();

    // Cast to XIDXGIAdapter
    typename DXGIAdapterPtr::InterfaceType* pAdapter = ToApi(pCreateInfo->pGpu)->GetDxAdapter();

    //
    // Enable SM 6.0 for DXIL support
    //
    // NOTE: This requires Windows 10 to be in Developer Mode.
    //
    {
        // Create a temp device so we can query it for SM 6.0 support
        D3D12DevicePtr device;
        {
            HRESULT hr = D3D12CreateDevice(pAdapter, featureLevel, IID_PPV_ARGS(&device));
            if (FAILED(hr)) {
                return ppx::ERROR_API_FAILURE;
            }
        }

        D3D12_FEATURE_DATA_SHADER_MODEL featureData = {D3D_SHADER_MODEL_6_0};

        // Check for SM 6.0
        HRESULT hr = device->CheckFeatureSupport(
            D3D12_FEATURE_SHADER_MODEL,
            &featureData,
            sizeof(featureData));
        if (FAILED(hr)) {
            return ppx::ERROR_API_FAILURE;
        }

        // Bail if SM 6.0 isn't supported
        if (featureData.HighestShaderModel != D3D_SHADER_MODEL_6_0) {
            return ppx::ERROR_REQUIRED_FEATURE_UNAVAILABLE;
        }

        // Destroy temp device;
        device.Reset();

        // clang-format off
        UUID experimentalFeatures[] = {
            D3D12ExperimentalShaderModels
        };
        // clang-format on

        hr = D3D12EnableExperimentalFeatures(1, experimentalFeatures, nullptr, nullptr);
        if (FAILED(hr)) {
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Create real D3D12 device
    HRESULT hr = D3D12CreateDevice(pAdapter, featureLevel, IID_PPV_ARGS(&mDevice));
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(D3D12Device, mDevice.Get());

    // Handle increment sizes
    mHandleIncrementSizeCBVSRVUAV = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mHandleIncrementSizeSampler   = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    // Check for Root signature version 1.1
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {D3D_ROOT_SIGNATURE_VERSION_1_1};

        hr = mDevice->CheckFeatureSupport(
            D3D12_FEATURE_ROOT_SIGNATURE,
            &featureData,
            sizeof(featureData));

        if (FAILED(hr)) {
            return ppx::ERROR_REQUIRED_FEATURE_UNAVAILABLE;
        }
    }

    // Create D3D12MA allocator
    {
        D3D12MA::ALLOCATOR_FLAGS flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        if (GetInstance()->ForceDxDiscreteAllocations()) {
            flags |= D3D12MA::ALLOCATOR_FLAG_ALWAYS_COMMITTED;
        }

        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.Flags                   = flags;
        allocatorDesc.pDevice                 = mDevice.Get();
        allocatorDesc.pAdapter                = pAdapter;

        HRESULT hr = D3D12MA::CreateAllocator(&allocatorDesc, &mAllocator);
        if (FAILED(hr)) {
            return ppx::ERROR_API_FAILURE;
        }
    }

    // Descriptor handle managers
    {
        // RTV
        Result ppxres = mRTVHandleManager.Create(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        if (Failed(ppxres)) {
            return ppxres;
        }

        // DSV
        ppxres = mDSVHandleManager.Create(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Load root signature functions
    LoadRootSignatureFunctions();

    // Create queues
    Result ppxres = CreateQueues(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Success
    return ppx::SUCCESS;
}

void Device::DestroyApiObjects()
{
    mFnD3D12CreateRootSignatureDeserializer          = nullptr;
    mFnD3D12SerializeVersionedRootSignature          = nullptr;
    mFnD3D12CreateVersionedRootSignatureDeserializer = nullptr;

    mRTVHandleManager.Destroy();
    mDSVHandleManager.Destroy();

    if (mAllocator) {
        mAllocator->Release();
        mAllocator.Reset();
    }

    mDevice.Reset();
}

Result Device::AllocateRTVHandle(dx::DescriptorHandle* pHandle)
{
    Result ppxres = mRTVHandleManager.AllocateHandle(pHandle);
    if (Failed(ppxres)) {
        return ppxres;
    }
    return ppx::SUCCESS;
}

void Device::FreeRTVHandle(const dx::DescriptorHandle* pHandle)
{
    if (IsNull(pHandle)) {
        return;
    }
    mRTVHandleManager.FreeHandle(*pHandle);
}

Result Device::AllocateDSVHandle(dx::DescriptorHandle* pHandle)
{
    Result ppxres = mDSVHandleManager.AllocateHandle(pHandle);
    if (Failed(ppxres)) {
        return ppxres;
    }
    return ppx::SUCCESS;
}

void Device::FreeDSVHandle(const dx::DescriptorHandle* pHandle)
{
    if (IsNull(pHandle)) {
        return;
    }
    mDSVHandleManager.FreeHandle(*pHandle);
}

HRESULT Device::CreateRootSignatureDeserializer(
    LPCVOID    pSrcData,
    SIZE_T     SrcDataSizeInBytes,
    const IID& pRootSignatureDeserializerInterface,
    void**     ppRootSignatureDeserializer)
{
    if (mFnD3D12CreateRootSignatureDeserializer == nullptr) {
        return E_NOTIMPL;
    }

    HRESULT hr = mFnD3D12CreateRootSignatureDeserializer(
        pSrcData,
        SrcDataSizeInBytes,
        pRootSignatureDeserializerInterface,
        ppRootSignatureDeserializer);
    return hr;
}

HRESULT Device::SerializeVersionedRootSignature(
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* pRootSignature,
    ID3DBlob**                                 ppBlob,
    ID3DBlob**                                 ppErrorBlob)
{
    if (mFnD3D12CreateRootSignatureDeserializer == nullptr) {
        return E_NOTIMPL;
    }

    HRESULT hr = mFnD3D12SerializeVersionedRootSignature(
        pRootSignature,
        ppBlob,
        ppErrorBlob);
    return hr;
}

HRESULT Device::CreateVersionedRootSignatureDeserializer(
    LPCVOID    pSrcData,
    SIZE_T     SrcDataSizeInBytes,
    const IID& pRootSignatureDeserializerInterface,
    void**     ppRootSignatureDeserializer)
{
    if (mFnD3D12CreateRootSignatureDeserializer == nullptr) {
        return E_NOTIMPL;
    }

    HRESULT hr = mFnD3D12CreateRootSignatureDeserializer(
        pSrcData,
        SrcDataSizeInBytes,
        pRootSignatureDeserializerInterface,
        ppRootSignatureDeserializer);
    return hr;
}

Result Device::AllocateObject(grfx::Buffer** ppObject)
{
    dx::Buffer* pObject = new dx::Buffer();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::CommandBuffer** ppObject)
{
    dx::CommandBuffer* pObject = new dx::CommandBuffer();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::CommandPool** ppObject)
{
    dx::CommandPool* pObject = new dx::CommandPool();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::ComputePipeline** ppObject)
{
    dx::ComputePipeline* pObject = new dx::ComputePipeline();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DepthStencilView** ppObject)
{
    dx::DepthStencilView* pObject = new dx::DepthStencilView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DescriptorPool** ppObject)
{
    dx::DescriptorPool* pObject = new dx::DescriptorPool();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DescriptorSet** ppObject)
{
    dx::DescriptorSet* pObject = new dx::DescriptorSet();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::DescriptorSetLayout** ppObject)
{
    dx::DescriptorSetLayout* pObject = new dx::DescriptorSetLayout();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Fence** ppObject)
{
    dx::Fence* pObject = new dx::Fence();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::GraphicsPipeline** ppObject)
{
    dx::GraphicsPipeline* pObject = new dx::GraphicsPipeline();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Image** ppObject)
{
    dx::Image* pObject = new dx::Image();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::PipelineInterface** ppObject)
{
    dx::PipelineInterface* pObject = new dx::PipelineInterface();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Queue** ppObject)
{
    dx::Queue* pObject = new dx::Queue();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::RenderPass** ppObject)
{
    dx::RenderPass* pObject = new dx::RenderPass();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::RenderTargetView** ppObject)
{
    dx::RenderTargetView* pObject = new dx::RenderTargetView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::SampledImageView** ppObject)
{
    dx::SampledImageView* pObject = new dx::SampledImageView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Sampler** ppObject)
{
    dx::Sampler* pObject = new dx::Sampler();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Semaphore** ppObject)
{
    dx::Semaphore* pObject = new dx::Semaphore();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::ShaderModule** ppObject)
{
    dx::ShaderModule* pObject = new dx::ShaderModule();
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

Result Device::AllocateObject(grfx::StorageImageView** ppObject)
{
    dx::StorageImageView* pObject = new dx::StorageImageView();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Swapchain** ppObject)
{
    dx::Swapchain* pObject = new dx::Swapchain();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::WaitIdle()
{
    for (auto& queue : mGraphicsQueues) {
        Result ppxres = ToApi(queue)->WaitIdle();
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    for (auto& queue : mComputeQueues) {
        Result ppxres = ToApi(queue)->WaitIdle();
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    for (auto& queue : mTransferQueues) {
        Result ppxres = ToApi(queue)->WaitIdle();
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

} // namespace dx
} // namespace grfx
} // namespace ppx
