#include "Shark.h"
#include "FishTornado.h"
#include "ShaderConfig.h"
#include "ppx/graphics_util.h"

Shark::Shark()
{
}

Shark::~Shark()
{
}

void Shark::Setup(uint32_t numFramesInFlight)
{
    ppx::Result                  ppxres         = ppx::ERROR_FAILED;
    FishTornadoApp*              pApp           = FishTornadoApp::GetThisApp();
    grfx::DevicePtr              device         = pApp->GetDevice();
    grfx::QueuePtr               queue          = pApp->GetGraphicsQueue();
    grfx::DescriptorPoolPtr      pool           = pApp->GetDescriptorPool();
    grfx::DescriptorSetLayoutPtr modelSetLayout = pApp->GetModelDataSetLayout();

    mPerFrame.resize(numFramesInFlight);
    for (uint32_t i = 0; i < numFramesInFlight; ++i) {
        PerFrame& frame = mPerFrame[i];

        PPX_CHECKED_CALL(ppxres = mPerFrame[i].modelConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));

        // Allocate descriptor set
        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, modelSetLayout, &frame.modelSet));

        // Update descriptor
        PPX_CHECKED_CALL(ppxres = frame.modelSet->UpdateUniformBuffer(0, 0, frame.modelConstants.GetGpuBuffer()));
    }

    mForwardPipeline = pApp->CreateForwardPipeline(pApp->GetAssetPath("fishtornado/shaders"), "Shark.vs", "Shark.ps");

    TriMesh::Options options = TriMesh::Options().Indices().AllAttributes().InvertTexCoordsV().InvertWinding();;
    PPX_CHECKED_CALL(ppxres = CreateModelFromFile(queue, pApp->GetAssetPath("fishtornado/models/shark/shark.obj"), &mModel, options));

    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/shark/sharkDiffuse.png"), &mFloorAlbedoTexture));
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/shark/sharkRoughness.png"), &mFloorRoughnessTexture));
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/shark/sharkNormal.png"), &mFloorNormalMapTexture));

    PPX_CHECKED_CALL(ppxres = mFloorMaterialConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));

    PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, pApp->GetMaterialSetLayout(), &mFloorMaterialSet));
    PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateUniformBuffer(0, 0, mFloorMaterialConstants.GetGpuBuffer()));
    PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(1, 0, mFloorAlbedoTexture));
    PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(2, 0, mFloorRoughnessTexture));
    PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(3, 0, mFloorNormalMapTexture));    
    PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampler(4, 0, pApp->GetClampedSampler()));
}

void Shark::Shutdown()
{
    for (size_t i = 0; i < mPerFrame.size(); ++i) {
        mPerFrame[i].modelConstants.Destroy();
    }

    mFloorMaterialConstants.Destroy();
}

void Shark::Update(uint32_t frameIndex)
{
    const float t = FishTornadoApp::GetThisApp()->GetTime();

    // Calculate position
    float3 prevPos = mPos;
    mPos.x         = sin(t * -0.0205f) * 100.0f;
    mPos.z         = sin(t * -0.0205f) * 900.0f; // 800.0f
    mPos.y         = 100.0f;
    //mPos.y         = 100.0f * 0.675f + (sin(t * 0.00125f) * 0.5f + 0.5f) * 100.0f * 0.25f;

    // Calculate velocity
    mVel = mPos - prevPos;

    // Find direction of travel
    mDir = normalize(mVel);

    // Calculate rotation matrix for orientation
    quat     q           = glm::rotation(float3(0, 0, 1), mDir);
    float4x4 rotMat      = glm::toMat4(q);
    float4x4 modelMatrix = glm::translate(mPos) * rotMat;

    // Write to CPU constants buffer
    {
        PerFrame& frame = mPerFrame[frameIndex];

        hlsl::ModelData* pData = static_cast<hlsl::ModelData*>(frame.modelConstants.GetMappedAddress());
        pData->modelMatrix     = modelMatrix;
    }
}

void Shark::CopyConstantsToGpu(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    PerFrame& frame = mPerFrame[frameIndex];

    pCmd->BufferResourceBarrier(frame.modelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

    grfx::BufferToBufferCopyInfo copyInfo = {};
    copyInfo.size                         = frame.modelConstants.GetSize();

    pCmd->CopyBufferToBuffer(
        &copyInfo,
        frame.modelConstants.GetCpuBuffer(),
        frame.modelConstants.GetGpuBuffer());

    pCmd->BufferResourceBarrier(frame.modelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
}

void Shark::DrawDebug(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    grfx::PipelineInterfacePtr pipelineInterface = FishTornadoApp::GetThisApp()->GetForwardPipelineInterface();
    grfx::GraphicsPipelinePtr  pipeline          = FishTornadoApp::GetThisApp()->GetDebugDrawPipeline();

    grfx::DescriptorSet* sets[2] = {nullptr};
    sets[0]                      = FishTornadoApp::GetThisApp()->GetSceneSet(frameIndex);
    sets[1]                      = mPerFrame[frameIndex].modelSet;

    pCmd->BindGraphicsDescriptorSets(pipelineInterface, 2, sets);

    pCmd->BindGraphicsPipeline(pipeline);

    pCmd->BindIndexBuffer(mModel);
    pCmd->BindVertexBuffers(mModel);
    pCmd->DrawIndexed(mModel->GetIndexCount());
}

void Shark::DrawForward(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    grfx::PipelineInterfacePtr pipelineInterface = FishTornadoApp::GetThisApp()->GetForwardPipelineInterface();

    grfx::DescriptorSet* sets[3] = {nullptr};
    sets[0]                      = FishTornadoApp::GetThisApp()->GetSceneSet(frameIndex);
    sets[1]                      = mPerFrame[frameIndex].modelSet;
    sets[2]                      = mFloorMaterialSet;

    pCmd->BindGraphicsDescriptorSets(pipelineInterface, 3, sets);

    pCmd->BindGraphicsPipeline(mForwardPipeline);

    pCmd->BindIndexBuffer(mModel);
    pCmd->BindVertexBuffers(mModel);
    pCmd->DrawIndexed(mModel->GetIndexCount());
}
