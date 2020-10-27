#include "ppx/ppx.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#if defined(USE_DX)
grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
grfx::Api kApi = grfx::API_VK_1_1;
#endif

#define kWindowWidth  1280
#define kWindowHeight 720

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

private:
    struct PerFrame
    {
        grfx::CommandBufferPtr cmd;
        grfx::SemaphorePtr     imageAcquiredSemaphore;
        grfx::FencePtr         imageAcquiredFence;
        grfx::SemaphorePtr     renderCompleteSemaphore;
        grfx::FencePtr         renderCompleteFence;
    };

    std::vector<PerFrame>        mPerFrame;
    grfx::ShaderModulePtr        mCS;
    grfx::ShaderModulePtr        mVS;
    grfx::ShaderModulePtr        mPS;
    grfx::PipelineInterfacePtr   mComputePipelineInterface;
    grfx::ComputePipelinePtr     mComputePipeline;
    grfx::PipelineInterfacePtr   mGraphicsPipelineInterface;
    grfx::GraphicsPipelinePtr    mGraphicsPipeline;
    grfx::BufferPtr              mVertexBuffer;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mComputeDescriptorSetLayout;
    grfx::DescriptorSetPtr       mComputeDescriptorSet;
    grfx::DescriptorSetLayoutPtr mGraphicsDescriptorSetLayout;
    grfx::DescriptorSetPtr       mGraphicsDescriptorSet;
    grfx::BufferPtr              mUniformBuffer;
    grfx::ImagePtr               mImage;
    grfx::SamplerPtr             mSampler;
    grfx::SampledImageViewPtr    mSampledImageView;
    grfx::StorageImageViewPtr    mStorageImageView;
    grfx::Viewport               mViewport;
    grfx::Rect                   mScissorRect;
    grfx::VertexBinding          mVertexBinding;
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "06_compute_fill";
    settings.window.width     = kWindowWidth;
    settings.window.height    = kWindowHeight;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Uniform buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mUniformBuffer));
    }

    // Texture image, view, and sampler
    {
        grfx::ImageUsageFlags additionalImageUsage = grfx::IMAGE_USAGE_STORAGE;
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("basic/textures/box_panel.jpg"), &mImage, additionalImageUsage));

        grfx::SampledImageViewCreateInfo sampledViewCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampledImageView(&sampledViewCreateInfo, &mSampledImageView));

        grfx::StorageImageViewCreateInfo storageViewCreateInfo = grfx::StorageImageViewCreateInfo::GuessFromImage(mImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateStorageImageView(&storageViewCreateInfo, &mStorageImageView));

        grfx::SamplerCreateInfo samplerCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&samplerCreateInfo, &mSampler));
    }

    // Descriptors
    {
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = 1;
        poolCreateInfo.sampledImage                   = 1;
        poolCreateInfo.sampler                        = 1;
        poolCreateInfo.storageImage                   = 1;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

        // Compute
        {
            grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE));
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mComputeDescriptorSetLayout));

            PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mComputeDescriptorSetLayout, &mComputeDescriptorSet));

            grfx::WriteDescriptor write = {};
            write.binding               = 0;
            write.type                  = grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE;
            write.pImageView            = mStorageImageView;
            PPX_CHECKED_CALL(ppxres = mComputeDescriptorSet->UpdateDescriptors(1, &write));
        }

        // Graphics
        {
            grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER));
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(2, grfx::DESCRIPTOR_TYPE_SAMPLER));
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mGraphicsDescriptorSetLayout));

            PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mGraphicsDescriptorSetLayout, &mGraphicsDescriptorSet));

            grfx::WriteDescriptor write = {};
            write.binding               = 0;
            write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.bufferOffset          = 0;
            write.bufferRange           = PPX_WHOLE_SIZE;
            write.pBuffer               = mUniformBuffer;
            PPX_CHECKED_CALL(ppxres = mGraphicsDescriptorSet->UpdateDescriptors(1, &write));

            write            = {};
            write.binding    = 1;
            write.type       = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write.pImageView = mSampledImageView;
            PPX_CHECKED_CALL(ppxres = mGraphicsDescriptorSet->UpdateDescriptors(1, &write));

            write          = {};
            write.binding  = 2;
            write.type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
            write.pSampler = mSampler;
            PPX_CHECKED_CALL(ppxres = mGraphicsDescriptorSet->UpdateDescriptors(1, &write));
        }
    }

    // Compute pipeline
    {
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "ComputeFill.cs");
        PPX_ASSERT_MSG(!bytecode.empty(), "CS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mCS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mComputeDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mComputePipelineInterface));

        grfx::ComputePipelineCreateInfo cpCreateInfo = {};
        cpCreateInfo.CS                              = {mCS.Get(), "csmain"};
        cpCreateInfo.pPipelineInterface              = mComputePipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateComputePipeline(&cpCreateInfo, &mComputePipeline));
    }

    // Graphics pipeline
    {
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "Texture.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

        bytecode = LoadShader(GetAssetPath("basic/shaders"), "Texture.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mGraphicsDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mGraphicsPipelineInterface));

        mVertexBinding.AppendAttribute({"POSITION", 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});
        mVertexBinding.AppendAttribute({"TEXCOORD", 1, grfx::FORMAT_R32G32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {mVS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {mPS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = 1;
        gpCreateInfo.vertexInputState.bindings[0]       = mVertexBinding;
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = false;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.pPipelineInterface                 = mGraphicsPipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mGraphicsPipeline));
    }

    // Per frame data
    {
        PerFrame frame = {};

        PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd));

        grfx::SemaphoreCreateInfo semaCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore));

        grfx::FenceCreateInfo fenceCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence));

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore));

        fenceCreateInfo = {true}; // Create signaled
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence));

        mPerFrame.push_back(frame);
    }

    // Vertex buffer and geometry data
    {
        // clang-format off
        std::vector<float> vertexData = {
            // position           // tex coords
            -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
             0.5f, -0.5f, 0.0f,   1.0f, 1.0f,

            -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
        };
        // clang-format on
        uint32_t dataSize = ppx::SizeInBytesU32(vertexData);

        grfx::BufferCreateInfo bufferCreateInfo       = {};
        bufferCreateInfo.size                         = dataSize;
        bufferCreateInfo.usageFlags.bits.vertexBuffer = true;
        bufferCreateInfo.memoryUsage                  = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mVertexBuffer));

        void* pAddr = nullptr;
        PPX_CHECKED_CALL(ppxres = ppxres = mVertexBuffer->MapMemory(0, &pAddr));
        memcpy(pAddr, vertexData.data(), dataSize);
        mVertexBuffer->UnmapMemory();
    }

    // Viewport and scissor rect
    mViewport    = {0, 0, kWindowWidth, kWindowHeight, 0, 1};
    mScissorRect = {0, 0, kWindowWidth, kWindowHeight};
}

void ProjApp::Render()
{
    Result    ppxres = ppx::SUCCESS;
    PerFrame& frame  = mPerFrame[0];

    grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(ppxres = swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(ppxres = frame.imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(ppxres = frame.renderCompleteFence->WaitAndReset());

    // Update uniform buffer
    {
        float    t   = GetElapsedSeconds();
        float4x4 mat = glm::rotate(t, float3(0, 0, 1));

        void* pData = nullptr;
        PPX_CHECKED_CALL(ppxres = mUniformBuffer->MapMemory(0, &pData));
        memcpy(pData, &mat, sizeof(mat));
        mUniformBuffer->UnmapMemory();
    }

    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{0, 0, 0, 0}};

        // Fill image with red
        frame.cmd->TransitionImageLayout(mImage, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_SHADER_RESOURCE, grfx::RESOURCE_STATE_UNORDERED_ACCESS);
        frame.cmd->BindComputeDescriptorSets(mComputePipelineInterface, 1, &mComputeDescriptorSet);
        frame.cmd->BindComputePipeline(mComputePipeline);
        frame.cmd->Dispatch(mImage->GetWidth(), mImage->GetHeight(), 1);
        frame.cmd->TransitionImageLayout(mImage, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_UNORDERED_ACCESS, grfx::RESOURCE_STATE_SHADER_RESOURCE);

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            // Draw texture
            frame.cmd->SetScissors(1, &mScissorRect);
            frame.cmd->SetViewports(1, &mViewport);
            frame.cmd->BindGraphicsDescriptorSets(mGraphicsPipelineInterface, 1, &mGraphicsDescriptorSet);
            frame.cmd->BindGraphicsPipeline(mGraphicsPipeline);
            frame.cmd->BindVertexBuffers(1, &mVertexBuffer, &mVertexBinding.GetStride());
            frame.cmd->Draw(6, 1, 0, 0);

            // Draw ImGui
            DrawDebugInfo();
            DrawImGui(frame.cmd);
        }
        frame.cmd->EndRenderPass();
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_RENDER_TARGET, grfx::RESOURCE_STATE_PRESENT);
    }
    PPX_CHECKED_CALL(ppxres = frame.cmd->End());

    grfx::SubmitInfo submitInfo     = {};
    submitInfo.commandBufferCount   = 1;
    submitInfo.ppCommandBuffers     = &frame.cmd;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.ppWaitSemaphores     = &frame.imageAcquiredSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.ppSignalSemaphores   = &frame.renderCompleteSemaphore;
    submitInfo.pFence               = frame.renderCompleteFence;

    PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->Submit(&submitInfo));

    PPX_CHECKED_CALL(ppxres = swapchain->Present(imageIndex, 1, &frame.renderCompleteSemaphore));
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}