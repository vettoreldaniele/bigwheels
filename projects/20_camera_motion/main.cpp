#include <cstdlib>

#include "ppx/ppx.h"
#include "ppx/camera.h"
#include "ppx/graphics_util.h"
#include <ppx/random.h>
#include <set>

using namespace ppx;

#if defined(USE_DX11)
const grfx::Api kApi = grfx::API_DX_11_1;
#elif defined(USE_DX12)
const grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
const grfx::Api kApi = grfx::API_VK_1_1;
#endif

#define kWindowWidth  1920
#define kWindowHeight 1080

// Number of entities. Add one more for the floor.
#define kNumEntities (45 + 1)

// Size of the world grid.
#define kGridDepth 100
#define kGridWidth 100

class ProjApp
    : public ppx::Application
{
public:
    ProjApp()
        : mPerFrame(),
          mVS(nullptr),
          mPS(nullptr),
          mPipelineInterface(nullptr),
          mDescriptorPool(nullptr),
          mDescriptorSetLayout(nullptr),
          mEntities(),
          mPerspCamera(),
          mArcballCamera(),
          mCurrentCamera(nullptr),
          mPressedKeys(),
          mRateOfMove(0.0),
          mLastInputCheck(0.0) {}
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void MouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons) override;
    virtual void Render() override;
    virtual void KeyDown(KeyCode key) override;
    virtual void KeyUp(KeyCode key) override;

private:
    struct PerFrame
    {
        grfx::CommandBufferPtr cmd;
        grfx::SemaphorePtr     imageAcquiredSemaphore;
        grfx::FencePtr         imageAcquiredFence;
        grfx::SemaphorePtr     renderCompleteSemaphore;
        grfx::FencePtr         renderCompleteFence;
    };

    enum EntityKind
    {
        FLOOR    = 0,
        TRI_MESH = 1,
        OBJECT   = 2
    };

    struct Entity
    {
        grfx::ModelPtr            model;
        grfx::DescriptorSetPtr    descriptorSet;
        grfx::BufferPtr           uniformBuffer;
        grfx::GraphicsPipelinePtr pipeline;
        float3                    location;
        float3                    dimension;
        enum EntityKind           kind;
        Entity()
            : model(nullptr),
              descriptorSet(nullptr),
              uniformBuffer(nullptr),
              pipeline(nullptr),
              location(0, 0, 0),
              dimension(0, 0, 0),
              kind(FLOOR) {}

        // Place this entity in a random location within the given sub-grid index.
        // @param subGridIx - Index identifying the sub-grid where the object should be randomly positioned.
        // @param subGridWidth - Width of the sub-grid.
        // @param subGridDepth - Depth of the sub-grid.
        void Place(int32_t subGridIx, ppx::Random random, const int2& gridDim, const int2& subGridDim);
    };

    std::vector<PerFrame>        mPerFrame;
    grfx::ShaderModulePtr        mVS;
    grfx::ShaderModulePtr        mPS;
    grfx::PipelineInterfacePtr   mPipelineInterface;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mDescriptorSetLayout;
    std::vector<Entity>          mEntities;
    PerspCamera                  mPerspCamera;
    ArcballCamera                mArcballCamera;
    PerspCamera*                 mCurrentCamera;
    std::set<KeyCode>            mPressedKeys;
    float                        mRateOfMove;
    float                        mLastInputCheck;

    void SetupDescriptors();
    void SetupPipelines();
    void SetupPerFrameData();
    void SetupCamera();
    void UpdateCamera(PerspCamera* camera, float3 cameraPosition, float3 lookAt);
    void SetupEntities();
    void SetupEntity(const TriMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity);
    void SetupEntity(const WireMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity);
    void ProcessInput();
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName                    = "20_camera_motion";
    settings.window.width               = kWindowWidth;
    settings.window.height              = kWindowHeight;
    settings.grfx.api                   = kApi;
    settings.grfx.swapchain.depthFormat = grfx::FORMAT_D32_FLOAT;
    settings.grfx.enableDebug           = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::SetupEntity(const TriMesh& mesh, const GeometryOptions& createInfo, Entity* pEntity)
{
    Result ppxres = ppx::SUCCESS;

    PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromTriMesh(GetGraphicsQueue(), &mesh, &pEntity->model));

    grfx::BufferCreateInfo bufferCreateInfo        = {};
    bufferCreateInfo.size                          = RoundUp(512, PPX_CONSTANT_BUFFER_ALIGNMENT);
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

void ProjApp::Entity::Place(int32_t subGridIx, ppx::Random random, const int2& gridDim, const int2& subGridDim)
{
    // The original grid has been split in equal-sized sub-grids that preserve the same ratio.  There are
    // as many sub-grids as entities to place.  The entity will be placed at random inside the sub-grid
    // with index @param subGridIx.
    //
    // Each sub-grid is assumed to have its origin at top-left.
    int32_t sgx = random.UInt32() % subGridDim[0];
    int32_t sgz = random.UInt32() % subGridDim[1];
    PPX_LOG_INFO("Object location in grid #" << subGridIx << ": (" << sgx << ", " << sgz << ")");

    // Translate the location relative to the sub-grid location to the main grid coordinates.
    int32_t xDisplacement = subGridDim[0] * subGridIx;
    int32_t x             = (xDisplacement + sgx) % gridDim[0];
    int32_t z             = sgz + (subGridDim[1] * (xDisplacement / gridDim[0]));
    PPX_LOG_INFO("xDisplacement: " << xDisplacement);
    PPX_LOG_INFO("Object location in main grid: (" << x << ", " << z << ")");

    // All the calculations above assume that the main grid has its origin at the top-left corner,
    // but grids have their origin in the center.  So, we need to shift the location accordingly.
    int32_t adjX = x - gridDim[0] / 2;
    int32_t adjZ = z - gridDim[1] / 2;
    PPX_LOG_INFO("Adjusted object location in main grid: (" << adjX << ", " << adjZ << ")\n\n");
    location = float3(adjX, 1, adjZ);
}

void ProjApp::SetupEntities()
{
    GeometryOptions geometryOptions = GeometryOptions::Planar().AddColor();

    // Each object will live in a square region on the grid.  The size of each grid
    // depends on how many objects we need to place.  Note that since the first
    // entity is the grid itself, we ignore it here.
    int numObstacles = (kNumEntities > 1) ? kNumEntities - 1 : 0;
    PPX_ASSERT_MSG(numObstacles > 0, "There should be at least 1 obstacle in the grid");

    // Using the total area of the main grid and the grid ratio, compute the height and
    // width of each sub-grid where each object will be placed at random. Each sub-grid
    // has the same ratio as the original grid.
    //
    // To compute the depth (SGD) and width (SGW) of each sub-grid, we start with:
    //
    // Grid area:  A = kGridWidth * kGridDepth
    // Grid ratio: R = kGridWidth / kGridDepth
    // Number of objects: N
    // Sub-grid area: SGA = A / N
    //
    // SGA = SGW * SGD
    // R = SGW / SGD
    //
    // Solving for SGW and SGD, we get:
    //
    // SGD = sqrt(SGA / R)
    // SGW = SGA / SGD
    float gridRatio    = static_cast<float>(kGridWidth) / static_cast<float>(kGridDepth);
    float subGridArea  = (kGridWidth * kGridDepth) / static_cast<float>(numObstacles);
    float subGridDepth = std::sqrtf(subGridArea / gridRatio);
    float subGridWidth = subGridArea / subGridDepth;

    ppx::Random random;
    for (int32_t i = 0; i < kNumEntities; ++i) {
        auto& entity = mEntities.emplace_back();

        if (i == 0) {
            // The first object is the mesh plane where all the other entities are placed.
            entity.dimension                = float3(kGridWidth, 0, kGridDepth);
            WireMeshOptions wireMeshOptions = WireMeshOptions().Indices().VertexColors();
            WireMesh        wireMesh        = WireMesh::CreatePlane(WIRE_MESH_PLANE_POSITIVE_Y, float2(kGridWidth, kGridDepth), 100, 100, wireMeshOptions);
            SetupEntity(wireMesh, geometryOptions, &entity);
            entity.location = float3(0, 0, 0);
            entity.kind     = FLOOR;
        }
        else {
            TriMesh  triMesh;
            uint32_t distribution = random.UInt32() % 100;

            // NOTE: TriMeshOptions added here must match the number of bindings when creating this entity's pipeline.
            // See the handling of different entities in ProjApp::SetupPipelines.
            if (distribution <= 60) {
                entity.dimension       = float3(2, 2, 2);
                TriMeshOptions options = TriMeshOptions().Indices().VertexColors();
                triMesh                = (distribution <= 30) ? TriMesh::CreateCube(entity.dimension, options) : TriMesh::CreateSphere(entity.dimension[0] / 2, 100, 100, options);
                entity.kind            = TRI_MESH;
            }
            else {
                float3         lb      = {0, 0, 0};
                float3         ub      = {1, 1, 1};
                TriMeshOptions options = TriMeshOptions().Indices().ObjectColor(random.Float3(lb, ub));
                triMesh                = TriMesh::CreateFromOBJ(GetAssetPath("basic/models/monkey.obj"), options);
                entity.kind            = OBJECT;
                entity.dimension       = triMesh.GetBoundingBoxMax();
                PPX_LOG_INFO("Object dimension: (" << entity.dimension[0] << ", " << entity.dimension[1] << ", " << entity.dimension[2] << ")");
            }

            SetupEntity(triMesh, geometryOptions, &entity);

            // Compute a random location for this object.  The location is computed within the boundaries of
            // the object's home grid.
            entity.Place(i - 1, random, int2(kGridWidth, kGridDepth), int2(subGridWidth, subGridDepth));
        }
    }
}

void ProjApp::SetupDescriptors()
{
    Result                         ppxres         = ppx::SUCCESS;
    grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.uniformBuffer                  = kNumEntities;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

    grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS});
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDescriptorSetLayout));
}

void ProjApp::SetupPipelines()
{
    Result            ppxres   = ppx::SUCCESS;
    std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "VertexColors.vs");
    PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
    grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

    bytecode = LoadShader(GetAssetPath("basic/shaders"), "VertexColors.ps");
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
    gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
    gpCreateInfo.cullMode                           = grfx::CULL_MODE_BACK;
    gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
    gpCreateInfo.depthReadEnable                    = true;
    gpCreateInfo.depthWriteEnable                   = true;
    gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
    gpCreateInfo.outputState.renderTargetCount      = 1;
    gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
    gpCreateInfo.outputState.depthStencilFormat     = GetSwapchain()->GetDepthFormat();
    gpCreateInfo.pPipelineInterface                 = mPipelineInterface;

    for (auto& entity : mEntities) {
        // NOTE: Number of vertex input bindings here must match the number of options added to each entity in ProjApp::SetupEntities.
        if (entity.kind == FLOOR || entity.kind == TRI_MESH) {
            gpCreateInfo.topology                      = (entity.kind == FLOOR) ? grfx::PRIMITIVE_TOPOLOGY_LINE_LIST : grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            gpCreateInfo.vertexInputState.bindingCount = entity.model->GetVertexBufferCount();
            gpCreateInfo.vertexInputState.bindings[0]  = *entity.model->GetVertexBinding(0);
            gpCreateInfo.vertexInputState.bindings[1]  = *entity.model->GetVertexBinding(1);
        }
        else if (entity.kind == OBJECT) {
            gpCreateInfo.topology                      = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            gpCreateInfo.vertexInputState.bindingCount = entity.model->GetVertexBufferCount();
            gpCreateInfo.vertexInputState.bindings[0]  = *entity.model->GetVertexBinding(0);
            gpCreateInfo.vertexInputState.bindings[1]  = *entity.model->GetVertexBinding(1);
        }
        else {
            PPX_ASSERT_MSG(false, "Unrecognized entity kind: " << entity.kind);
        }
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &entity.pipeline));
    }
}

void ProjApp::SetupPerFrameData(void)
{
    Result   ppxres = ppx::SUCCESS;
    PerFrame frame  = {};

    PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd));

    grfx::SemaphoreCreateInfo semaCreateInfo = {};
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore));

    grfx::FenceCreateInfo fenceCreateInfo = {};
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence));

    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore));

    fenceCreateInfo = {true};
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence));

    mPerFrame.push_back(frame);
}

void ProjApp::SetupCamera()
{
    float3 lookAt(0, 0, 1);
    float3 cameraPosition(0, 1, -10);
    mCurrentCamera = &mPerspCamera;
    mRateOfMove    = 2.0;
    UpdateCamera(&mPerspCamera, cameraPosition, lookAt);
    UpdateCamera(&mArcballCamera, cameraPosition, lookAt);
}

void ProjApp::UpdateCamera(PerspCamera* camera, float3 cameraPosition, float3 lookAt)
{
    camera->LookAt(cameraPosition, lookAt, ppx::PPX_CAMERA_DEFAULT_WORLD_UP);
    camera->SetPerspective(60.f, GetWindowAspect());

    PPX_LOG_DEBUG("Camera looking at: (" << camera->GetTarget()[0] << ", " << camera->GetTarget()[1] << ", " << camera->GetTarget()[2] << ")");
    PPX_LOG_DEBUG("Camera position:   (" << camera->GetEyePosition()[0] << ", " << camera->GetEyePosition()[1] << ", " << camera->GetEyePosition()[2] << ")\n");
}

void ProjApp::Setup()
{
    SetupDescriptors();
    SetupEntities();
    SetupPipelines();
    SetupPerFrameData();
    SetupCamera();
}

void ProjApp::MouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons)
{
    float2 prevPos  = GetNormalizedDeviceCoordinates(x - dx, y - dy);
    float2 curPos   = GetNormalizedDeviceCoordinates(x, y);
    float2 deltaPos = prevPos - curPos;
    float3 lookAt   = mCurrentCamera->GetTarget() + float3(deltaPos[0] * 60.0, -deltaPos[1] * 60.0, 0);
    UpdateCamera(mCurrentCamera, mCurrentCamera->GetEyePosition(), lookAt);
}

void ProjApp::KeyDown(KeyCode key)
{
    mPressedKeys.insert(key);
}

void ProjApp::KeyUp(KeyCode key)
{
    mPressedKeys.erase(key);
}

void ProjApp::ProcessInput()
{
    if (mPressedKeys.empty()) {
        return;
    }

    // Process key events every 10ms.
    float msElapsed = GetElapsedSeconds() * 1000.0f;
    if (mLastInputCheck + 10.0 > msElapsed) {
        return;
    }
    mLastInputCheck = msElapsed;

    float3 eyePosition(mCurrentCamera->GetEyePosition());
    float3 lookAt(mCurrentCamera->GetTarget());

    if (mPressedKeys.count(ppx::KEY_W) > 0) {
        mCurrentCamera->MoveAlongViewDirection(mRateOfMove);
        return;
    }
    else if (mPressedKeys.count(ppx::KEY_A) > 0) {
        eyePosition += float3(-mRateOfMove, 0, 0);
    }
    else if (mPressedKeys.count(ppx::KEY_S) > 0) {
        mCurrentCamera->MoveAlongViewDirection(-mRateOfMove);
        return;
    }
    else if (mPressedKeys.count(ppx::KEY_D) > 0) {
        eyePosition += float3(mRateOfMove, 0, 0);
    }
    else if (mPressedKeys.count(ppx::KEY_SPACE) > 0) {
        SetupCamera();
        return;
    }
    else if (mPressedKeys.count(ppx::KEY_1) > 0) {
        mCurrentCamera = &mPerspCamera;
    }
    else if (mPressedKeys.count(ppx::KEY_2) > 0) {
        mCurrentCamera = &mArcballCamera;
    }
    else if (mPressedKeys.count(ppx::KEY_LEFT) > 0) {
        lookAt += float3(mRateOfMove, 0, 0);
    }
    else if (mPressedKeys.count(ppx::KEY_RIGHT) > 0) {
        lookAt += float3(-mRateOfMove, 0, 0);
    }
    else if (mPressedKeys.count(ppx::KEY_UP) > 0) {
        lookAt += float3(0, mRateOfMove, 0);
    }
    else if (mPressedKeys.count(ppx::KEY_DOWN) > 0) {
        lookAt += float3(0, -mRateOfMove, 0);
    }
    else {
        return;
    }

    UpdateCamera(mCurrentCamera, eyePosition, lookAt);
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

    // Update uniform buffers
    {
        ProcessInput();

        const float4x4& P = mCurrentCamera->GetProjectionMatrix();
        const float4x4& V = mCurrentCamera->GetViewMatrix();

        for (auto& entity : mEntities) {
            float4x4 T   = glm::translate(entity.location);
            float4x4 mat = P * V * T;
            entity.uniformBuffer->CopyFromSource(sizeof(mat), &mat);
        }
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

            for (auto& entity : mEntities) {
                frame.cmd->BindGraphicsPipeline(entity.pipeline);
                frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 1, &entity.descriptorSet);
                frame.cmd->BindIndexBuffer(entity.model);
                frame.cmd->BindVertexBuffers(entity.model);
                frame.cmd->DrawIndexed(entity.model->GetIndexCount());
            }

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
