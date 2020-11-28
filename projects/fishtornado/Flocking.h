#ifndef FLOCKING_H
#define FLOCKING_H

#include "ppx/grfx/grfx_model.h"
using namespace ppx;

#include "Buffer.h"

class FishTornadoApp;

class Flocking
{
public:
    Flocking();
    ~Flocking();

    void Setup(uint32_t numFramesInFlight);
    void Shutdown();
    void Update(uint32_t frameIndex);
    void CopyConstantsToGpu(uint32_t frameIndex, grfx::CommandBuffer* pCmd);
    void Compute(uint32_t frameIndex, grfx::CommandBuffer* pCmd);
    void DrawDebug(uint32_t frameIndex, grfx::CommandBuffer* pCmd);
    void DrawForward(uint32_t frameIndex, grfx::CommandBuffer* pCmd);

private:
    void SetupSetLayouts();
    void SetupSets();
    void SetupPipelineInterfaces();
    void SetupPipelines();

private:
    struct PerFrame
    {
        ConstantBuffer         modelConstants;
        ConstantBuffer         flockingConstants;
        grfx::TexturePtr       positionTexture;
        grfx::TexturePtr       velocityTexture;
        grfx::DescriptorSetPtr modelSet;
        grfx::DescriptorSetPtr positionSet;
        grfx::DescriptorSetPtr velocitySet;
        grfx::DescriptorSetPtr renderSet;
    };

    uint32_t mResX;
    uint32_t mResY;
    float    mMinThresh;
    float    mMaxThresh;
    float    mMinSpeed;
    float    mMaxSpeed;
    float    mZoneRadius;

    grfx::DescriptorSetLayoutPtr mFlockingSetLayout;
    grfx::PipelineInterfacePtr   mFlockingPipelineInterface;
    grfx::ComputePipelinePtr     mFlockingPositionPipeline;
    grfx::ComputePipelinePtr     mFlockingVelocityPipeline;
    grfx::DescriptorSetLayoutPtr mRenderSetLayout;
    grfx::PipelineInterfacePtr   mForwardPipelineInterface;
    grfx::GraphicsPipelinePtr    mForwardPipeline;
    std::vector<PerFrame>        mPerFrame;
    grfx::ModelPtr               mModel;
    grfx::TexturePtr             mAlbedoTexture;
    grfx::TexturePtr             mRoughnessTexture;
    grfx::TexturePtr             mNormalMapTexture;
};

#endif // FLOCKING_H
