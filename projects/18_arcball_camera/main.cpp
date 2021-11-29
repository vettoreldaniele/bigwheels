#include "ppx/ppx.h"
#include "ppx/camera.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#if defined(PORTO_D3DCOMPILE)
#include "ppx/grfx/dx/d3dcompile_util.h"
#endif

#if defined(USE_DX11)
const grfx::Api kApi = grfx::API_DX_11_1;
#elif defined(USE_DX12)
const grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
const grfx::Api kApi = grfx::API_VK_1_1;
#endif

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void MouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons) override;
    virtual void Scroll(float dx, float dy) override;
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

    struct Entity
    {
        grfx::ModelPtr         model;
        grfx::DescriptorSetPtr descriptorSet;
        grfx::BufferPtr        uniformBuffer;
    };

    std::vector<PerFrame>        mPerFrame;
    grfx::ShaderModulePtr        mVS;
    grfx::ShaderModulePtr        mPS;
    grfx::PipelineInterfacePtr   mPipelineInterface;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mDescriptorSetLayout;
    grfx::GraphicsPipelinePtr    mTrianglePipeline;
    uint32_t                     mWindowWidth;
    uint32_t                     mWindowHeight;
    float                        mWindowAspect;
    Entity                       mCube;
    grfx::GraphicsPipelinePtr    mWirePipeline;
    Entity                       mWirePlane;

    ArcballCamera mArcballCamera;

private:
    void SetupEntity(const TriMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity);
    void SetupEntity(const WireMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity);
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    // If user did not provide resolution from the CL use this default
    if (GetStandardOptions().resolution.first == -1 && GetStandardOptions().resolution.second == -1) {
        settings.window.width  = 1280;
        settings.window.height = 720;
    }
    settings.appName                    = "18_arcball_camera";
    settings.grfx.api                   = kApi;
    settings.grfx.swapchain.depthFormat = grfx::FORMAT_D32_FLOAT;
    settings.grfx.enableDebug           = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
#if defined(USE_DXVK_SPV)
    settings.grfx.enableDXVKSPV = true;
#endif
    mWindowWidth  = settings.window.width;
    mWindowHeight = settings.window.height;
    mWindowAspect = float(mWindowWidth) / float(mWindowHeight);
}

void ProjApp::SetupEntity(const TriMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity)
{
    Result ppxres = ppx::SUCCESS;

    PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromTriMesh(GetGraphicsQueue(), &mesh, &pEntity->model));

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
}

void ProjApp::SetupEntity(const WireMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity)
{
    Result ppxres = ppx::SUCCESS;

    PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromWireMesh(GetGraphicsQueue(), &mesh, &pEntity->model));

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
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Descriptor stuff
    {
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = 6;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDescriptorSetLayout));
    }

    // Entities
    {
        GeometryOptions geometryOptions = GeometryOptions::Planar().AddColor();
        TriMeshOptions  triMeshOptions  = TriMeshOptions().Indices().VertexColors();
        WireMeshOptions wireMeshOptions = WireMeshOptions().Indices().VertexColors();

        TriMesh triMesh = TriMesh::CreateCube(float3(2, 2, 2), triMeshOptions);
        SetupEntity(triMesh, geometryOptions, &mCube);

        WireMesh wireMesh = WireMesh::CreatePlane(WIRE_MESH_PLANE_POSITIVE_Y, float2(10, 10), 10, 10, wireMeshOptions);
        SetupEntity(wireMesh, geometryOptions, &mWirePlane);
    }

    // Pipelines
    {
#if defined(PORTO_D3DCOMPILE)
        grfx::dx::ShaderIncludeHandler basicShaderIncludeHandler(
            GetAssetPath("basic/shaders"));
        std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "VertexColors", "vs_5_0", &basicShaderIncludeHandler);
#else
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "VertexColors.vs");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

#if defined(PORTO_D3DCOMPILE)
        bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "VertexColors", "ps_5_0", &basicShaderIncludeHandler);
#else
        bytecode = LoadShader(GetAssetPath("basic/shaders"), "VertexColors.ps");
#endif
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
        gpCreateInfo.vertexInputState.bindingCount      = mCube.model->GetVertexBufferCount();
        gpCreateInfo.vertexInputState.bindings[0]       = *mCube.model->GetVertexBinding(0);
        gpCreateInfo.vertexInputState.bindings[1]       = *mCube.model->GetVertexBinding(1);
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthReadEnable                    = true;
        gpCreateInfo.depthWriteEnable                   = true;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
        gpCreateInfo.pPipelineInterface                 = mPipelineInterface;

        // Triange pipeline
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mTrianglePipeline));

        // Wire pipeline
        gpCreateInfo.topology = grfx::PRIMITIVE_TOPOLOGY_LINE_LIST;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mWirePipeline));
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

    // Arcball camera
    {
        mArcballCamera.LookAt(float3(4, 5, 8), float3(0, 0, 0), float3(0, 1, 0));
        mArcballCamera.SetPerspective(60.0f, GetWindowAspect());
    }
}

void ProjApp::MouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons)
{
    if (buttons & ppx::MOUSE_BUTTON_LEFT) {
        int32_t prevX = x - dx;
        int32_t prevY = y - dy;

        float2 prevPos = GetNormalizedDeviceCoordinates(prevX, prevY);
        float2 curPos  = GetNormalizedDeviceCoordinates(x, y);

        mArcballCamera.Rotate(prevPos, curPos);
    }
    else if (buttons & ppx::MOUSE_BUTTON_RIGHT) {
        int32_t prevX = x - dx;
        int32_t prevY = y - dy;

        float2 prevPos = GetNormalizedDeviceCoordinates(prevX, prevY);
        float2 curPos  = GetNormalizedDeviceCoordinates(x, y);
        float2 delta   = curPos - prevPos;

        mArcballCamera.Pan(delta);
    }
}

void ProjApp::Scroll(float dx, float dy)
{
    mArcballCamera.Zoom(dy / 2.0f);
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
        float           t = GetElapsedSeconds();
        const float4x4& P = mArcballCamera.GetProjectionMatrix();
        const float4x4& V = mArcballCamera.GetViewMatrix();

        // Move cube up so it sits on top of the plane
        float4x4 T   = glm::translate(float3(0, 1, 0));
        float4x4 mat = P * V * T;
        mCube.uniformBuffer->CopyFromSource(sizeof(mat), &mat);

        T   = glm::translate(float3(0, 0, 0));
        mat = P * V * T;
        mWirePlane.uniformBuffer->CopyFromSource(sizeof(mat), &mat);
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

            // Triangle pipeline
            frame.cmd->BindGraphicsPipeline(mTrianglePipeline);

            // Cube
            frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 1, &mCube.descriptorSet);
            frame.cmd->BindIndexBuffer(mCube.model);
            frame.cmd->BindVertexBuffers(mCube.model);
            frame.cmd->DrawIndexed(mCube.model->GetIndexCount());

            // -------------------------------------------------------------------------------------
            //
            // Wire pipeline
            frame.cmd->BindGraphicsPipeline(mWirePipeline);

            // Wire plane
            frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 1, &mWirePlane.descriptorSet);
            frame.cmd->BindIndexBuffer(mWirePlane.model);
            frame.cmd->BindVertexBuffers(mWirePlane.model);
            frame.cmd->DrawIndexed(mWirePlane.model->GetIndexCount());

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
