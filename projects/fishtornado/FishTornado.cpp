#include "FishTornado.h"
#include "ppx/graphics_util.h"

#define kWindowWidth  1920
#define kWindowHeight 1080

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
    gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
    gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
    gpCreateInfo.depthReadEnable                    = true;
    gpCreateInfo.depthWriteEnable                   = true;
    gpCreateInfo.blendModes[PPX_MAX_RENDER_TARGETS] = {grfx::BLEND_MODE_NONE};
    gpCreateInfo.outputState.renderTargetCount      = 1;
    gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
    gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
    gpCreateInfo.pPipelineInterface                 = IsNull(pPipelineInterface) ? mForwardPipelineInterface : pPipelineInterface;
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

    grfx::DescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mSceneDataSetLayout));
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mModelDataSetLayout));

    createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});
    createInfo.bindings.push_back(grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{2, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{3, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});
    createInfo.bindings.push_back(grfx::DescriptorBinding{4, grfx::DESCRIPTOR_TYPE_SAMPLER});
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
        PPX_CHECKED_CALL(ppxres = frame.sceneConstants.Create(GetDevice(), PPX_MINIUM_CONSTANT_BUFFER_SIZE));

        // Allocate descriptor set
        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mSceneDataSetLayout, &frame.sceneSet));

        // Update descriptor
        PPX_CHECKED_CALL(ppxres = frame.sceneSet->UpdateUniformBuffer(0, 0, frame.sceneConstants.GetGpuBuffer()));
    }
}

void FishTornadoApp::SetupSamplers()
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::SamplerCreateInfo createInfo = {};
    createInfo.magFilter               = grfx::FILTER_LINEAR;
    createInfo.minFilter               = grfx::FILTER_LINEAR;
    createInfo.mipmapMode              = grfx::SAMPLER_MIPMAP_MODE_LINEAR;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&createInfo, &mClampedSampler));
}

void FishTornadoApp::SetupDebug()
{
    Result ppxres = ppx::ERROR_FAILED;

    // Debug draw
    {
        mDebugDrawPipeline = CreateForwardPipeline(GetAssetPath("fishtornado/shaders"), "DebugDraw.vs", "DebugDraw.ps");

        //grfx::ShaderModulePtr VS, PS;
        //PPX_CHECKED_CALL(ppxres = CreateShader(GetAssetPath("fishtornado/shaders"), "DebugDraw.vs", &VS));
        //PPX_CHECKED_CALL(ppxres = CreateShader(GetAssetPath("fishtornado/shaders"), "DebugDraw.ps", &PS));
        //
        //grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        //gpCreateInfo.VS                                 = {VS, "vsmain"};
        //gpCreateInfo.PS                                 = {PS, "psmain"};
        //gpCreateInfo.vertexInputState.bindingCount      = 2;
        //gpCreateInfo.vertexInputState.bindings[0]       = grfx::VertexAttribute{PPX_SEMANTIC_NAME_POSITION, 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX};
        //gpCreateInfo.vertexInputState.bindings[1]       = grfx::VertexAttribute{PPX_SEMANTIC_NAME_COLOR, 1, grfx::FORMAT_R32G32B32_FLOAT, 1, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX};
        //gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        //gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        //gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        //gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        //gpCreateInfo.depthReadEnable                    = true;
        //gpCreateInfo.depthWriteEnable                   = true;
        //gpCreateInfo.blendModes[PPX_MAX_RENDER_TARGETS] = {grfx::BLEND_MODE_NONE};
        //gpCreateInfo.outputState.renderTargetCount      = 1;
        //gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        //gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
        //gpCreateInfo.pPipelineInterface                 = mForwardPipelineInterface;
        //PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mDebugDrawPipeline));
        //
        //GetDevice()->DestroyShaderModule(VS);
        //GetDevice()->DestroyShaderModule(PS);
    }
}

void FishTornadoApp::SetupScene()
{
    mCamera.SetPerspective(45.0f, GetWindowAspect());
    mCamera.LookAt(float3(135.312f, 64.086f, -265.332f), float3(0.0f, 100.0f, 0.0f));
}

void FishTornadoApp::Setup()
{
    SetupDescriptorPool();
    SetupSetLayouts();
    SetupPipelineInterfaces();
    SetupPerFrame();
    SetupSamplers();
    SetupDebug();

    const uint32_t numFramesInFlight = GetNumFramesInFlight();
    mFlocking.Setup(numFramesInFlight);
    mOcean.Setup(numFramesInFlight);
    mShark.Setup(numFramesInFlight);

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

    hlsl::SceneData* pSceneData      = static_cast<hlsl::SceneData*>(frame.sceneConstants.GetMappedAddress());
    pSceneData->time                 = GetTime();
    pSceneData->eyePosition          = mCamera.GetEyePosition();
    pSceneData->viewMatrix           = mCamera.GetViewMatrix();
    pSceneData->projectionMatrix     = mCamera.GetProjectionMatrix();
    pSceneData->viewProjectionMatrix = mCamera.GetViewProjectionMatrix();
    pSceneData->fogNearDistance      = 20.0f;
    pSceneData->fogFarDistance       = 900.0f;
    pSceneData->fogPower             = 1.0f;
    pSceneData->fogColor             = kFogColor;
    pSceneData->lightPosition        = float3(0.0f, 5000.0, 500.0f);
    pSceneData->ambient              = float3(0.25f, 0.45f, 0.5f);
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

            //frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 0, nullptr);
            //frame.cmd->BindGraphicsPipeline(mPipeline);
            //frame.cmd->BindVertexBuffers(1, &mVertexBuffer, &mVertexBinding.GetStride());
            //frame.cmd->Draw(3, 1, 0, 0);

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