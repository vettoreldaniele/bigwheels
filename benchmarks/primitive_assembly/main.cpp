#include "ppx/ppx.h"

#if defined(PORTO_D3DCOMPILE)
#include "ppx/grfx/dx/d3dcompile_util.h"
#endif

using namespace ppx;

#if defined(USE_DX11)
const grfx::Api kApi = grfx::API_DX_11_1;
#elif defined(USE_DX12)
const grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
const grfx::Api kApi = grfx::API_VK_1_1;
#endif

#if !defined(PPX_DXIIVK) && !defined(USE_VK)
#define ENABLE_PIPELINE_QUERIES
#endif

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
        ppx::grfx::CommandBufferPtr cmd;
        ppx::grfx::SemaphorePtr     imageAcquiredSemaphore;
        ppx::grfx::FencePtr         imageAcquiredFence;
        ppx::grfx::SemaphorePtr     renderCompleteSemaphore;
        ppx::grfx::FencePtr         renderCompleteFence;
        ppx::grfx::QueryPtr         timestampQuery;
#if defined(ENABLE_PIPELINE_QUERIES)
        ppx::grfx::QueryPtr         pipelineStatsQuery;
#endif // defined(ENABLE_PIPELINE_QUERIES)
    };

    std::vector<PerFrame>           mPerFrame;
    grfx::DescriptorPoolPtr         mDescriptorPool;
    ppx::grfx::ShaderModulePtr      mVS;
    ppx::grfx::ShaderModulePtr      mPS;
    ppx::grfx::PipelineInterfacePtr mPipelineInterface;
    ppx::grfx::GraphicsPipelinePtr  mPipeline;
    ppx::grfx::BufferPtr            mVertexBuffer;
    grfx::DrawPassPtr               mDrawPass;
    grfx::Viewport                  mViewport;
    grfx::Rect                      mScissorRect;
    grfx::VertexBinding             mVertexBinding;
    uint2                           mRenderTargetSize;
    uint32_t                        mNumTriangles;
    uint64_t                        mGpuWorkDuration = 0;
    grfx::PipelineStatistics        mPipelineStatistics = {};
    // For drawing into the swapchain
    grfx::DescriptorSetLayoutPtr mDrawToSwapchainLayout;
    grfx::DescriptorSetPtr       mDrawToSwapchainSet;
    grfx::FullscreenQuadPtr      mDrawToSwapchain;
    grfx::SamplerPtr             mSampler;

    void SetupDrawToSwapchain();
    void SetupTestParameters();
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "primitive_assembly";
    settings.enableImGui      = false;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
#if defined(USE_DXVK_SPV)
    settings.grfx.enableDXVKSPV = true;
#endif
}

void ProjApp::SetupDrawToSwapchain()
{
    Result ppxres = ppx::SUCCESS;

    // Descriptor set layout
    {
        // Descriptor set layout
        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLER));
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDrawToSwapchainLayout));
    }

    // Pipeline
    {
        grfx::ShaderModulePtr VS;
#if defined(PORTO_D3DCOMPILE)
        grfx::dx::ShaderIncludeHandler basicShaderIncludeHandler(
            GetAssetPath("basic/shaders"));
        std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "FullScreenTriangle", "vs_5_0", &basicShaderIncludeHandler);
#else
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "FullScreenTriangle.vs");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        grfx::ShaderModulePtr PS;
#if defined(PORTO_D3DCOMPILE)
        bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "FullScreenTriangle", "ps_5_0", &basicShaderIncludeHandler);
#else
        bytecode                   = LoadShader(GetAssetPath("basic/shaders"), "FullScreenTriangle.ps");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        grfx::FullscreenQuadCreateInfo createInfo = {};
        createInfo.VS                             = VS;
        createInfo.PS                             = PS;
        createInfo.setCount                       = 1;
        createInfo.sets[0].set                    = 0;
        createInfo.sets[0].pLayout                = mDrawToSwapchainLayout;
        createInfo.renderTargetCount              = 1;
        createInfo.renderTargetFormats[0]         = GetSwapchain()->GetColorFormat();
        createInfo.depthStencilFormat             = GetSwapchain()->GetDepthFormat();

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFullscreenQuad(&createInfo, &mDrawToSwapchain));
    }

    // Allocate descriptor set
    PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mDrawToSwapchainLayout, &mDrawToSwapchainSet));

    // Write descriptors
    {
        grfx::WriteDescriptor writes[2] = {};
        writes[0].binding               = 0;
        writes[0].arrayIndex            = 0;
        writes[0].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[0].pImageView            = mDrawPass->GetRenderTargetTexture(0)->GetSampledImageView();

        writes[1].binding  = 1;
        writes[1].type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
        writes[1].pSampler = mSampler;

        PPX_CHECKED_CALL(ppxres = mDrawToSwapchainSet->UpdateDescriptors(2, writes));
    }
}

void ProjApp::SetupTestParameters()
{
    // Set render target size
    mRenderTargetSize                             = uint2(1, 1);
    std::map<std::string, std::string> cl_options = GetExtraOptions();
    // Number of triangles to draw
    mNumTriangles = 1000000;
    if (cl_options.count("triangles") > 0) {
        try
        {
            mNumTriangles = static_cast<uint32_t>(std::stoul(cl_options.at("triangles")));
        }
        catch (std::exception&)
        {
            PPX_LOG_WARN("Invalid value for number of triangles, default to: 1,000,0000");
        }
    }   
    // Name of the CSV output file
    if (cl_options.count("stats-file") > 0) {
        const std::string file = cl_options.at("stats-file");
        if (!file.empty())
        {
            GetCSVLogger().Restart(file);
        }
        else
        {
            PPX_LOG_WARN("Invalid name for CSV log file, default to: stats.csv");
        }
    }
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;
    
    SetupTestParameters();

    // Create descriptor pool
    {
        grfx::DescriptorPoolCreateInfo createInfo = {};
        createInfo.sampler                        = 1;
        createInfo.sampledImage                   = 1;
        createInfo.uniformBuffer                  = 0;
        createInfo.structuredBuffer               = 0;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&createInfo, &mDescriptorPool));
    }

    // Sampler
    {
        grfx::SamplerCreateInfo createInfo = {};
        createInfo.magFilter               = grfx::FILTER_NEAREST;
        createInfo.minFilter               = grfx::FILTER_NEAREST;
        createInfo.mipmapMode              = grfx::SAMPLER_MIPMAP_MODE_NEAREST;
        createInfo.minLod                  = 0.0f;
        createInfo.maxLod                  = FLT_MAX;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&createInfo, &mSampler));
    }

    // Draw pass
    {
        // Usage flags for render target and depth stencil will automatically
        // be added during create. So we only need to specify the additional
        // usage flags here.
        //
        grfx::ImageUsageFlags        additionalUsageFlags = grfx::IMAGE_USAGE_SAMPLED;
        
        grfx::DrawPassCreateInfo createInfo     = {};
        createInfo.width                        = mRenderTargetSize.x;
        createInfo.height                       = mRenderTargetSize.y;
        createInfo.renderTargetCount            = 1;
        createInfo.renderTargetFormats[0]       = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.depthStencilFormat           = grfx::FORMAT_D32_FLOAT;
        createInfo.renderTargetUsageFlags[0]    = additionalUsageFlags;
        createInfo.depthStencilUsageFlags       = additionalUsageFlags;
        createInfo.renderTargetInitialStates[0] = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.depthStencilInitialState     = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.renderTargetClearValues[0]   = {0, 0, 0, 0};
        createInfo.depthStencilClearValue       = {1.0f, 0xFF};

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDrawPass(&createInfo, &mDrawPass));
    }
    // Pipeline
    {
#if defined(PORTO_D3DCOMPILE)
        grfx::dx::ShaderIncludeHandler basicShaderIncludeHandler(
            GetAssetPath("basic/shaders"));
        std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "PassThroughPos", "vs_5_0", &basicShaderIncludeHandler);
#else
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "PassThroughPos.vs");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

#if defined(PORTO_D3DCOMPILE)
        bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "PassThroughPos", "ps_5_0", &basicShaderIncludeHandler);
#else
        bytecode = LoadShader(GetAssetPath("basic/shaders"), "PassThroughPos.ps");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 0;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = nullptr;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mPipelineInterface));

        mVertexBinding.AppendAttribute({"POSITION", 0, grfx::FORMAT_R32G32B32A32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {mVS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {mPS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = 1;
        gpCreateInfo.vertexInputState.bindings[0]       = mVertexBinding;
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthReadEnable                    = false;
        gpCreateInfo.depthWriteEnable                   = false;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = mDrawPass->GetRenderTargetTexture(0)->GeImageFormat();
        gpCreateInfo.outputState.depthStencilFormat     = mDrawPass->GetDepthStencilTexture()->GeImageFormat();
        gpCreateInfo.pPipelineInterface                 = mPipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mPipeline));
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

        // Create the timestam queries
        grfx::QueryCreateInfo queryCreateInfo = {};
        queryCreateInfo.type                  = grfx::QUERY_TYPE_TIMESTAMP;
        queryCreateInfo.count                 = 2;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateQuery(&queryCreateInfo, &frame.timestampQuery));
#if defined(ENABLE_PIPELINE_QUERIES)
        // Pipeline statistics query pool
        queryCreateInfo       = {};
        queryCreateInfo.type  = grfx::QUERY_TYPE_PIPELINE_STATISTICS;
        queryCreateInfo.count = 1;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateQuery(&queryCreateInfo, &frame.pipelineStatsQuery));
#endif // defined(ENABLE_PIPELINE_QUERIES)
        mPerFrame.push_back(frame);
    }

    // Buffer and geometry data
    {
        // clang-format off
        std::vector<float> vertexData = {
            // position           
             0.0f,  0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f,
        };
        // clang-format on
        uint32_t dataSize = ppx::SizeInBytesU32(vertexData);

        grfx::BufferCreateInfo bufferCreateInfo       = {};
        bufferCreateInfo.size                         = dataSize;
        bufferCreateInfo.usageFlags.bits.vertexBuffer = true;
        bufferCreateInfo.memoryUsage                  = grfx::MEMORY_USAGE_CPU_TO_GPU;
        bufferCreateInfo.initialState                 = grfx::RESOURCE_STATE_VERTEX_BUFFER;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mVertexBuffer));

        void* pAddr = nullptr;
        PPX_CHECKED_CALL(ppxres = mVertexBuffer->MapMemory(0, &pAddr));
        memcpy(pAddr, vertexData.data(), dataSize);
        mVertexBuffer->UnmapMemory();
    }

    mViewport    = {0, 0, float(mRenderTargetSize.x), float(mRenderTargetSize.y), 0, 1};
    mScissorRect = {0, 0, mRenderTargetSize.x, mRenderTargetSize.y};

    // Setup fullscreen quad to draw to swapchain
    SetupDrawToSwapchain();
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

    // Read query results
    if (GetFrameCount() > 0) {
        uint64_t data[2] = {0};
        PPX_CHECKED_CALL(ppxres = frame.timestampQuery->GetData(data, 2 * sizeof(uint64_t)));
        mGpuWorkDuration = data[1] - data[0];
#if defined(ENABLE_PIPELINE_QUERIES)
        PPX_CHECKED_CALL(ppxres = frame.pipelineStatsQuery->GetData(&mPipelineStatistics, sizeof(grfx::PipelineStatistics)));
#endif // defined(ENABLE_PIPELINE_QUERIES)
    }
    // Reset queries
    frame.timestampQuery->Reset(0, 2);
#if defined(ENABLE_PIPELINE_QUERIES)
    frame.pipelineStatsQuery->Reset(0, 1);
#endif // defined(ENABLE_PIPELINE_QUERIES)
    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        // Transition texture to render target
        frame.cmd->TransitionImageLayout(
            mDrawPass,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_RENDER_TARGET,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE);
        // Write start timestamp
        frame.cmd->WriteTimestamp(frame.timestampQuery, grfx::PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0);
        // Render pass to texture
        frame.cmd->BeginRenderPass(mDrawPass, grfx::DRAW_PASS_CLEAR_FLAG_CLEAR_RENDER_TARGETS);
        {
            frame.cmd->SetScissors(1, &mScissorRect);
            frame.cmd->SetViewports(1, &mViewport);
            frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 0, nullptr);
            frame.cmd->BindGraphicsPipeline(mPipeline);
            frame.cmd->BindVertexBuffers(1, &mVertexBuffer, &mVertexBinding.GetStride());
#if defined(ENABLE_PIPELINE_QUERIES)
            frame.cmd->BeginQuery(frame.pipelineStatsQuery, 0);
#endif
            frame.cmd->Draw(3, mNumTriangles, 0, 0);
#if defined(ENABLE_PIPELINE_QUERIES)
            frame.cmd->EndQuery(frame.pipelineStatsQuery, 0);
#endif
        }
        frame.cmd->EndRenderPass();
        // Write end timestamp
        frame.cmd->WriteTimestamp(frame.timestampQuery, grfx::PIPELINE_STAGE_TOP_OF_PIPE_BIT, 1);
        // Resolve queries
        frame.cmd->ResolveQueryData(frame.timestampQuery, 0, 2);
#if defined(ENABLE_PIPELINE_QUERIES)
        frame.cmd->ResolveQueryData(frame.pipelineStatsQuery, 0, 1);
#endif
        // Transition texture to Shader resource
        frame.cmd->TransitionImageLayout(
            mDrawPass,
            grfx::RESOURCE_STATE_RENDER_TARGET,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE,
            grfx::RESOURCE_STATE_SHADER_RESOURCE);
        // Blit the texture into the swapchain
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        frame.cmd->SetScissors(renderPass->GetScissor());
        frame.cmd->SetViewports(renderPass->GetViewport());

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(renderPass);
        {
            // Draw render target output to swapchain
            frame.cmd->Draw(mDrawToSwapchain, 1, &mDrawToSwapchainSet);
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
    // Register the gpu timestamp query
    uint64_t frequency = 0;
    GetGraphicsQueue()->GetTimestampFrequency(&frequency);
    const float gpuWorkDuration = static_cast<float>(mGpuWorkDuration / static_cast<double>(frequency)) * 1000.0f;
    GetCSVLogger().Lock();
    GetCSVLogger().LogField(gpuWorkDuration);
#if defined(ENABLE_PIPELINE_QUERIES)
    GetCSVLogger().LogField(mPipelineStatistics.IAVertices);
    GetCSVLogger().LogField(mPipelineStatistics.IAPrimitives);
    GetCSVLogger().LogField(mPipelineStatistics.VSInvocations);
    GetCSVLogger().LogField(mPipelineStatistics.CPrimitives);
    GetCSVLogger().LogField(mPipelineStatistics.PSInvocations);
#endif
    GetCSVLogger().Flush();
    GetCSVLogger().Unlock();
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
