#include "ppx/ppx.h"
#include "ppx/camera.h"
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
    void DrawGui();

private:
    struct PerFrame
    {
        grfx::CommandBufferPtr cmd;
        grfx::SemaphorePtr     imageAcquiredSemaphore;
        grfx::FencePtr         imageAcquiredFence;
        grfx::SemaphorePtr     renderCompleteSemaphore;
        grfx::FencePtr         renderCompleteFence;
    };

    struct Entity
    {
        float3                 translate = float3(0, 0, 0);
        float3                 rotate    = float3(0, 0, 0);
        float3                 scale     = float3(1, 1, 1);
        grfx::ModelPtr         model;
        grfx::DescriptorSetPtr drawDescriptorSet;
        grfx::BufferPtr        drawUniformBuffer;
    };

    std::vector<PerFrame>        mPerFrame;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mDrawObjectSetLayout;
    grfx::PipelineInterfacePtr   mDrawObjectPipelineInterface;
    grfx::GraphicsPipelinePtr    mDrawObjectPipeline;
    grfx::ImagePtr               mAlbedoTexture;
    grfx::ImagePtr               mNormalMap;
    grfx::SampledImageViewPtr    mAlbedoTextureView;
    grfx::SampledImageViewPtr    mNormalMapView;
    grfx::SamplerPtr             mSampler;
    Entity                       mCube;
    PerspCamera                  mCamera;

    grfx::DescriptorSetLayoutPtr mLightSetLayout;
    grfx::PipelineInterfacePtr   mLightPipelineInterface;
    grfx::GraphicsPipelinePtr    mLightPipeline;
    Entity                       mLight;
    float3                       mLightPosition = float3(0, 5, 5);

private:
    void SetupEntity(
        const TriMesh&                   mesh,
        grfx::DescriptorPool*            pDescriptorPool,
        const grfx::DescriptorSetLayout* pDrawSetLayout,
        const grfx::DescriptorSetLayout* pShadowSetLayout,
        Entity*                          pEntity);
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName                    = "normal_map";
    settings.window.width               = kWindowWidth;
    settings.window.height              = kWindowHeight;
    settings.grfx.api                   = kApi;
    settings.grfx.swapchain.depthFormat = grfx::FORMAT_D32_FLOAT;
    settings.grfx.enableDebug           = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::SetupEntity(
    const TriMesh&                   mesh,
    grfx::DescriptorPool*            pDescriptorPool,
    const grfx::DescriptorSetLayout* pDrawSetLayout,
    const grfx::DescriptorSetLayout* pShadowSetLayout,
    Entity*                          pEntity)
{
    Result ppxres = ppx::SUCCESS;

    Geometry geo;
    PPX_CHECKED_CALL(ppxres = Geometry::Create(mesh, &geo));
    PPX_CHECKED_CALL(ppxres = CreateModelFromGeometry(GetGraphicsQueue(), &geo, &pEntity->model));

    // Draw uniform buffer
    grfx::BufferCreateInfo bufferCreateInfo        = {};
    bufferCreateInfo.size                          = RoundUp(512, PPX_CONSTANT_BUFFER_ALIGNMENT);
    bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
    bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &pEntity->drawUniformBuffer));

    // Draw descriptor set
    PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(pDescriptorPool, pDrawSetLayout, &pEntity->drawDescriptorSet));

    // Update draw descriptor set
    grfx::WriteDescriptor writes[4] = {};
    writes[0].binding               = 0; // Uniform buffer
    writes[0].type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].bufferOffset          = 0;
    writes[0].bufferRange           = PPX_WHOLE_SIZE;
    writes[0].pBuffer               = pEntity->drawUniformBuffer;
    writes[1].binding               = 1; // Albedo texture
    writes[1].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    writes[1].pImageView            = mAlbedoTextureView;
    writes[2].binding               = 2; // Normal map
    writes[2].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    writes[2].pImageView            = mNormalMapView;
    writes[3].binding               = 3; // Sampler
    writes[3].type                  = grfx::DESCRIPTOR_TYPE_SAMPLER;
    writes[3].pSampler              = mSampler;

    PPX_CHECKED_CALL(ppxres = pEntity->drawDescriptorSet->UpdateDescriptors(4, writes));
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Cameras
    {
        mCamera = PerspCamera(60.0f, GetWindowAspect());
    }

    // Create descriptor pool large enough for this project
    {
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = 512;
        poolCreateInfo.sampledImage                   = 512;
        poolCreateInfo.sampler                        = 512;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));
    }

    // Descriptor set layouts
    {
        // Draw objects
        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_PS});
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{2, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_PS});
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{3, grfx::DESCRIPTOR_TYPE_SAMPLER, 1, grfx::SHADER_STAGE_PS});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDrawObjectSetLayout));
    }

    // Textures, views, and samplers
    {
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("basic/textures/normal_map/albedo.jpg"), &mAlbedoTexture));
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("basic/textures/normal_map/normal.jpg"), &mNormalMap));

        grfx::SampledImageViewCreateInfo sivCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mAlbedoTexture);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampledImageView(&sivCreateInfo, &mAlbedoTextureView));

        sivCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mNormalMap);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampledImageView(&sivCreateInfo, &mNormalMapView));

        grfx::SamplerCreateInfo samplerCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&samplerCreateInfo, &mSampler));
    }

    // Setup entities
    {
        TriMesh::Options options = TriMesh::Options().Indices().Normals().TexCoords().Tangents();
        TriMesh          mesh    = TriMesh::CreateCube(float3(2, 2, 2), TriMesh::Options(options).ObjectColor(float3(0.7f)));
        SetupEntity(mesh, mDescriptorPool, mDrawObjectSetLayout, mDrawObjectSetLayout, &mCube);
    }

    // Draw object pipeline interface and pipeline
    {
        // Pipeline interface
        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mDrawObjectSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mDrawObjectPipelineInterface));

        // Pipeline
        grfx::ShaderModulePtr VS;
        std::vector<char>     bytecode = LoadShader(GetAssetPath("basic/shaders"), "NormalMap.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        grfx::ShaderModulePtr PS;
        bytecode = LoadShader(GetAssetPath("basic/shaders"), "NormalMap.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {VS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {PS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = mCube.model->GetVertexBufferCount();
        gpCreateInfo.vertexInputState.bindings[0]       = *mCube.model->GetVertexBinding(0);
        gpCreateInfo.vertexInputState.bindings[1]       = *mCube.model->GetVertexBinding(1);
        gpCreateInfo.vertexInputState.bindings[2]       = *mCube.model->GetVertexBinding(2);
        gpCreateInfo.vertexInputState.bindings[3]       = *mCube.model->GetVertexBinding(3);
        gpCreateInfo.vertexInputState.bindings[4]       = *mCube.model->GetVertexBinding(4);
        gpCreateInfo.vertexInputState.bindings[5]       = *mCube.model->GetVertexBinding(5);
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_BACK;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = true;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
        gpCreateInfo.pPipelineInterface                 = mDrawObjectPipelineInterface;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mDrawObjectPipeline));
        GetDevice()->DestroyShaderModule(VS);
        GetDevice()->DestroyShaderModule(PS);
    }

    // Light
    {
        // Descriptor set layt
        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mLightSetLayout));

        // Model
        TriMesh::Options options = TriMesh::Options().Indices().ObjectColor(float3(1, 1, 1));
        TriMesh          mesh    = TriMesh::CreateCube(float3(0.25f, 0.25f, 0.25f), options);

        Geometry geo;
        PPX_CHECKED_CALL(ppxres = Geometry::Create(mesh, &geo));
        PPX_CHECKED_CALL(ppxres = CreateModelFromGeometry(GetGraphicsQueue(), &geo, &mLight.model));

        // Uniform buffer
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mLight.drawUniformBuffer));

        // Descriptor set
        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mLightSetLayout, &mLight.drawDescriptorSet));

        // Update descriptor set
        grfx::WriteDescriptor write = {};
        write.binding               = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset          = 0;
        write.bufferRange           = PPX_WHOLE_SIZE;
        write.pBuffer               = mLight.drawUniformBuffer;
        PPX_CHECKED_CALL(ppxres = mLight.drawDescriptorSet->UpdateDescriptors(1, &write));

        // Pipeline interface
        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mLightSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mLightPipelineInterface));

        // Pipeline
        grfx::ShaderModulePtr VS;
        std::vector<char>     bytecode = LoadShader(GetAssetPath("basic/shaders"), "VertexColors.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        grfx::ShaderModulePtr PS;
        bytecode = LoadShader(GetAssetPath("basic/shaders"), "VertexColors.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {VS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {PS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = 2;
        gpCreateInfo.vertexInputState.bindings[0]       = *mLight.model->GetVertexBinding(0);
        gpCreateInfo.vertexInputState.bindings[1]       = *mLight.model->GetVertexBinding(1);
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_BACK;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = true;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
        gpCreateInfo.pPipelineInterface                 = mLightPipelineInterface;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mLightPipeline));
        GetDevice()->DestroyShaderModule(VS);
        GetDevice()->DestroyShaderModule(PS);
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

    // Update light position
    float t        = GetElapsedSeconds() / 2.0f;
    float r        = 3.0f;
    mLightPosition = float3(2, 2, 2);

    // Update camera(s)
    mCamera.LookAt(float3(0, 0, 5), float3(0, 0, 0));

    // Update uniform buffer(s)
    {
        Entity* pEntity = &mCube;

        pEntity->rotate = float3(t, t, 2*t);

        float4x4 T = glm::translate(pEntity->translate);
        float4x4 R = glm::rotate(pEntity->rotate.z, float3(0, 0, 1)) *
                     glm::rotate(pEntity->rotate.y, float3(0, 1, 0)) *
                     glm::rotate(pEntity->rotate.x, float3(1, 0, 0));
        float4x4 S = glm::scale(pEntity->scale);
        float4x4 M = T * R * S;

        // Draw uniform buffers
        struct Scene
        {
            float4x4 ModelMatrix;                // Transforms object space to world space
            float4x4 NormalMatrix;               // Transforms object space to normal space
            float4   Ambient;                    // Object's ambient intensity
            float4x4 CameraViewProjectionMatrix; // Camera's view projection matrix
            float4   LightPosition;              // Light's position
        };

        Scene scene                      = {};
        scene.ModelMatrix                = M;
        scene.NormalMatrix               = glm::inverseTranspose(M);
        scene.Ambient                    = float4(0.3f);
        scene.CameraViewProjectionMatrix = mCamera.GetViewProjectionMatrix();
        scene.LightPosition              = float4(mLightPosition, 0);

        pEntity->drawUniformBuffer->CopyFromSource(sizeof(scene), &scene);
    }

    // Update light uniform buffer
    {
        float4x4        T   = glm::translate(mLightPosition);
        const float4x4& PV  = mCamera.GetViewProjectionMatrix();
        float4x4        MVP = PV * T; // Yes - the other is reversed

        mLight.drawUniformBuffer->CopyFromSource(sizeof(MVP), &MVP);
    }

    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        // =====================================================================
        //  Render scene
        // =====================================================================
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(renderPass);
        {
            frame.cmd->SetScissors(GetScissor());
            frame.cmd->SetViewports(GetViewport());

            // Draw entities
            frame.cmd->BindGraphicsPipeline(mDrawObjectPipeline);
            frame.cmd->BindGraphicsDescriptorSets(mDrawObjectPipelineInterface, 1, &mCube.drawDescriptorSet);
            frame.cmd->BindIndexBuffer(mCube.model);
            frame.cmd->BindVertexBuffers(mCube.model);
            frame.cmd->DrawIndexed(mCube.model->GetIndexCount());

            // Draw light
            frame.cmd->BindGraphicsPipeline(mLightPipeline);
            frame.cmd->BindGraphicsDescriptorSets(mLightPipelineInterface, 1, &mLight.drawDescriptorSet);
            frame.cmd->BindIndexBuffer(mLight.model);
            frame.cmd->BindVertexBuffers(mLight.model);
            frame.cmd->DrawIndexed(mLight.model->GetIndexCount());

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

void ProjApp::DrawGui()
{

}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}