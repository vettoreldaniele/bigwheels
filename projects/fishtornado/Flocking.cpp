#include "Flocking.h"

#include "Config.h"
#include "FishTornado.h"
#include "ppx/graphics_util.h"
#include "ppx/random.h"

#define NUM_THREADS_X 8
#define NUM_THREADS_Y 8

static uint32_t PreviousFrameIndex(uint32_t frameIndex, uint32_t numFrameInFlights)
{
    uint32_t previousFrameIndex = (frameIndex == 0) ? (numFrameInFlights - 1) : (frameIndex)-1;
    return previousFrameIndex;
}

// -------------------------------------------------------------------------------------------------
// Flocking
// -------------------------------------------------------------------------------------------------
Flocking::Flocking()
{
    mResX       = 128; // Gets rounded up to NUM_THREADS_X
    mResY       = 128; // Gets rounded up to NUM_THREADS_Y
    mMinThresh  = 0.55f;
    mMaxThresh  = 0.85f;
    mMinSpeed   = 2.0f; //1.5
    mMaxSpeed   = 6.0f;
    mZoneRadius = 35.0f;
}

Flocking::~Flocking()
{
}

static void FillInitialPositionData(Bitmap* pVelocity, Bitmap* pPosition)
{
    ppx::Random rand;

    for (Bitmap::PixelIterator iter = pPosition->GetPixelIterator(); !iter.Done(); iter.Next()) {
        float4* pPixel = iter.GetPixelAddress<float4>();
        float3  pos    = float3(rand.Float(-200.0f, 200.0f), rand.Float(50.0f, 450.0f), rand.Float(-200.0f, 200.0f));
        pPixel->r      = pos.x;
        pPixel->g      = pos.y;
        pPixel->b      = pos.z;
        pPixel->a      = rand.Float(0.5f, 1.0f);
    }

    Bitmap::PixelIterator posIter = pPosition->GetPixelIterator();
    Bitmap::PixelIterator velIter = pVelocity->GetPixelIterator();
    for (; posIter.Next() && velIter.Next();) {
        float4*     pPos = posIter.GetPixelAddress<float4>();
        float4*     pVel = velIter.GetPixelAddress<float4>();
        const float s    = 0.1f;
        pPos->r -= s * pVel->r;
        pPos->g -= s * pVel->g;
        pPos->b -= s * pVel->b;
    }
}

static void FillInitialVelocityData(Bitmap* pPosition)
{
    const float PI          = ppx::pi<float>();
    const float numFlockers = static_cast<float>(pPosition->GetWidth() * pPosition->GetHeight());
    const float azimuth     = 64.0f * PI / numFlockers;
    const float inclination = PI / numFlockers;
    const float radius      = 0.1f;

    int i = 0;
    for (Bitmap::PixelIterator iter = pPosition->GetPixelIterator(); !iter.Done(); iter.Next(), ++i) {
        float4* pPixel = iter.GetPixelAddress<float4>();
        pPixel->r      = radius * sin(inclination * (float)i) * cos(azimuth * i);
        pPixel->g      = radius * cos(inclination * (float)i);
        pPixel->b      = radius * sin(inclination * (float)i) * sin(azimuth * i);
        pPixel->a      = 1.0f;
    }
}

void Flocking::SetupSetLayouts()
{
    Result          ppxres = ppx::ERROR_FAILED;
    FishTornadoApp* pApp   = FishTornadoApp::GetThisApp();
    grfx::DevicePtr device = pApp->GetDevice();

    // See FlockingPosition.hlsl
    //
    grfx::DescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_FLOCKING_DATA_REGISTER, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});            // b0
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE}); // t1
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});  // t2
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_OUTPUT_POSITION_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE});   // u3
    PPX_CHECKED_CALL(ppxres = device->CreateDescriptorSetLayout(&createInfo, &mFlockingPositionSetLayout));

    // See FlockingVelocity.hlsl
    //
    createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_FLOCKING_DATA_REGISTER, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});            // b0
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE}); // t1
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_PREVIOUS_VELOCITY_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE}); // t2
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_OUTPUT_VELOCITY_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE});   // u3
    PPX_CHECKED_CALL(ppxres = device->CreateDescriptorSetLayout(&createInfo, &mFlockingVelocitySetLayout));

    // See FlockingRender.hlsl
    createInfo = {};
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_FLOCKING_DATA_REGISTER, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER});            // b0
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE}); // t1
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_CURRENT_POSITION_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});  // t2
    createInfo.bindings.push_back(grfx::DescriptorBinding{RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE});  // t3
    PPX_CHECKED_CALL(ppxres = device->CreateDescriptorSetLayout(&createInfo, &mRenderSetLayout));
}

void Flocking::SetupSets()
{
    Result                       ppxres         = ppx::ERROR_FAILED;
    FishTornadoApp*              pApp           = FishTornadoApp::GetThisApp();
    grfx::DevicePtr              device         = pApp->GetDevice();
    grfx::DescriptorPoolPtr      pool           = pApp->GetDescriptorPool();
    grfx::DescriptorSetLayoutPtr modelSetLayout = pApp->GetModelDataSetLayout();

    for (size_t i = 0; i < mPerFrame.size(); ++i) {
        uint32_t  frameIndex     = static_cast<uint32_t>(i);
        uint32_t  prevFrameIndex = PreviousFrameIndex(frameIndex, pApp->GetNumFramesInFlight());
        PerFrame& frame          = mPerFrame[frameIndex];
        PerFrame& prevFrame      = mPerFrame[prevFrameIndex];

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, modelSetLayout, &frame.modelSet));
        frame.modelSet->UpdateUniformBuffer(RENDER_MODEL_DATA_REGISTER, 0, frame.modelConstants.GetGpuBuffer());

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, mFlockingPositionSetLayout, &frame.positionSet));
        PPX_CHECKED_CALL(ppxres = frame.positionSet->UpdateUniformBuffer(RENDER_FLOCKING_DATA_REGISTER, 0, frame.flockingConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = frame.positionSet->UpdateSampledImage(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, 0, prevFrame.positionTexture));
        PPX_CHECKED_CALL(ppxres = frame.positionSet->UpdateSampledImage(RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER, 0, frame.velocityTexture));
        PPX_CHECKED_CALL(ppxres = frame.positionSet->UpdateStorageImage(RENDER_OUTPUT_POSITION_TEXTURE_REGISTER, 0, frame.positionTexture));

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, mFlockingVelocitySetLayout, &frame.velocitySet));
        PPX_CHECKED_CALL(ppxres = frame.velocitySet->UpdateUniformBuffer(RENDER_FLOCKING_DATA_REGISTER, 0, frame.flockingConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = frame.velocitySet->UpdateSampledImage(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, 0, prevFrame.positionTexture));
        PPX_CHECKED_CALL(ppxres = frame.velocitySet->UpdateSampledImage(RENDER_PREVIOUS_VELOCITY_TEXTURE_REGISTER, 0, prevFrame.velocityTexture));
        PPX_CHECKED_CALL(ppxres = frame.velocitySet->UpdateStorageImage(RENDER_OUTPUT_VELOCITY_TEXTURE_REGISTER, 0, frame.velocityTexture));

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, mRenderSetLayout, &frame.renderSet));
        PPX_CHECKED_CALL(ppxres = frame.renderSet->UpdateUniformBuffer(RENDER_FLOCKING_DATA_REGISTER, 0, frame.flockingConstants.GetGpuBuffer()));
        PPX_CHECKED_CALL(ppxres = frame.renderSet->UpdateSampledImage(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, 0, prevFrame.positionTexture));
        PPX_CHECKED_CALL(ppxres = frame.renderSet->UpdateSampledImage(RENDER_CURRENT_POSITION_TEXTURE_REGISTER, 0, frame.positionTexture));
        PPX_CHECKED_CALL(ppxres = frame.renderSet->UpdateSampledImage(RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER, 0, frame.velocityTexture));
    }

    PPX_CHECKED_CALL(ppxres = mMaterialConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));

    PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, pApp->GetMaterialSetLayout(), &mMaterialSet));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateUniformBuffer(RENDER_MATERIAL_DATA_REGISTER, 0, mMaterialConstants.GetGpuBuffer()));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateSampledImage(RENDER_ALBEDO_TEXTURE_REGISTER, 0, mAlbedoTexture));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateSampledImage(RENDER_ROUGHNESS_TEXTURE_REGISTER, 0, mRoughnessTexture));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateSampledImage(RENDER_NORMAL_MAP_TEXTURE_REGISTER, 0, mNormalMapTexture));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateSampledImage(RENDER_CAUSTICS_TEXTURE_REGISTER, 0, pApp->GetCausticsTexture()));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateSampler(RENDER_CLAMPED_SAMPLER_REGISTER, 0, pApp->GetClampedSampler()));
    PPX_CHECKED_CALL(ppxres = mMaterialSet->UpdateSampler(RENDER_REPEAT_SAMPLER_REGISTER, 0, pApp->GetRepeatSampler()));
}

void Flocking::SetupPipelineInterfaces()
{
    ppx::Result     ppxres = ppx::ERROR_FAILED;
    FishTornadoApp* pApp   = FishTornadoApp::GetThisApp();
    grfx::DevicePtr device = pApp->GetDevice();

    // [set0] : scene resources
    // [set1] : model resources
    // [set2] : material resources
    // [set3] : flocking resources
    //
    grfx::PipelineInterfaceCreateInfo createInfo = {};
    createInfo.setCount                          = 4;
    createInfo.sets[0].set                       = 0;
    createInfo.sets[0].pLayout                   = pApp->GetSceneDataSetLayout();
    createInfo.sets[1].set                       = 1;
    createInfo.sets[1].pLayout                   = pApp->GetModelDataSetLayout();
    createInfo.sets[2].set                       = 2;
    createInfo.sets[2].pLayout                   = pApp->GetMaterialSetLayout();
    createInfo.sets[3].set                       = 3;
    createInfo.sets[3].pLayout                   = mRenderSetLayout;
    PPX_CHECKED_CALL(ppxres = device->CreatePipelineInterface(&createInfo, &mForwardPipelineInterface));

    // [set0] : resources for position and velocity calculations
    //
    createInfo                 = {};
    createInfo.setCount        = 1;
    createInfo.sets[0].set     = 0;
    createInfo.sets[0].pLayout = mFlockingPositionSetLayout;
    PPX_CHECKED_CALL(ppxres = device->CreatePipelineInterface(&createInfo, &mFlockingPositionPipelineInterface));

    // [set0] : resources for position and velocity calculations
    //
    createInfo                 = {};
    createInfo.setCount        = 1;
    createInfo.sets[0].set     = 0;
    createInfo.sets[0].pLayout = mFlockingVelocitySetLayout;
    PPX_CHECKED_CALL(ppxres = device->CreatePipelineInterface(&createInfo, &mFlockingVelocityPipelineInterface));
}

void Flocking::SetupPipelines()
{
    ppx::Result     ppxres = ppx::ERROR_FAILED;
    FishTornadoApp* pApp   = FishTornadoApp::GetThisApp();
    grfx::DevicePtr device = pApp->GetDevice();

    // Flocking position
    {
        grfx::ShaderModulePtr CS;
        PPX_CHECKED_CALL(ppxres = pApp->CreateShader(pApp->GetAssetPath("fishtornado/shaders"), "FlockingPosition.cs", &CS));

        grfx::ComputePipelineCreateInfo createInfo = {};
        createInfo.CS                              = {CS, "csmain"};
        createInfo.pPipelineInterface              = mFlockingPositionPipelineInterface;
        PPX_CHECKED_CALL(ppxres = device->CreateComputePipeline(&createInfo, &mFlockingPositionPipeline));

        device->DestroyShaderModule(CS);
    }

    // Flocking velocity
    {
        grfx::ShaderModulePtr CS;
        PPX_CHECKED_CALL(ppxres = pApp->CreateShader(pApp->GetAssetPath("fishtornado/shaders"), "FlockingVelocity.cs", &CS));

        grfx::ComputePipelineCreateInfo createInfo = {};
        createInfo.CS                              = {CS, "csmain"};
        createInfo.pPipelineInterface              = mFlockingVelocityPipelineInterface;
        PPX_CHECKED_CALL(ppxres = device->CreateComputePipeline(&createInfo, &mFlockingVelocityPipeline));

        device->DestroyShaderModule(CS);
    }

    // Foward
    mForwardPipeline = pApp->CreateForwardPipeline(pApp->GetAssetPath("fishtornado/shaders"), "FlockingRender.vs", "FlockingRender.ps", mForwardPipelineInterface);

    // Shadow
    mShadowPipeline = pApp->CreateShadowPipeline(pApp->GetAssetPath("fishtornado/shaders"), "FlockingShadow.vs", mForwardPipelineInterface);
}

void Flocking::Setup(uint32_t numFramesInFlight)
{
    ppx::Result             ppxres = ppx::ERROR_FAILED;
    FishTornadoApp*         pApp   = FishTornadoApp::GetThisApp();
    grfx::DevicePtr         device = pApp->GetDevice();
    grfx::QueuePtr          queue  = pApp->GetGraphicsQueue();
    grfx::DescriptorPoolPtr pool   = pApp->GetDescriptorPool();

    // Round up resolution to nearest NUM_THREADS_X and NUM_THREADS_Y
    mResX = RoundUp<uint32_t>(mResX, NUM_THREADS_X);
    mResY = RoundUp<uint32_t>(mResX, NUM_THREADS_Y);

    // Fill initial data for velocity texture
    Bitmap velocityData = Bitmap::Create(mResX, mResY, ppx::Bitmap::FORMAT_RGBA_FLOAT);
    FillInitialVelocityData(&velocityData);

    // Fill initial data for position texture
    Bitmap positionData = Bitmap::Create(mResX, mResY, ppx::Bitmap::FORMAT_RGBA_FLOAT);
    FillInitialPositionData(&velocityData, &positionData);

    // Create layouts, interfaces, and pipelines
    SetupSetLayouts();
    SetupPipelineInterfaces();
    SetupPipelines();

    // Per frame
    mPerFrame.resize(numFramesInFlight);
    for (uint32_t i = 0; i < numFramesInFlight; ++i) {
        PerFrame& frame = mPerFrame[i];
        PPX_CHECKED_CALL(ppxres = frame.modelConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));
        PPX_CHECKED_CALL(ppxres = frame.flockingConstants.Create(device, PPX_MINIUM_CONSTANT_BUFFER_SIZE));

        grfx_util::TextureOptions textureOptions = grfx_util::TextureOptions().AdditionalUsage(grfx::IMAGE_USAGE_STORAGE).MipLevelCount(1);
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTextureFromBitmap(queue, &positionData, &frame.positionTexture, textureOptions));
        PPX_CHECKED_CALL(ppxres = grfx_util::CreateTextureFromBitmap(queue, &velocityData, &frame.velocityTexture, textureOptions));

        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, mFlockingPositionSetLayout, &frame.positionSet));
        PPX_CHECKED_CALL(ppxres = device->AllocateDescriptorSet(pool, mFlockingVelocitySetLayout, &frame.velocitySet));
    }

    // Create model
    TriMeshOptions options = TriMeshOptions().Indices().AllAttributes().InvertTexCoordsV().InvertWinding();
    PPX_CHECKED_CALL(ppxres = grfx_util::CreateModelFromFile(queue, pApp->GetAssetPath("fishtornado/models/trevallie/trevallie.obj"), &mModel, options));

    // Create textures
#if defined(PPX_GGP) && (PPX_D3D12)
    grfx_util::TextureOptions textureOptions = grfx_util::TextureOptions().MipLevelCount(1);
#else
    grfx_util::TextureOptions textureOptions = grfx_util::TextureOptions().MipLevelCount(PPX_REMAINING_MIP_LEVELS);
#endif
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/trevallie/trevallieDiffuse.png"), &mAlbedoTexture, textureOptions));
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/trevallie/trevallieRoughness.png"), &mRoughnessTexture, textureOptions));
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(queue, pApp->GetAssetPath("fishtornado/textures/trevallie/trevallieNormal.png"), &mNormalMapTexture, textureOptions));

    // Descriptor sets
    SetupSets();
}

void Flocking::Shutdown()
{
    grfx::DevicePtr device = FishTornadoApp::GetThisApp()->GetDevice();

    for (size_t i = 0; i < mPerFrame.size(); ++i) {
        PerFrame& frame = mPerFrame[i];
        frame.modelConstants.Destroy();
        frame.flockingConstants.Destroy();
        device->DestroyTexture(frame.positionTexture);
        device->DestroyTexture(frame.velocityTexture);
    }

    mMaterialConstants.Destroy();
}

void Flocking::Update(uint32_t frameIndex)
{
    FishTornadoApp* pApp  = FishTornadoApp::GetThisApp();
    const float     t     = pApp->GetTime();
    const float     dt    = pApp->GetDt();
    PerFrame&       frame = mPerFrame[frameIndex];

    // Write to CPU constants buffers
    {
        PerFrame& frame = mPerFrame[frameIndex];

        hlsl::ModelData* pModelData = static_cast<hlsl::ModelData*>(frame.modelConstants.GetMappedAddress());
        pModelData->modelMatrix     = float4x4(1);
        pModelData->mormalMatrix    = float4x4(1);

        hlsl::FlockingData* pFlockingData = static_cast<hlsl::FlockingData*>(frame.flockingConstants.GetMappedAddress());
        pFlockingData->resX               = static_cast<int>(mResX);
        pFlockingData->resY               = static_cast<int>(mResY);
        pFlockingData->minThresh          = mMinThresh;
        pFlockingData->maxThresh          = mMaxThresh;
        pFlockingData->minSpeed           = mMinSpeed;
        pFlockingData->maxSpeed           = mMaxSpeed;
        pFlockingData->zoneRadius         = mZoneRadius;
        pFlockingData->time               = t;
        pFlockingData->timeDelta          = dt;
        pFlockingData->predPos            = pApp->GetShark()->GetPosition();
        pFlockingData->camPos             = pApp->GetCamera()->GetEyePosition();
    }
}

void Flocking::CopyConstantsToGpu(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    PerFrame& frame = mPerFrame[frameIndex];

    // Model constants
    {
        pCmd->BufferResourceBarrier(frame.modelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

        grfx::BufferToBufferCopyInfo copyInfo = {};
        copyInfo.size                         = frame.modelConstants.GetSize();

        pCmd->CopyBufferToBuffer(
            &copyInfo,
            frame.modelConstants.GetCpuBuffer(),
            frame.modelConstants.GetGpuBuffer());

        pCmd->BufferResourceBarrier(frame.modelConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
    }

    // Flocking constants
    {
        pCmd->BufferResourceBarrier(frame.flockingConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_COPY_DST);

        grfx::BufferToBufferCopyInfo copyInfo = {};
        copyInfo.size                         = frame.flockingConstants.GetSize();

        pCmd->CopyBufferToBuffer(
            &copyInfo,
            frame.flockingConstants.GetCpuBuffer(),
            frame.flockingConstants.GetGpuBuffer());

        pCmd->BufferResourceBarrier(frame.flockingConstants.GetGpuBuffer(), grfx::RESOURCE_STATE_COPY_DST, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
    }
}

void Flocking::Compute(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    uint32_t prevFrameIndex = PreviousFrameIndex(frameIndex, FishTornadoApp::GetThisApp()->GetNumFramesInFlight());

    uint32_t groupCountX = mResX / NUM_THREADS_X;
    uint32_t groupCountY = mResY / NUM_THREADS_Y;
    uint32_t groupCountZ = 1;

    PerFrame& frame     = mPerFrame[frameIndex];
    PerFrame& prevFrame = mPerFrame[prevFrameIndex];

    // Velocity
    {
        pCmd->TransitionImageLayout(frame.velocityTexture, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_SHADER_RESOURCE, grfx::RESOURCE_STATE_GENERAL);

        pCmd->BindComputeDescriptorSets(mFlockingVelocityPipelineInterface, 1, &frame.velocitySet);
        pCmd->BindComputePipeline(mFlockingVelocityPipeline);
        pCmd->Dispatch(groupCountX, groupCountY, groupCountZ);

        pCmd->TransitionImageLayout(frame.velocityTexture, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_GENERAL, grfx::RESOURCE_STATE_SHADER_RESOURCE);
    }

    // Position
    {
        pCmd->TransitionImageLayout(frame.positionTexture, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_SHADER_RESOURCE, grfx::RESOURCE_STATE_GENERAL);
    
        pCmd->BindComputeDescriptorSets(mFlockingPositionPipelineInterface, 1, &frame.positionSet);
        pCmd->BindComputePipeline(mFlockingPositionPipeline);
        pCmd->Dispatch(groupCountX, groupCountY, groupCountZ);
    
        pCmd->TransitionImageLayout(frame.positionTexture, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_GENERAL, grfx::RESOURCE_STATE_SHADER_RESOURCE);
    }
}

void Flocking::DrawDebug(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
}

void Flocking::DrawShadow(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    FishTornadoApp* pApp = FishTornadoApp::GetThisApp();

    PerFrame& frame = mPerFrame[frameIndex];

    grfx::DescriptorSet* sets[4] = {nullptr};
    sets[0]                      = pApp->GetSceneShadowSet(frameIndex);
    sets[1]                      = frame.modelSet;
    sets[2]                      = mMaterialSet;
    sets[3]                      = frame.renderSet;

    pCmd->BindGraphicsDescriptorSets(mForwardPipelineInterface, 4, sets);

    pCmd->BindGraphicsPipeline(mShadowPipeline);

    pCmd->BindIndexBuffer(mModel);
    pCmd->BindVertexBuffers(mModel);
    pCmd->DrawIndexed(mModel->GetIndexCount(), mResX * mResY);
}

void Flocking::DrawForward(uint32_t frameIndex, grfx::CommandBuffer* pCmd)
{
    PerFrame& frame = mPerFrame[frameIndex];

    grfx::DescriptorSet* sets[4] = {nullptr};
    sets[0]                      = FishTornadoApp::GetThisApp()->GetSceneSet(frameIndex);
    sets[1]                      = frame.modelSet;
    sets[2]                      = mMaterialSet;
    sets[3]                      = frame.renderSet;

    pCmd->BindGraphicsDescriptorSets(mForwardPipelineInterface, 4, sets);

    pCmd->BindGraphicsPipeline(mForwardPipeline);

    pCmd->BindIndexBuffer(mModel);
    pCmd->BindVertexBuffers(mModel);
    pCmd->DrawIndexed(mModel->GetIndexCount(), mResX * mResY);
}
