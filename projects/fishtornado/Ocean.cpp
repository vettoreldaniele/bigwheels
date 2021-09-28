#include "Ocean.h"
#include "FishTornado.h"
#include "ppx/graphics_util.h"

Ocean::Ocean()
{
}

Ocean::~Ocean()
{
}

void Ocean::Setup(uint32_t numFramesInFlight)
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

        // Floor
        {
            PPX_CHECKED_CALL(ppxres = mPerFrame[i].floorModelConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));
            PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, modelSetLayout, &frame.floorModelSet));
            PPX_CHECKED_CALL(ppxres = frame.floorModelSet->UpdateUniformBuffer(RENDER_MODEL_DATA_REGISTER, 0, frame.floorModelConstants.GetGpuBuffer()));
        }

        // Surface
        {
            PPX_CHECKED_CALL(ppxres = mPerFrame[i].surfaceModelConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));
            PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, modelSetLayout, &frame.surfaceModelSet));
            PPX_CHECKED_CALL(ppxres = frame.surfaceModelSet->UpdateUniformBuffer(RENDER_MODEL_DATA_REGISTER, 0, frame.surfaceModelConstants.GetGpuBuffer()));
        }

        // Beam
        {
            PPX_CHECKED_CALL(ppxres = mPerFrame[i].beamModelConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));
            PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, modelSetLayout, &frame.beamModelSet));
            PPX_CHECKED_CALL(ppxres = frame.beamModelSet->UpdateUniformBuffer(RENDER_MODEL_DATA_REGISTER, 0, frame.beamModelConstants.GetGpuBuffer()));
        }
    }

    // Floor
    {
        mFloorForwardPipeline = pApp->CreateForwardPipeline(pApp->GetAssetPath("fishtornado/shaders"), "OceanFloor.vs", "OceanFloor.ps");

        TriMeshOptions options = TriMeshOptions().Indices().AllAttributes().TexCoordScale(float2(25.0f));
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromFile(queue, pApp->GetAssetPath("fishtornado/models/ocean/floor_lowRes.obj"), &mFloorModel, options));

        grfx_util::TextureOptions textureOptions = grfx_util::TextureOptions().MipLevelCount(PPX_ALL_MIP_LEVELS);
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/ocean/floorDiffuse.png"), &mFloorAlbedoTexture, textureOptions));
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/ocean/floorRoughness.png"), &mFloorRoughnessTexture, textureOptions));
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/ocean/floorNormal.png"), &mFloorNormalMapTexture, textureOptions));

        PPX_CHECKED_CALL(ppxres = mFloorMaterialConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, pApp->GetMaterialSetLayout(), &mFloorMaterialSet));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateUniformBuffer(RENDER_MATERIAL_DATA_REGISTER, 0, mFloorMaterialConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(RENDER_ALBEDO_TEXTURE_REGISTER, 0, mFloorAlbedoTexture));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(RENDER_ROUGHNESS_TEXTURE_REGISTER, 0, mFloorRoughnessTexture));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(RENDER_NORMAL_MAP_TEXTURE_REGISTER, 0, mFloorNormalMapTexture));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampledImage(RENDER_CAUSTICS_TEXTURE_REGISTER, 0, pApp->GetCausticsTexture()));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampler(RENDER_CLAMPED_SAMPLER_REGISTER, 0, pApp->GetClampedSampler()));
        PPX_CHECKED_CALL(ppxres = mFloorMaterialSet->UpdateSampler(RENDER_REPEAT_SAMPLER_REGISTER, 0, pApp->GetRepeatSampler()));
    }

    // Surface
    {
        mSurfaceForwardPipeline = pApp->CreateForwardPipeline(pApp->GetAssetPath("fishtornado/shaders"), "OceanSurface.vs", "OceanSurface.ps");

        TriMeshOptions options = TriMeshOptions().Indices().AllAttributes().TexCoordScale(float2(1.0f));
        TriMesh        mesh    = TriMesh::CreatePlane(TRI_MESH_PLANE_NEGATIVE_Y, float2(2500.0f), 10, 10, options);
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromTriMesh(queue, &mesh, &mSurfaceModel));

        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTexture1x1(queue, float4(0, 0, 0, 0), &mSurfaceAlbedoTexture));
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTexture1x1(queue, float4(1, 1, 1, 1), &mSurfaceRoughnessTexture));
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/ocean/surfaceNormalMap.png"), &mSurfaceNormalMapTexture));

        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, pApp->GetMaterialSetLayout(), &mSurfaceMaterialSet));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateUniformBuffer(RENDER_MATERIAL_DATA_REGISTER, 0, mSurfaceMaterialConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateSampledImage(RENDER_ALBEDO_TEXTURE_REGISTER, 0, mSurfaceAlbedoTexture));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateSampledImage(RENDER_ROUGHNESS_TEXTURE_REGISTER, 0, mSurfaceRoughnessTexture));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateSampledImage(RENDER_NORMAL_MAP_TEXTURE_REGISTER, 0, mSurfaceNormalMapTexture));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateSampledImage(RENDER_CAUSTICS_TEXTURE_REGISTER, 0, pApp->GetCausticsTexture()));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateSampler(RENDER_CLAMPED_SAMPLER_REGISTER, 0, pApp->GetClampedSampler()));
        PPX_CHECKED_CALL(ppxres = mSurfaceMaterialSet->UpdateSampler(RENDER_REPEAT_SAMPLER_REGISTER, 0, pApp->GetRepeatSampler()));
    }

    // Beam
    {
        {
            grfx::ShaderModulePtr VS, PS;
#if defined(PORTO_D3DCOMPILE)
            PPX_CHECKED_CALL(ppxres = pApp->CompileHlslShader(pApp->GetAssetPath("fishtornado/shaders"), "OceanBeam", "vs_5_0", &VS));
            PPX_CHECKED_CALL(ppxres = pApp->CompileHlslShader(pApp->GetAssetPath("fishtornado/shaders"), "OceanBeam", "ps_5_0", &PS));
#else
            PPX_CHECKED_CALL(ppxres = pApp->CreateShader(pApp->GetAssetPath("fishtornado/shaders"), "OceanBeam.vs", &VS));
            PPX_CHECKED_CALL(ppxres = pApp->CreateShader(pApp->GetAssetPath("fishtornado/shaders"), "OceanBeam.ps", &PS));
#endif

            const grfx::VertexInputRate inputRate = grfx::VERTEX_INPUT_RATE_VERTEX;
            grfx::VertexDescription     vertexDescription;
            vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_POSITION, 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, inputRate});
            vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_NORMAL, 1, grfx::FORMAT_R32G32B32_FLOAT, 1, PPX_APPEND_OFFSET_ALIGNED, inputRate});
            vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_TEXCOORD, 2, grfx::FORMAT_R32G32_FLOAT, 2, PPX_APPEND_OFFSET_ALIGNED, inputRate});

            grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
            gpCreateInfo.VS                                 = {VS, "vsmain"};
            gpCreateInfo.PS                                 = {PS, "psmain"};
            gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
            gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
            gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
            gpCreateInfo.depthReadEnable                    = false;
            gpCreateInfo.depthWriteEnable                   = false;
            gpCreateInfo.blendModes[0]                      = {grfx::BLEND_MODE_ADDITIVE};
            gpCreateInfo.outputState.renderTargetCount      = 1;
            gpCreateInfo.outputState.renderTargetFormats[0] = pApp->GetSwapchain()->GetColorFormat();
            gpCreateInfo.outputState.depthStencilFormat     = pApp->GetSwapchain()->GetDepthFormat();
            gpCreateInfo.pPipelineInterface                 = pApp->GetForwardPipelineInterface();
            // Vertex description
            gpCreateInfo.vertexInputState.bindingCount = vertexDescription.GetBindingCount();
            for (uint32_t i = 0; i < vertexDescription.GetBindingCount(); ++i) {
                gpCreateInfo.vertexInputState.bindings[i] = *vertexDescription.GetBinding(i);
            }

            PPX_CHECKED_CALL(ppxres = device->CreateGraphicsPipeline(&gpCreateInfo, &mBeamForwardPipeline));

            device->DestroyShaderModule(VS);
            device->DestroyShaderModule(PS);
        }

        TriMeshOptions options = TriMeshOptions().Indices().Normals().TexCoords();
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromFile(queue, pApp->GetAssetPath("fishtornado/models/ocean/beams.obj"), &mBeamModel, options));
    }
}

void Ocean::Shutdown()
{
    for (size_t i = 0; i < mPerFrame.size(); ++i) {
        mPerFrame[i].floorModelConstants.Destroy();
        mPerFrame[i].surfaceModelConstants.Destroy();
        mPerFrame[i].beamModelConstants.Destroy();
    }

    mFloorMaterialConstants.Destroy();
    mSurfaceMaterialConstants.Destroy();
}

void Ocean::Update(uint32_t frameIndex)
{
    FishTornadoApp* pApp  = FishTornadoApp::GetThisApp();
    const float     t     = pApp->GetTime();
    const float     dt    = pApp->GetDt();
    PerFrame&       frame = mPerFrame[frameIndex];

    // Write to CPU constants buffers
    {
        PerFrame& frame = mPerFrame[frameIndex];

        hlsl::ModelData* pModelData = static_cast<hlsl::ModelData*>(frame.floorModelConstants.GetMappedAddress());
        pModelData->modelMatrix     = float4x4(1);
        pModelData->mormalMatrix    = float4x4(1);

        pModelData               = static_cast<hlsl::ModelData*>(frame.surfaceModelConstants.GetMappedAddress());
        pModelData->modelMatrix  = glm::translate(float3(0, 350, 0));
        pModelData->mormalMatrix = float4x4(1);

        hlsl::BeamModelData* pBeamModelData = static_cast<hlsl::BeamModelData*>(frame.beamModelConstants.GetMappedAddress());
        pBeamModelData->modelMatrix[0]      = glm::rotate(t * 0.01f, float3(0.0f, 1.0f, 0.0f));
        pBeamModelData->modelMatrix[1]      = glm::rotate(t * -0.022f, float3(0.0f, 1.0f, 0.0f));
    }
}

void Ocean::CopyConstantsToGpu(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    PerFrame& frame = mPerFrame[frameIndex];

    // Floor
    {
        pCmd->BufferResourceBarrier(frame.floorModelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

        grfx::BufferToBufferCopyInfo copyInfo = {};
        copyInfo.size                         = frame.floorModelConstants.GetSize();

        pCmd->CopyBufferToBuffer(
            &copyInfo,
            frame.floorModelConstants.GetCpuBuffer(),
            frame.floorModelConstants.GetGpuBuffer());

        pCmd->BufferResourceBarrier(frame.floorModelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
    }

    // Surface
    {
        pCmd->BufferResourceBarrier(frame.surfaceModelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

        grfx::BufferToBufferCopyInfo copyInfo = {};
        copyInfo.size                         = frame.surfaceModelConstants.GetSize();

        pCmd->CopyBufferToBuffer(
            &copyInfo,
            frame.surfaceModelConstants.GetCpuBuffer(),
            frame.surfaceModelConstants.GetGpuBuffer());

        pCmd->BufferResourceBarrier(frame.surfaceModelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
    }

    // Beam
    {
        pCmd->BufferResourceBarrier(frame.beamModelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

        grfx::BufferToBufferCopyInfo copyInfo = {};
        copyInfo.size                         = frame.beamModelConstants.GetSize();

        pCmd->CopyBufferToBuffer(
            &copyInfo,
            frame.beamModelConstants.GetCpuBuffer(),
            frame.beamModelConstants.GetGpuBuffer());

        pCmd->BufferResourceBarrier(frame.beamModelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
    }
}

void Ocean::DrawForward(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    grfx::PipelineInterfacePtr pipelineInterface = FishTornadoApp::GetThisApp()->GetForwardPipelineInterface();

    // Floor
    {
        grfx::DescriptorSet* sets[3] = {nullptr};
        sets[0]                      = FishTornadoApp::GetThisApp()->GetSceneSet(frameIndex);
        sets[1]                      = mPerFrame[frameIndex].floorModelSet;
        sets[2]                      = mFloorMaterialSet;

        pCmd->BindGraphicsDescriptorSets(pipelineInterface, 3, sets);

        pCmd->BindGraphicsPipeline(mFloorForwardPipeline);

        pCmd->BindIndexBuffer(mFloorModel);
        pCmd->BindVertexBuffers(mFloorModel);
        pCmd->DrawIndexed(mFloorModel->GetIndexCount());
    }

    // Surface
    {
        grfx::DescriptorSet* sets[3] = {nullptr};
        sets[0]                      = FishTornadoApp::GetThisApp()->GetSceneSet(frameIndex);
        sets[1]                      = mPerFrame[frameIndex].surfaceModelSet;
        sets[2]                      = mSurfaceMaterialSet;

        pCmd->BindGraphicsDescriptorSets(pipelineInterface, 3, sets);

        pCmd->BindGraphicsPipeline(mSurfaceForwardPipeline);

        pCmd->BindIndexBuffer(mSurfaceModel);
        pCmd->BindVertexBuffers(mSurfaceModel);
        pCmd->DrawIndexed(mSurfaceModel->GetIndexCount());
    }

    // Beam
    {
        grfx::DescriptorSet* sets[2] = {nullptr};
        sets[0]                      = FishTornadoApp::GetThisApp()->GetSceneSet(frameIndex);
        sets[1]                      = mPerFrame[frameIndex].beamModelSet;

        pCmd->BindGraphicsDescriptorSets(pipelineInterface, 2, sets);

        pCmd->BindGraphicsPipeline(mBeamForwardPipeline);

        pCmd->BindIndexBuffer(mBeamModel);
        pCmd->BindVertexBuffers(mBeamModel);
        pCmd->DrawIndexed(mBeamModel->GetIndexCount(), 1);
    }
}
