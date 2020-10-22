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
#define kWindowAspect (float)kWindowWidth / (float)kWindowHeight

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
        grfx::ModelPtr         model;
        grfx::DescriptorSetPtr descriptorSet;
        grfx::BufferPtr        uniformBuffer;
    };

    std::vector<PerFrame>          mPerFrame;
    grfx::ShaderModulePtr          mVS;
    grfx::ShaderModulePtr          mPS;
    grfx::PipelineInterfacePtr     mPipelineInterface;
    grfx::DescriptorPoolPtr        mDescriptorPool;
    grfx::DescriptorSetLayoutPtr   mDescriptorSetLayout;
    grfx::GraphicsPipelinePtr      mSkyBoxPipeline;
    Entity                         mSkyBox;
    grfx::GraphicsPipelinePtr      mReflectorPipeline;
    Entity                         mReflector;
    ppx::grfx::ImagePtr            mCubeMapImage;
    ppx::grfx::SampledImageViewPtr mCubeMapImageView;
    ppx::grfx::SamplerPtr          mCubeMapSampler;
    float                          mRotY = 0;
    float                          mRotX = 0;

private:
    void SetupSkyBox(const GeometryCreateInfo& createInfo, Entity* pEntity);
    void SetupReflector(const GeometryCreateInfo& createInfo, Entity* pEntity);
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName                    = "10_cube_map";
    settings.window.width               = kWindowWidth;
    settings.window.height              = kWindowHeight;
    settings.grfx.api                   = kApi;
    settings.grfx.swapchain.depthFormat = grfx::FORMAT_D32_FLOAT;
    settings.grfx.enableDebug           = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::SetupSkyBox(const GeometryCreateInfo& createInfo, Entity* pEntity)
{
    Result ppxres = ppx::SUCCESS;

    Geometry cube;
    PPX_CHECKED_CALL(ppxres = Geometry::CreateCube(createInfo, float3(8, 8, 8), &cube));
    PPX_CHECKED_CALL(ppxres = CreateModelFromGeometry(GetGraphicsQueue(), &cube, &pEntity->model));

    grfx::BufferCreateInfo bufferCreateInfo        = {};
    bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
    bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
    bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &pEntity->uniformBuffer));

    PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mDescriptorSetLayout, &pEntity->descriptorSet));

    grfx::WriteDescriptor write = {};
    write.binding               = 0;
    write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.bufferOffset          = 0;
    write.bufferRange           = PPX_WHOLE_SIZE;
    write.pBuffer               = pEntity->uniformBuffer;
    PPX_CHECKED_CALL(ppxres = pEntity->descriptorSet->UpdateDescriptors(1, &write));

    write            = {};
    write.binding    = 1;
    write.type       = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write.pImageView = mCubeMapImageView;
    PPX_CHECKED_CALL(ppxres = pEntity->descriptorSet->UpdateDescriptors(1, &write));

    write          = {};
    write.binding  = 2;
    write.type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
    write.pSampler = mCubeMapSampler;
    PPX_CHECKED_CALL(ppxres = pEntity->descriptorSet->UpdateDescriptors(1, &write));
}

void ProjApp::SetupReflector(const GeometryCreateInfo& createInfo, Entity* pEntity)
{
    Result ppxres = ppx::SUCCESS;

    Geometry objGeo;
    PPX_CHECKED_CALL(ppxres = Geometry::CreateFromOBJ(createInfo, GetAssetPath("basic/models/material_sphere.obj"), &objGeo));
    //PPX_CHECKED_CALL(ppxres = Geometry::CreateCube(createInfo, float3(1, 1, 1), &objGeo));
    PPX_CHECKED_CALL(ppxres = CreateModelFromGeometry(GetGraphicsQueue(), &objGeo, &pEntity->model));

    grfx::BufferCreateInfo bufferCreateInfo        = {};
    bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
    bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
    bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &pEntity->uniformBuffer));

    PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mDescriptorSetLayout, &pEntity->descriptorSet));

    grfx::WriteDescriptor write = {};
    write.binding               = 0;
    write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.bufferOffset          = 0;
    write.bufferRange           = PPX_WHOLE_SIZE;
    write.pBuffer               = pEntity->uniformBuffer;
    PPX_CHECKED_CALL(ppxres = pEntity->descriptorSet->UpdateDescriptors(1, &write));

    write            = {};
    write.binding    = 1;
    write.type       = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write.pImageView = mCubeMapImageView;
    PPX_CHECKED_CALL(ppxres = pEntity->descriptorSet->UpdateDescriptors(1, &write));

    write          = {};
    write.binding  = 2;
    write.type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
    write.pSampler = mCubeMapSampler;
    PPX_CHECKED_CALL(ppxres = pEntity->descriptorSet->UpdateDescriptors(1, &write));
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Texture image, view,  and sampler
    {
        CubeMapCreateInfo createInfo = {};
        createInfo.layout            = CUBE_IMAGE_LAYOUT_CROSS_HORIZONTAL;
        createInfo.posX              = PPX_ENCODE_CUBE_FACE(3, CUBE_FACE_OP_NONE, CUBE_FACE_OP_NONE);
        createInfo.negX              = PPX_ENCODE_CUBE_FACE(1, CUBE_FACE_OP_NONE, CUBE_FACE_OP_NONE);
        createInfo.posY              = PPX_ENCODE_CUBE_FACE(0, CUBE_FACE_OP_NONE, CUBE_FACE_OP_NONE);
        createInfo.negY              = PPX_ENCODE_CUBE_FACE(5, CUBE_FACE_OP_NONE, CUBE_FACE_OP_NONE);
        createInfo.posZ              = PPX_ENCODE_CUBE_FACE(2, CUBE_FACE_OP_NONE, CUBE_FACE_OP_NONE);
        createInfo.negZ              = PPX_ENCODE_CUBE_FACE(4, CUBE_FACE_OP_NONE, CUBE_FACE_OP_NONE);

        PPX_CHECKED_CALL(ppxres = CreateCubeMapFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("basic/textures/cube_map_debug.jpg"), &createInfo, &mCubeMapImage));

        grfx::SampledImageViewCreateInfo viewCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mCubeMapImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampledImageView(&viewCreateInfo, &mCubeMapImageView));

        grfx::SamplerCreateInfo samplerCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&samplerCreateInfo, &mCubeMapSampler));
    }

    // Descriptor stuff
    {
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = 2;
        poolCreateInfo.sampledImage                   = 2;
        poolCreateInfo.sampler                        = 2;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{2, grfx::DESCRIPTOR_TYPE_SAMPLER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDescriptorSetLayout));
    }

    // Entities
    {
        SetupSkyBox(GeometryCreateInfo::InterleavedU16().AddColor(), &mSkyBox);
        SetupReflector(GeometryCreateInfo::InterleavedU16().AddNormal(), &mReflector);
    }

    // Sky box pipeline
    {
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "SkyBox.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

        bytecode = LoadShader(GetAssetPath("basic/shaders"), "SkyBox.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mPipelineInterface));

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {mVS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {mPS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = mReflector.model->GetVertexBufferCount();
        gpCreateInfo.vertexInputState.bindings[0]       = *mReflector.model->GetVertexBinding(0);
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_FRONT;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = true;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
        gpCreateInfo.pPipelineInterface                 = mPipelineInterface;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mSkyBoxPipeline));
    }

    // Reflector pipeline
    {
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "CubeMap.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

        bytecode = LoadShader(GetAssetPath("basic/shaders"), "CubeMap.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mPipelineInterface));

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {mVS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {mPS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = mReflector.model->GetVertexBufferCount();
        gpCreateInfo.vertexInputState.bindings[0]       = *mReflector.model->GetVertexBinding(0);
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_BACK;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = true;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
        gpCreateInfo.pPipelineInterface                 = mPipelineInterface;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mReflectorPipeline));
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

    // Update uniform buffer
    {
        float3   eyePos = float3(0, 0, 5);
        float    t      = GetElapsedSeconds();
        float4x4 P      = glm::perspective(glm::radians(60.0f), kWindowAspect, 0.001f, 10000.0f);
        float4x4 V      = glm::lookAt(eyePos, float3(0, 0, 0), float3(0, 1, 0));
        float4x4 M      = glm::translate(float3(0, 0, 0));

        // Sky box
        float4x4 mat = P * V * M;
        mSkyBox.uniformBuffer->CopyFromSource(sizeof(mat), &mat);

        // Reflector
        struct Transform
        {
            float4x4 MVPMatrix;
            float4x4 ModelMatrix;
            float3x3 NormalMatrix;
            float3   EyePos;
        };

        float4x4 T = glm::translate(float3(0, 0, 0));
        float4x4 R = glm::rotate(glm::radians(mRotX), float3(1, 0, 0)) * glm::rotate(glm::radians(mRotY), float3(0, 1, 0));
        float4x4 S = glm::scale(float3(3, 3, 3));
        M          = T * R * S;
        float3x3 N = glm::inverseTranspose(float3x3(M));

        char constantData[PPX_MINIUM_CONSTANT_BUFFER_SIZE] = {0};
        // Get offsets to memeber vars
        float4x4* pMVPMatrix      = reinterpret_cast<float4x4*>(constantData + 0);
        float4x4* pModelMatrix    = reinterpret_cast<float4x4*>(constantData + 64);
        float3*   pNormalMatrixR0 = reinterpret_cast<float3*>(constantData + 128);
        float3*   pNormalMatrixR1 = reinterpret_cast<float3*>(constantData + 144);
        float3*   pNormalMatrixR2 = reinterpret_cast<float3*>(constantData + 160);
        float3*   pEyePos         = reinterpret_cast<float3*>(constantData + 176);
        // Assign values
        *pMVPMatrix      = P * V * M;
        *pModelMatrix    = M;
        *pNormalMatrixR0 = N[0];
        *pNormalMatrixR1 = N[1];
        *pNormalMatrixR2 = N[2];
        *pEyePos         = eyePos;
        mReflector.uniformBuffer->CopyFromSource(sizeof(constantData), constantData);
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
        beginInfo.DSVClearValue             = {1.0f, 0xFF};

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            frame.cmd->SetScissors(GetScissor());
            frame.cmd->SetViewports(GetViewport());

            // Draw reflector
            frame.cmd->BindGraphicsPipeline(mReflectorPipeline);
            frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 1, &mReflector.descriptorSet);
            frame.cmd->BindIndexBuffer(mReflector.model);
            frame.cmd->BindVertexBuffers(mReflector.model);
            frame.cmd->DrawIndexed(mReflector.model->GetIndexCount());

            // Draw sky box
            frame.cmd->BindGraphicsPipeline(mSkyBoxPipeline);
            frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 1, &mSkyBox.descriptorSet);
            frame.cmd->BindIndexBuffer(mSkyBox.model);
            frame.cmd->BindVertexBuffers(mSkyBox.model);
            frame.cmd->DrawIndexed(mSkyBox.model->GetIndexCount());

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
    ImGui::Separator();

    ImGui::SliderFloat("Rot X", &mRotX, 0.0f, 360.0f, "%.03f degrees");
    ImGui::SliderFloat("Rot Y", &mRotY, 0.0f, 360.0f, "%.03f degrees");
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
