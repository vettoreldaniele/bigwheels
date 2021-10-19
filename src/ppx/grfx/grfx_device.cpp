#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_gpu.h"
#include "ppx/grfx/grfx_instance.h"

namespace ppx {
namespace grfx {

Result Device::Create(const grfx::DeviceCreateInfo* pCreateInfo)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo->pGpu);
    Result ppxres = grfx::InstanceObject<grfx::DeviceCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }
    PPX_LOG_INFO("Created device: " << pCreateInfo->pGpu->GetDeviceName());
    return ppx::SUCCESS;
}

void Device::Destroy()
{
    // Destroy queues first to clear any pending work
    DestroyAllObjects(mGraphicsQueues);
    DestroyAllObjects(mComputeQueues);
    DestroyAllObjects(mTransferQueues);

    // Destroy helper objects first
    DestroyAllObjects(mDrawPasses);
    DestroyAllObjects(mFullscreenQuads);
    DestroyAllObjects(mModels);
    DestroyAllObjects(mTextDraws);
    DestroyAllObjects(mTextures);
    DestroyAllObjects(mTextureFonts);

    // Destroy render passes before images and views
    DestroyAllObjects(mRenderPasses);

    DestroyAllObjects(mBuffers);
    DestroyAllObjects(mCommandBuffers);
    DestroyAllObjects(mCommandPools);
    DestroyAllObjects(mComputePipelines);
    DestroyAllObjects(mDepthStencilViews);
    DestroyAllObjects(mDescriptorSets); // Descriptor sets need to be destroyed before pools
    DestroyAllObjects(mDescriptorPools);
    DestroyAllObjects(mDescriptorSetLayouts);
    DestroyAllObjects(mFences);
    DestroyAllObjects(mImages);
    DestroyAllObjects(mGraphicsPipelines);
    DestroyAllObjects(mPipelineInterfaces);
    DestroyAllObjects(mQuerys);
    DestroyAllObjects(mRenderTargetViews);
    DestroyAllObjects(mSampledImageViews);
    DestroyAllObjects(mSamplers);
    DestroyAllObjects(mSemaphores);
    DestroyAllObjects(mStorageImageViews);
    DestroyAllObjects(mShaderModules);
    DestroyAllObjects(mSwapchains);

    grfx::InstanceObject<grfx::DeviceCreateInfo>::Destroy();
    PPX_LOG_INFO("Destroyed device: " << mCreateInfo.pGpu->GetDeviceName());
}

bool Device::isDebugEnabled() const
{
    return GetInstance()->IsDebugEnabled();
}

grfx::Api Device::GetApi() const
{
    return GetInstance()->GetApi();
}

const char* Device::GetDeviceName() const
{
    return mCreateInfo.pGpu->GetDeviceName();
}

grfx::VendorId Device::GetDeviceVendorId() const
{
    return mCreateInfo.pGpu->GetDeviceVendorId();
}

template <
    typename ObjectT,
    typename CreateInfoT,
    typename ContainerT>
Result Device::CreateObject(const CreateInfoT* pCreateInfo, ContainerT& container, ObjectT** ppObject)
{
    // Allocate object
    ObjectT* pObject = nullptr;
    Result   ppxres  = AllocateObject(&pObject);
    if (Failed(ppxres)) {
        return ppxres;
    }
    // Set parent
    pObject->SetParent(this);
    // Create internal objects
    ppxres = pObject->Create(pCreateInfo);
    if (Failed(ppxres)) {
        pObject->Destroy();
        delete pObject;
        pObject = nullptr;
        return ppxres;
    }
    // Store
    container.push_back(ObjPtr<ObjectT>(pObject));
    // Assign
    *ppObject = pObject;
    // Success
    return ppx::SUCCESS;
}

template <
    typename ObjectT,
    typename ContainerT>
void Device::DestroyObject(ContainerT& container, const ObjectT* pObject)
{
    // Make sure object is in container
    auto it = std::find_if(
        std::begin(container),
        std::end(container),
        [pObject](const ObjPtr<ObjectT>& elem) -> bool {
            bool res = (elem == pObject);
            return res; });
    if (it == std::end(container)) {
        return;
    }
    // Copy pointer
    ObjPtr<ObjectT> object = *it;
    // Remove object pointer from container
    RemoveElement(object, container);
    // Destroy internal objects
    object->Destroy();
    // Delete allocation
    ObjectT* ptr = object.Get();
    delete ptr;
}

template <typename ObjectT>
void Device::DestroyAllObjects(std::vector<ObjPtr<ObjectT>>& container)
{
    size_t n = container.size();
    for (size_t i = 0; i < n; ++i) {
        // Get object pointer
        ObjPtr<ObjectT> object = container[i];
        // Destroy internal objects
        object->Destroy();
        // Delete allocation
        ObjectT* ptr = object.Get();
        delete ptr;
    }
    // Clear container
    container.clear();
}

Result Device::AllocateObject(grfx::DrawPass** ppObject)
{
    grfx::DrawPass* pObject = new grfx::DrawPass();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::FullscreenQuad** ppObject)
{
    grfx::FullscreenQuad* pObject = new grfx::FullscreenQuad();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Model** ppObject)
{
    grfx::Model* pObject = new grfx::Model();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::TextDraw** ppObject)
{
    grfx::TextDraw* pObject = new grfx::TextDraw();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::Texture** ppObject)
{
    grfx::Texture* pObject = new grfx::Texture();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::AllocateObject(grfx::TextureFont** ppObject)
{
    grfx::TextureFont* pObject = new grfx::TextureFont();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppObject = pObject;
    return ppx::SUCCESS;
}

Result Device::CreateBuffer(const grfx::BufferCreateInfo* pCreateInfo, grfx::Buffer** ppBuffer)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppBuffer);
    Result gxres = CreateObject(pCreateInfo, mBuffers, ppBuffer);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyBuffer(const grfx::Buffer* pBuffer)
{
    PPX_ASSERT_NULL_ARG(pBuffer);
    DestroyObject(mBuffers, pBuffer);
}

Result Device::CreateCommandPool(const grfx::CommandPoolCreateInfo* pCreateInfo, grfx::CommandPool** ppCommandPool)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppCommandPool);
    Result gxres = CreateObject(pCreateInfo, mCommandPools, ppCommandPool);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyCommandPool(const grfx::CommandPool* pCommandPool)
{
    PPX_ASSERT_NULL_ARG(pCommandPool);
    DestroyObject(mCommandPools, pCommandPool);
}

Result Device::CreateComputePipeline(const grfx::ComputePipelineCreateInfo* pCreateInfo, grfx::ComputePipeline** ppComputePipeline)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppComputePipeline);
    Result gxres = CreateObject(pCreateInfo, mComputePipelines, ppComputePipeline);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyComputePipeline(const grfx::ComputePipeline* pComputePipeline)
{
    PPX_ASSERT_NULL_ARG(pComputePipeline);
    DestroyObject(mComputePipelines, pComputePipeline);
}

Result Device::CreateDepthStencilView(const grfx::DepthStencilViewCreateInfo* pCreateInfo, grfx::DepthStencilView** ppDepthStencilView)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppDepthStencilView);
    Result gxres = CreateObject(pCreateInfo, mDepthStencilViews, ppDepthStencilView);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyDepthStencilView(const grfx::DepthStencilView* pDepthStencilView)
{
    PPX_ASSERT_NULL_ARG(pDepthStencilView);
    DestroyObject(mDepthStencilViews, pDepthStencilView);
}

Result Device::CreateDescriptorPool(const grfx::DescriptorPoolCreateInfo* pCreateInfo, grfx::DescriptorPool** ppDescriptorPool)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppDescriptorPool);
    Result gxres = CreateObject(pCreateInfo, mDescriptorPools, ppDescriptorPool);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyDescriptorPool(const grfx::DescriptorPool* pDescriptorPool)
{
    PPX_ASSERT_NULL_ARG(pDescriptorPool);
    DestroyObject(mDescriptorPools, pDescriptorPool);
}

Result Device::CreateDescriptorSetLayout(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo, grfx::DescriptorSetLayout** ppDescriptorSetLayout)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppDescriptorSetLayout);
    Result gxres = CreateObject(pCreateInfo, mDescriptorSetLayouts, ppDescriptorSetLayout);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyDescriptorSetLayout(const grfx::DescriptorSetLayout* pDescriptorSetLayout)
{
    PPX_ASSERT_NULL_ARG(pDescriptorSetLayout);
    DestroyObject(mDescriptorSetLayouts, pDescriptorSetLayout);
}

Result Device::CreateDrawPass(const grfx::DrawPassCreateInfo* pCreateInfo, grfx::DrawPass** ppDrawPass)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppDrawPass);

    grfx::internal::DrawPassCreateInfo createInfo = grfx::internal::DrawPassCreateInfo(*pCreateInfo);

    Result gxres = CreateObject(&createInfo, mDrawPasses, ppDrawPass);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateDrawPass(const grfx::DrawPassCreateInfo2* pCreateInfo, grfx::DrawPass** ppDrawPass)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppDrawPass);

    grfx::internal::DrawPassCreateInfo createInfo = grfx::internal::DrawPassCreateInfo(*pCreateInfo);

    Result gxres = CreateObject(&createInfo, mDrawPasses, ppDrawPass);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateDrawPass(const grfx::DrawPassCreateInfo3* pCreateInfo, grfx::DrawPass** ppDrawPass)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppDrawPass);

    grfx::internal::DrawPassCreateInfo createInfo = grfx::internal::DrawPassCreateInfo(*pCreateInfo);

    Result gxres = CreateObject(&createInfo, mDrawPasses, ppDrawPass);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyDrawPass(const grfx::DrawPass* pDrawPass)
{
    PPX_ASSERT_NULL_ARG(pDrawPass);
    DestroyObject(mDrawPasses, pDrawPass);
}

Result Device::CreateFence(const grfx::FenceCreateInfo* pCreateInfo, grfx::Fence** ppFence)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppFence);
    Result gxres = CreateObject(pCreateInfo, mFences, ppFence);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyFence(const grfx::Fence* pFence)
{
    PPX_ASSERT_NULL_ARG(pFence);
    DestroyObject(mFences, pFence);
}

Result Device::CreateFullscreenQuad(const grfx::FullscreenQuadCreateInfo* pCreateInfo, grfx::FullscreenQuad** ppFullscreenQuad)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppFullscreenQuad);
    Result gxres = CreateObject(pCreateInfo, mFullscreenQuads, ppFullscreenQuad);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyFullscreenQuad(const grfx::FullscreenQuad* pFullscreenQuad)
{
    PPX_ASSERT_NULL_ARG(pFullscreenQuad);
    DestroyObject(mFullscreenQuads, pFullscreenQuad);
}

Result Device::CreateGraphicsPipeline(const grfx::GraphicsPipelineCreateInfo* pCreateInfo, grfx::GraphicsPipeline** ppGraphicsPipeline)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppGraphicsPipeline);
    Result gxres = CreateObject(pCreateInfo, mGraphicsPipelines, ppGraphicsPipeline);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateGraphicsPipeline(const grfx::GraphicsPipelineCreateInfo2* pCreateInfo, grfx::GraphicsPipeline** ppGraphicsPipeline)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppGraphicsPipeline);

    grfx::GraphicsPipelineCreateInfo createInfo = {};
    grfx::internal::FillOutGraphicsPipelineCreateInfo(pCreateInfo, &createInfo);

    Result gxres = CreateObject(&createInfo, mGraphicsPipelines, ppGraphicsPipeline);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyGraphicsPipeline(const grfx::GraphicsPipeline* pGraphicsPipeline)
{
    PPX_ASSERT_NULL_ARG(pGraphicsPipeline);
    DestroyObject(mGraphicsPipelines, pGraphicsPipeline);
}

Result Device::CreateImage(const grfx::ImageCreateInfo* pCreateInfo, grfx::Image** ppImage)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppImage);
    Result gxres = CreateObject(pCreateInfo, mImages, ppImage);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyImage(const grfx::Image* pImage)
{
    PPX_ASSERT_NULL_ARG(pImage);
    DestroyObject(mImages, pImage);
}

Result Device::CreateModel(const grfx::ModelCreateInfo* pCreateInfo, grfx::Model** ppModel)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppModel);
    Result gxres = CreateObject(pCreateInfo, mModels, ppModel);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyModel(const grfx::Model* pModel)
{
    PPX_ASSERT_NULL_ARG(pModel);
    DestroyObject(mModels, pModel);
}

Result Device::CreatePipelineInterface(const grfx::PipelineInterfaceCreateInfo* pCreateInfo, grfx::PipelineInterface** ppPipelineInterface)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppPipelineInterface);
    Result gxres = CreateObject(pCreateInfo, mPipelineInterfaces, ppPipelineInterface);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyPipelineInterface(const grfx::PipelineInterface* pPipelineInterface)
{
    PPX_ASSERT_NULL_ARG(pPipelineInterface);
    DestroyObject(mPipelineInterfaces, pPipelineInterface);
}

Result Device::CreateQuery(const grfx::QueryCreateInfo* pCreateInfo, grfx::Query** ppQuery)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppQuery);
    Result gxres = CreateObject(pCreateInfo, mQuerys, ppQuery);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyQuery(const grfx::Query* pQuery)
{
    PPX_ASSERT_NULL_ARG(pQuery);
    DestroyObject(mQuerys, pQuery);
}

Result Device::CreateRenderPass(const grfx::RenderPassCreateInfo* pCreateInfo, grfx::RenderPass** ppRenderPass)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppRenderPass);

    grfx::internal::RenderPassCreateInfo createInfo = grfx::internal::RenderPassCreateInfo(*pCreateInfo);

    Result gxres = CreateObject(&createInfo, mRenderPasses, ppRenderPass);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateRenderPass(const grfx::RenderPassCreateInfo2* pCreateInfo, grfx::RenderPass** ppRenderPass)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppRenderPass);

    grfx::internal::RenderPassCreateInfo createInfo = grfx::internal::RenderPassCreateInfo(*pCreateInfo);

    Result gxres = CreateObject(&createInfo, mRenderPasses, ppRenderPass);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateRenderPass(const grfx::RenderPassCreateInfo3* pCreateInfo, grfx::RenderPass** ppRenderPass)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppRenderPass);

    grfx::internal::RenderPassCreateInfo createInfo = grfx::internal::RenderPassCreateInfo(*pCreateInfo);

    Result gxres = CreateObject(&createInfo, mRenderPasses, ppRenderPass);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyRenderPass(const grfx::RenderPass* pRenderPass)
{
    PPX_ASSERT_NULL_ARG(pRenderPass);
    DestroyObject(mRenderPasses, pRenderPass);
}

Result Device::CreateRenderTargetView(const grfx::RenderTargetViewCreateInfo* pCreateInfo, grfx::RenderTargetView** ppRenderTargetView)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppRenderTargetView);
    Result gxres = CreateObject(pCreateInfo, mRenderTargetViews, ppRenderTargetView);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyRenderTargetView(const grfx::RenderTargetView* pRenderTargetView)
{
    PPX_ASSERT_NULL_ARG(pRenderTargetView);
    DestroyObject(mRenderTargetViews, pRenderTargetView);
}

Result Device::CreateSampledImageView(const grfx::SampledImageViewCreateInfo* pCreateInfo, grfx::SampledImageView** ppSampledImageView)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppSampledImageView);
    Result gxres = CreateObject(pCreateInfo, mSampledImageViews, ppSampledImageView);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroySampledImageView(const grfx::SampledImageView* pSampledImageView)
{
    PPX_ASSERT_NULL_ARG(pSampledImageView);
    DestroyObject(mSampledImageViews, pSampledImageView);
}

Result Device::CreateSampler(const grfx::SamplerCreateInfo* pCreateInfo, grfx::Sampler** ppSampler)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppSampler);
    Result gxres = CreateObject(pCreateInfo, mSamplers, ppSampler);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroySampler(const grfx::Sampler* pSampler)
{
    PPX_ASSERT_NULL_ARG(pSampler);
    DestroyObject(mSamplers, pSampler);
}

Result Device::CreateSemaphore(const grfx::SemaphoreCreateInfo* pCreateInfo, grfx::Semaphore** ppSemaphore)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppSemaphore);
    Result gxres = CreateObject(pCreateInfo, mSemaphores, ppSemaphore);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroySemaphore(const grfx::Semaphore* pSemaphore)
{
    PPX_ASSERT_NULL_ARG(pSemaphore);
    DestroyObject(mSemaphores, pSemaphore);
}

Result Device::CreateShaderModule(const grfx::ShaderModuleCreateInfo* pCreateInfo, grfx::ShaderModule** ppShaderModule)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppShaderModule);
    Result gxres = CreateObject(pCreateInfo, mShaderModules, ppShaderModule);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyShaderModule(const grfx::ShaderModule* pShaderModule)
{
    PPX_ASSERT_NULL_ARG(pShaderModule);
    DestroyObject(mShaderModules, pShaderModule);
}

Result Device::CreateStorageImageView(const grfx::StorageImageViewCreateInfo* pCreateInfo, grfx::StorageImageView** ppStorageImageView)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppStorageImageView);
    Result gxres = CreateObject(pCreateInfo, mStorageImageViews, ppStorageImageView);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyStorageImageView(const grfx::StorageImageView* pStorageImageView)
{
    PPX_ASSERT_NULL_ARG(pStorageImageView);
    DestroyObject(mStorageImageViews, pStorageImageView);
}

Result Device::CreateSwapchain(const grfx::SwapchainCreateInfo* pCreateInfo, grfx::Swapchain** ppSwapchain)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppSwapchain);
    Result gxres = CreateObject(pCreateInfo, mSwapchains, ppSwapchain);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroySwapchain(const grfx::Swapchain* pSwapchain)
{
    PPX_ASSERT_NULL_ARG(pSwapchain);
    DestroyObject(mSwapchains, pSwapchain);
}

Result Device::CreateTextDraw(const grfx::TextDrawCreateInfo* pCreateInfo, grfx::TextDraw** ppTextDraw)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppTextDraw);
    Result gxres = CreateObject(pCreateInfo, mTextDraws, ppTextDraw);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyTextDraw(const grfx::TextDraw* pTextDraw)
{
    PPX_ASSERT_NULL_ARG(pTextDraw);
    DestroyObject(mTextDraws, pTextDraw);
}

Result Device::CreateTexture(const grfx::TextureCreateInfo* pCreateInfo, grfx::Texture** ppTexture)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppTexture);
    Result gxres = CreateObject(pCreateInfo, mTextures, ppTexture);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyTexture(const grfx::Texture* pTexture)
{
    PPX_ASSERT_NULL_ARG(pTexture);
    DestroyObject(mTextures, pTexture);
}

Result Device::CreateTextureFont(const grfx::TextureFontCreateInfo* pCreateInfo, grfx::TextureFont** ppTextureFont)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppTextureFont);
    Result gxres = CreateObject(pCreateInfo, mTextureFonts, ppTextureFont);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::DestroyTextureFont(const grfx::TextureFont* pTextureFont)
{
    PPX_ASSERT_NULL_ARG(pTextureFont);
    DestroyObject(mTextureFonts, pTextureFont);
}

Result Device::AllocateCommandBuffer(
    const grfx::CommandPool* pPool,
    grfx::CommandBuffer**    ppCommandBuffer,
    uint32_t                 resourceDescriptorCount,
    uint32_t                 samplerDescriptorCount)
{
    PPX_ASSERT_NULL_ARG(ppCommandBuffer);

    grfx::internal::CommandBufferCreateInfo createInfo = {};
    createInfo.pPool                                   = pPool;
    createInfo.resourceDescriptorCount                 = resourceDescriptorCount;
    createInfo.samplerDescriptorCount                  = samplerDescriptorCount;

    Result gxres = CreateObject(&createInfo, mCommandBuffers, ppCommandBuffer);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::FreeCommandBuffer(const grfx::CommandBuffer* pCommandBuffer)
{
    PPX_ASSERT_NULL_ARG(pCommandBuffer);
    DestroyObject(mCommandBuffers, pCommandBuffer);
}

Result Device::AllocateDescriptorSet(grfx::DescriptorPool* pPool, const grfx::DescriptorSetLayout* pLayout, grfx::DescriptorSet** ppSet)
{
    PPX_ASSERT_NULL_ARG(pPool);
    PPX_ASSERT_NULL_ARG(pLayout);
    PPX_ASSERT_NULL_ARG(ppSet);

    grfx::internal::DescriptorSetCreateInfo createInfo = {};
    createInfo.pPool                                   = pPool;
    createInfo.pLayout                                 = pLayout;

    Result gxres = CreateObject(&createInfo, mDescriptorSets, ppSet);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

void Device::FreeDescriptorSet(const grfx::DescriptorSet* pSet)
{
    PPX_ASSERT_NULL_ARG(pSet);
    DestroyObject(mDescriptorSets, pSet);
}

Result Device::CreateGraphicsQueue(const grfx::internal::QueueCreateInfo* pCreateInfo, grfx::Queue** ppQueue)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppQueue);
    Result gxres = CreateObject(pCreateInfo, mGraphicsQueues, ppQueue);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateComputeQueue(const grfx::internal::QueueCreateInfo* pCreateInfo, grfx::Queue** ppQueue)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppQueue);
    Result gxres = CreateObject(pCreateInfo, mComputeQueues, ppQueue);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

Result Device::CreateTransferQueue(const grfx::internal::QueueCreateInfo* pCreateInfo, grfx::Queue** ppQueue)
{
    PPX_ASSERT_NULL_ARG(pCreateInfo);
    PPX_ASSERT_NULL_ARG(ppQueue);
    Result gxres = CreateObject(pCreateInfo, mTransferQueues, ppQueue);
    if (Failed(gxres)) {
        return gxres;
    }
    return ppx::SUCCESS;
}

uint32_t Device::GetGraphicsQueueCount() const
{
    uint32_t count = CountU32(mGraphicsQueues);
    return count;
}

Result Device::GetGraphicsQueue(uint32_t index, grfx::Queue** ppQueue) const
{
    if (!IsIndexInRange(index, mGraphicsQueues)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppQueue = mGraphicsQueues[index].Get();
    return ppx::SUCCESS;
}

grfx::QueuePtr Device::GetGraphicsQueue(uint32_t index) const
{
    grfx::QueuePtr queue;
    Result         ppxres = GetGraphicsQueue(index, &queue);
    if (Failed(ppxres)) {
        // @TODO: something?
    }
    return queue;
}

uint32_t Device::GetComputeQueueCount() const
{
    uint32_t count = CountU32(mComputeQueues);
    return count;
}

Result Device::GetComputeQueue(uint32_t index, grfx::Queue** ppQueue) const
{
    if (!IsIndexInRange(index, mComputeQueues)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppQueue = mComputeQueues[index].Get();
    return ppx::SUCCESS;
}

grfx::QueuePtr Device::GetComputeQueue(uint32_t index) const
{
    grfx::QueuePtr queue;
    Result         ppxres = GetComputeQueue(index, &queue);
    if (Failed(ppxres)) {
        // @TODO: something?
    }
    return queue;
}

uint32_t Device::GetTransferQueueCount() const
{
    uint32_t count = CountU32(mTransferQueues);
    return count;
}

Result Device::GetTransferQueue(uint32_t index, grfx::Queue** ppQueue) const
{
    if (!IsIndexInRange(index, mTransferQueues)) {
        return ppx::ERROR_OUT_OF_RANGE;
    }
    *ppQueue = mTransferQueues[index].Get();
    return ppx::SUCCESS;
}

grfx::QueuePtr Device::GetTransferQueue(uint32_t index) const
{
    grfx::QueuePtr queue;
    Result         ppxres = GetTransferQueue(index, &queue);
    if (Failed(ppxres)) {
        // @TODO: something?
    }
    return queue;
}

grfx::QueuePtr Device::GetAnyAvailableQueue() const
{
    grfx::QueuePtr queue;

    uint32_t count = GetGraphicsQueueCount();
    if (count > 0) {
        for (uint32_t i = 0; i < count; ++i) {
            queue = GetGraphicsQueue(i);
            if (queue) {
                break;
            }
        }
    }

    if (!queue) {
        count = GetComputeQueueCount();
        if (count > 0) {
            for (uint32_t i = 0; i < count; ++i) {
                queue = GetComputeQueue(i);
                if (queue) {
                    break;
                }
            }
        }
    }

    if (!queue) {
        count = GetTransferQueueCount();
        if (count > 0) {
            for (uint32_t i = 0; i < count; ++i) {
                queue = GetTransferQueue(i);
                if (queue) {
                    break;
                }
            }
        }
    }

    return queue;
}

} // namespace grfx
} // namespace ppx
