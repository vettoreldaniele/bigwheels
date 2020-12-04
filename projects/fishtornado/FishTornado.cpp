#include "FishTornado.h"
#include "ppx/graphics_util.h"

#define kWindowWidth        1920
#define kWindowHeight       1080
#define kShadowRes          1024
#define kCausticsImageCount 32

static const float3 kFogColor   = float3(15.0f, 86.0f, 107.0f) / 255.0f;
static const float3 kFloorColor = float3(145.0f, 189.0f, 155.0f) / 255.0f;

FishTornadoApp* FishTornadoApp::GetThisApp()
{
    FishTornadoApp* pApp = static_cast<FishTornadoApp*>(Application::Get());
    return pApp;
}

grfx::DescriptorSetPtr FishTornadoApp::GetSceneSet(uint32_t frameIndex) const
{
    return mPerFrame[frameIndex].sceneSet;
}

grfx::DescriptorSetPtr FishTornadoApp::GetSceneShadowSet(uint32_t frameIndex) const
{
    return mPerFrame[frameIndex].sceneShadowSet;
}

grfx::TexturePtr FishTornadoApp::GetShadowTexture(uint32_t frameIndex) const
{
    return mPerFrame[frameIndex].shadowDrawPass->GetDepthStencilTexture();
}

grfx::GraphicsPipelinePtr FishTornadoApp::CreateForwardPipeline(
    const fs::path&          baseDir,
    const std::string&       vsBaseName,
    const std::string&       psBaseName,
    grfx::PipelineInterface* pPipelineInterface)
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::ShaderModulePtr VS, PS;
    PPX_CHECKED_CALL(ppxres = CreateShader(baseDir, vsBaseName, &VS));
    PPX_CHECKED_CALL(ppxres = CreateShader(baseDir, psBaseName, &PS));

    const grfx::VertexInputRate inputRate = grfx::VERTEX_INPUT_RATE_VERTEX;
    grfx::VertexDescription     vertexDescription;
    // clang-format off
    vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_POSITION , 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, inputRate});
    vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_COLOR    , 1, grfx::FORMAT_R32G32B32_FLOAT, 1, PPX_APPEND_OFFSET_ALIGNED, inputRate});
    vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_NORMAL   , 2, grfx::FORMAT_R32G32B32_FLOAT, 2, PPX_APPEND_OFFSET_ALIGNED, inputRate});
    vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_TEXCOORD , 3, grfx::FORMAT_R32G32_FLOAT,    3, PPX_APPEND_OFFSET_ALIGNED, inputRate});
    vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_TANGENT  , 4, grfx::FORMAT_R32G32B32_FLOAT, 4, PPX_APPEND_OFFSET_ALIGNED, inputRate});
    vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_BITANGENT, 5, grfx::FORMAT_R32G32B32_FLOAT, 5, PPX_APPEND_OFFSET_ALIGNED, inputRate});
    // clang-format on

    grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
    gpCreateInfo.VS                                 = {VS, "vsmain"};
    gpCreateInfo.PS                                 = {PS, "psmain"};
    gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
    gpCreateInfo.cullMode                           = grfx::CULL_MODE_BACK;
    gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
    gpCreateInfo.depthReadEnable                    = true;
    gpCreateInfo.depthWriteEnable                   = true;
    gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
    gpCreateInfo.outputState.renderTargetCount      = 1;
    gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
    gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
    gpCreateInfo.pPipelineInterface                 = IsNull(pPipelineInterface) ? mForwardPipelineInterface.Get() : pPipelineInterface;
    // Vertex description
    gpCreateInfo.vertexInputState.bindingCount = vertexDescription.GetBindingCount();
    for (uint32_t i = 0; i < vertexDescription.GetBindingCount(); ++i) {
        gpCreateInfo.vertexInputState.bindings[i] = *vertexDescription.GetBinding(i);
    }

    grfx::GraphicsPipelinePtr pipeline;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &pipeline));

    GetDevice()->DestroyShaderModule(VS);
    GetDevice()->DestroyShaderModule(PS);

    return pipeline;
}

grfx::GraphicsPipelinePtr FishTornadoApp::CreateShadowPipeline(
    const fs::path&          baseDir,
    const std::string&       vsBaseName,
    grfx::PipelineInterface* pPipelineInterface)
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::ShaderModulePtr VS;
    PPX_CHECKED_CALL(ppxres = CreateShader(baseDir, vsBaseName, &VS));

    grfx::GraphicsPipelineCreateInfo2 gpCreateInfo = {};
    gpCreateInfo.VS                                = {VS.Get(), "vsmain"};
    gpCreateInfo.vertexInputState.bindingCount     = 1;
    gpCreateInfo.vertexInputState.bindings[0]      = grfx::VertexBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_POSITION, 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});
    gpCreateInfo.topology                          = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    gpCreateInfo.polygonMode                       = grfx::POLYGON_MODE_FILL;
    gpCreateInfo.cullMode                          = grfx::CULL_MODE_FRONT;
    gpCreateInfo.frontFace                         = grfx::FRONT_FACE_CCW;
    gpCreateInfo.depthReadEnable                   = true;
    gpCreateInfo.depthWriteEnable                  = true;
    gpCreateInfo.blendModes[0]                     = grfx::BLEND_MODE_NONE;
    gpCreateInfo.outputState.renderTargetCount     = 0;
    gpCreateInfo.outputState.depthStencilFormat    = grfx::FORMAT_D32_FLOAT;
    gpCreateInfo.pPipelineInterface                = IsNull(pPipelineInterface) ? mForwardPipelineInterface.Get() : pPipelineInterface;

    grfx::GraphicsPipelinePtr pipeline;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &pipeline));

    GetDevice()->DestroyShaderModule(VS);

    return pipeline;
}

void FishTornadoApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName                    = "Fish Tornado";
    settings.window.width               = kWindowWidth;
    settings.window.height              = kWindowHeight;
    settings.grfx.api                   = kApi;
    settings.grfx.numFramesInFlight     = 2;
    settings.grfx.enableDebug           = true;
    settings.grfx.swapchain.imageCount  = 3;
    settings.grfx.swapchain.depthFormat = grfx::FORMAT_D32_FLOAT;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void FishTornadoApp::SetupDescriptorPool()
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::DescriptorPoolCreateInfo createInfo = {};
    createInfo.sampler                        = 1000;
    createInfo.sampledImage                   = 1000;
    createInfo.uniformBuffer                  = 1000;
    createInfo.structuredBuffer               = 1000;

    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&createInfo, &mDescriptorPool));
}

void FishTornadoApp::SetupSetLayouts()
{
    Result ppxres = ppx::ERROR_FAILED;

    // Scene
    grfx::DescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});
    createInfo.bindings.push_back(grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{2, grfx::DESCRIPTOR_TYPE_SAMPLER});
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mSceneDataSetLayout));

    // Model
    createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mModelDataSetLayout));

    // Material
    createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});
    createInfo.bindings.push_back(grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{2, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{3, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{4, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{5, grfx::DESCRIPTOR_TYPE_SAMPLER});
    createInfo.bindings.push_back(grfx::DescriptorBinding{6, grfx::DESCRIPTOR_TYPE_SAMPLER});
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mMaterialSetLayout));
}

void FishTornadoApp::SetupPipelineInterfaces()
{
    Result ppxres = ppx::ERROR_FAILED;

    // Forward render pipeline interface
    {
        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 3;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mSceneDataSetLayout;
        piCreateInfo.sets[1].set                       = 1;
        piCreateInfo.sets[1].pLayout                   = mModelDataSetLayout;
        piCreateInfo.sets[2].set                       = 2;
        piCreateInfo.sets[2].pLayout                   = mMaterialSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mForwardPipelineInterface));
    }
}

void FishTornadoApp::SetupTextures()
{
    Result ppxres = ppx::ERROR_FAILED;

    PPX_CHECKED_CALL(ppxres = CreateTexture1x1(GetGraphicsQueue(), {0, 0, 0, 0}, &m1x1BlackTexture));
}

void FishTornadoApp::SetupSamplers()
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::SamplerCreateInfo createInfo = {};
    createInfo.magFilter               = grfx::FILTER_LINEAR;
    createInfo.minFilter               = grfx::FILTER_LINEAR;
    createInfo.mipmapMode              = grfx::SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.addressModeU            = grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeV            = grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeW            = grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.minLod                  = 0.0f;
    createInfo.maxLod                  = FLT_MAX;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&createInfo, &mClampedSampler));

    createInfo              = {};
    createInfo.magFilter    = grfx::FILTER_LINEAR;
    createInfo.minFilter    = grfx::FILTER_LINEAR;
    createInfo.mipmapMode   = grfx::SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.addressModeU = grfx::SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = grfx::SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = grfx::SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.minLod       = 0.0f;
    createInfo.maxLod       = FLT_MAX;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&createInfo, &mRepeatSampler));

    createInfo               = {};
    createInfo.addressModeU  = grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeV  = grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeW  = grfx::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.compareEnable = true;
    createInfo.compareOp     = grfx::COMPARE_OP_LESS_OR_EQUAL;
    createInfo.borderColor   = grfx::BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&createInfo, &mShadowSampler));
}

void FishTornadoApp::SetupPerFrame()
{
    Result ppxres = ppx::ERROR_FAILED;

    const uint32_t numFramesInFlight = GetNumFramesInFlight();

    mPerFrame.resize(numFramesInFlight);

    for (uint32_t i = 0; i < numFramesInFlight; ++i) {
        PerFrame& frame = mPerFrame[i];

        PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd));

        grfx::SemaphoreCreateInfo semaCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore));

        grfx::FenceCreateInfo fenceCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence));

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore));

        fenceCreateInfo = {true}; // Create signaled
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence));

        // Scene constants buffer
        PPX_CHECKED_CALL(ppxres = frame.sceneConstants.Create(GetDevice(), 3 * PPX_MINIUM_CONSTANT_BUFFER_SIZE));

        // Shadow draw pass
        {
            grfx::DrawPassCreateInfo drawPassCreateInfo = {};
            drawPassCreateInfo.width                    = kShadowRes;
            drawPassCreateInfo.height                   = kShadowRes;
            drawPassCreateInfo.depthStencilFormat       = grfx::FORMAT_D32_FLOAT;
            drawPassCreateInfo.depthStencilUsageFlags   = grfx::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT | grfx::IMAGE_USAGE_SAMPLED;
            drawPassCreateInfo.depthStencilInitialState = grfx::RESOURCE_STATE_SHADER_RESOURCE;
            drawPassCreateInfo.depthStencilClearValue   = {1.0f, 0xFF};

            PPX_CHECKED_CALL(ppxres = ppxres = GetDevice()->CreateDrawPass(&drawPassCreateInfo, &frame.shadowDrawPass));
        }

        // Allocate scene descriptor set
        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mSceneDataSetLayout, &frame.sceneSet));
        // Update scene descriptor
        PPX_CHECKED_CALL(ppxres = frame.sceneSet->UpdateUniformBuffer(0, 0, frame.sceneConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = frame.sceneSet->UpdateSampledImage(1, 0, frame.shadowDrawPass->GetDepthStencilTexture()));
        PPX_CHECKED_CALL(ppxres = frame.sceneSet->UpdateSampler(2, 0, mShadowSampler));

        // Scene shadow
        //
        // NOTE: We store a separate just for the scene constants when rendering shadows because
        //       DX12 will throw a validation error if we don't set the descriptor to
        //       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE in the descriptor table range.
        //       The default value is D3D12_DESCRIPTOR_RANGE_FLAG_NONE which sets the descriptor and
        //       data to static.
        //
        // Allocate scene shadow descriptor set
        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mSceneDataSetLayout, &frame.sceneShadowSet));
        // Update scene shadow descriptor
        PPX_CHECKED_CALL(ppxres = frame.sceneShadowSet->UpdateUniformBuffer(0, 0, frame.sceneConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = frame.sceneShadowSet->UpdateSampledImage(1, 0, m1x1BlackTexture));
        PPX_CHECKED_CALL(ppxres = frame.sceneShadowSet->UpdateSampler(2, 0, mClampedSampler));
    }
}

void FishTornadoApp::SetupCaustics()
{
    Result ppxres = ppx::ERROR_FAILED;

    // Texture
    {
        // Load first file to get properties
        Bitmap bitmap;
        PPX_CHECKED_CALL(ppxres = Bitmap::LoadFile(GetAssetPath("fishtornado/textures/ocean/caustics/save.00.png"), &bitmap));

        grfx::TextureCreateInfo createInfo = {};
        createInfo.imageType               = grfx::IMAGE_TYPE_2D;
        createInfo.width                   = bitmap.GetWidth();
        createInfo.height                  = bitmap.GetHeight();
        createInfo.depth                   = 1;
        createInfo.imageFormat             = ToGrfxFormat(bitmap.GetFormat());
        createInfo.sampleCount             = grfx::SAMPLE_COUNT_1;
        createInfo.mipLevelCount           = 1;
        createInfo.arrayLayerCount         = kCausticsImageCount;
        createInfo.usageFlags              = grfx::ImageUsageFlags::SampledImage() | grfx::IMAGE_USAGE_TRANSFER_DST;
        createInfo.memoryUsage             = grfx::MEMORY_USAGE_GPU_ONLY;
        createInfo.initialState            = grfx::RESOURCE_STATE_SHADER_RESOURCE;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateTexture(&createInfo, &mCausticsTexture));
    }
}

void FishTornadoApp::UploadCaustics()
{
    Result ppxres = ppx::ERROR_FAILED;

    for (uint32_t i = 0; i < kCausticsImageCount; ++i) {
        Timer timer;
        PPX_ASSERT_MSG(timer.Start() == ppx::TIMER_RESULT_SUCCESS, "timer start failed");
        double fnStartTime = timer.SecondsSinceStart();

        std::stringstream filename;
        filename << "fishtornado/textures/ocean/caustics/save." << std::setw(2) << std::setfill('0') << i << ".png";
        fs::path path = GetAssetPath(filename.str());

        Bitmap bitmap;
        PPX_CHECKED_CALL(ppxres = Bitmap::LoadFile(path, &bitmap));

        PPX_CHECKED_CALL(ppxres = CopyBitmapToTexture(GetGraphicsQueue(), &bitmap, mCausticsTexture, 0, i, grfx::RESOURCE_STATE_SHADER_RESOURCE, grfx::RESOURCE_STATE_SHADER_RESOURCE));

        double fnEndTime = timer.SecondsSinceStart();
        float  fnElapsed = static_cast<float>(fnEndTime - fnStartTime);
        PPX_LOG_INFO("Created image from image file: " << path << " (" << FloatString(fnElapsed) << " seconds)");
    }
}

void FishTornadoApp::SetupDebug()
{
    Result ppxres = ppx::ERROR_FAILED;

    // Debug draw
    {
        mDebugDrawPipeline = CreateForwardPipeline(GetAssetPath("fishtornado/shaders"), "DebugDraw.vs", "DebugDraw.ps");
    }
}

void FishTornadoApp::SetupScene()
{
    mCamera.SetPerspective(45.0f, GetWindowAspect());
    mCamera.LookAt(float3(135.312f, 64.086f, -265.332f), float3(0.0f, 100.0f, 0.0f));
    mCamera.MoveAlongViewDirection(-300.0f);

    mShadowCamera.LookAt(float3(0.0f, 5000.0, 500.0f), float3(0.0f, 0.0f, 0.0f));
    mShadowCamera.SetPerspective(10.0f, 1.0f, 3500.0f, 5500.0f);
}

void FishTornadoApp::Setup()
{
    SetupDescriptorPool();
    SetupSetLayouts();
    SetupPipelineInterfaces();
    SetupTextures();
    SetupSamplers();
    SetupPerFrame();
    SetupCaustics();
    SetupDebug();

    const uint32_t numFramesInFlight = GetNumFramesInFlight();
    mFlocking.Setup(numFramesInFlight);
    mOcean.Setup(numFramesInFlight);
    mShark.Setup(numFramesInFlight);

    // Caustic iamge copy to GPU texture is giving Vulkan some grief
    // so we split up for now.
    //
    UploadCaustics();

    SetupScene();
}

void FishTornadoApp::Shutdown()
{
    mFlocking.Shutdown();
    mOcean.Shutdown();
    mShark.Shutdown();

    for (size_t i = 0; i < mPerFrame.size(); ++i) {
        PerFrame& frame = mPerFrame[i];
        frame.sceneConstants.Destroy();
    }
}

void FishTornadoApp::Scroll(float dx, float dy)
{
    mCamera.MoveAlongViewDirection(dy * -5.0f);
}

void FishTornadoApp::UpdateTime()
{
    static float sPrevTime = GetElapsedSeconds();
    float        curTime   = GetElapsedSeconds();
    float        dt        = curTime - sPrevTime;

    mDt       = std::min<float>(dt, 1.0f / 60.0f) * 6.0f;
    mTime     = mTime + mDt;
    sPrevTime = curTime;
}

void FishTornadoApp::UpdateScene(uint32_t frameIndex)
{
    PerFrame& frame = mPerFrame[frameIndex];

    hlsl::SceneData* pSceneData            = static_cast<hlsl::SceneData*>(frame.sceneConstants.GetMappedAddress());
    pSceneData->time                       = GetTime();
    pSceneData->eyePosition                = mCamera.GetEyePosition();
    pSceneData->viewMatrix                 = mCamera.GetViewMatrix();
    pSceneData->projectionMatrix           = mCamera.GetProjectionMatrix();
    pSceneData->viewProjectionMatrix       = mCamera.GetViewProjectionMatrix();
    pSceneData->fogNearDistance            = 20.0f;
    pSceneData->fogFarDistance             = 900.0f;
    pSceneData->fogPower                   = 1.0f;
    pSceneData->fogColor                   = kFogColor;
    pSceneData->lightPosition              = float3(0.0f, 5000.0, 500.0f);
    pSceneData->ambient                    = float3(0.45f, 0.45f, 0.5f) * 0.25f;
    pSceneData->shadowViewProjectionMatrix = mShadowCamera.GetViewProjectionMatrix();
    pSceneData->shadowTextureDim           = float2(kShadowRes);
    pSceneData->usePCF                     = static_cast<uint32_t>(mUsePCF);
}

void FishTornadoApp::Render()
{
    Result             ppxres     = ppx::SUCCESS;
    uint32_t           frameIndex = GetInFlightFrameIndex();
    PerFrame&          frame      = mPerFrame[frameIndex];
    grfx::SwapchainPtr swapchain  = GetSwapchain();

    UpdateTime();

    UpdateScene(frameIndex);
    mShark.Update(frameIndex);
    mFlocking.Update(frameIndex);
    mOcean.Update(frameIndex);

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(ppxres = swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(ppxres = frame.imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(ppxres = frame.renderCompleteFence->WaitAndReset());

    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        mShark.CopyConstantsToGpu(frameIndex, frame.cmd);
        mFlocking.CopyConstantsToGpu(frameIndex, frame.cmd);
        mOcean.CopyConstantsToGpu(frameIndex, frame.cmd);

        // Scene constants
        {
            frame.cmd->BufferResourceBarrier(frame.sceneConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

            grfx::BufferToBufferCopyInfo copyInfo = {};
            copyInfo.size                         = frame.sceneConstants.GetSize();

            frame.cmd->CopyBufferToBuffer(
                &copyInfo,
                frame.sceneConstants.GetCpuBuffer(),
                frame.sceneConstants.GetGpuBuffer());

            frame.cmd->BufferResourceBarrier(frame.sceneConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
        }

        // -----------------------------------------------------------------------------------------

        // Compute flocking
        mFlocking.Compute(frameIndex, frame.cmd);

        // -----------------------------------------------------------------------------------------

        frame.cmd->TransitionImageLayout(frame.shadowDrawPass, grfx::RESOURCE_STATE_UNDEFINED, grfx::RESOURCE_STATE_UNDEFINED, grfx::RESOURCE_STATE_SHADER_RESOURCE, grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE);
        frame.cmd->BeginRenderPass(frame.shadowDrawPass);
        {
            frame.cmd->SetScissors(frame.shadowDrawPass->GetScissor());
            frame.cmd->SetViewports(frame.shadowDrawPass->GetViewport());

            mShark.DrawShadow(frameIndex, frame.cmd);
            mFlocking.DrawShadow(frameIndex, frame.cmd);
        }
        frame.cmd->EndRenderPass();
        frame.cmd->TransitionImageLayout(frame.shadowDrawPass, grfx::RESOURCE_STATE_UNDEFINED, grfx::RESOURCE_STATE_UNDEFINED, grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE, grfx::RESOURCE_STATE_SHADER_RESOURCE);

        // -----------------------------------------------------------------------------------------

        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{kFogColor.r, kFogColor.g, kFogColor.b, 1.0f}};

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            frame.cmd->SetScissors(renderPass->GetScissor());
            frame.cmd->SetViewports(renderPass->GetViewport());

            mShark.DrawForward(frameIndex, frame.cmd);
            mFlocking.DrawForward(frameIndex, frame.cmd);
            mOcean.DrawForward(frameIndex, frame.cmd);

            //frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 0, nullptr);
            //frame.cmd->BindGraphicsPipeline(mPipeline);
            //frame.cmd->BindVertexBuffers(1, &mVertexBuffer, &mVertexBinding.GetStride());
            //frame.cmd->Draw(3, 1, 0, 0);

            // Draw ImGui
            DrawDebugInfo([this]() { this->DrawGui(); });
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

void FishTornadoApp::DrawGui()
{
    ImGui::Separator();

    ImGui::Checkbox("Use PCF Shadows", &mUsePCF);
}
