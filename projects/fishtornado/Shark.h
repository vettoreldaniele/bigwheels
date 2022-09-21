#ifndef SHARK_H
#define SHARK_H

#include "ppx/grfx/grfx_descriptor.h"
#include "ppx/grfx/grfx_mesh.h"
using namespace ppx;

#include "Buffer.h"

class FishTornadoApp;

class Shark
{
public:
    Shark();
    ~Shark();

    const float3& GetPosition() const { return mPos; }

    void Setup(uint32_t numFramesInFlight);
    void Shutdown();
    void Update(uint32_t frameIndex);
    void CopyConstantsToGpu(uint32_t frameIndex, grfx::CommandBuffer* pCmd);
    void DrawDebug(uint32_t frameIndex, grfx::CommandBuffer* pCmd);
    void DrawShadow(uint32_t frameIndex, grfx::CommandBuffer* pCmd);
    void DrawForward(uint32_t frameIndex, grfx::CommandBuffer* pCmd);

private:
    struct PerFrame
    {
        ConstantBuffer         modelConstants;
        grfx::DescriptorSetPtr modelSet;
    };

    std::vector<PerFrame>     mPerFrame;
    ConstantBuffer            mMaterialConstants;
    grfx::DescriptorSetPtr    mMaterialSet;
    grfx::GraphicsPipelinePtr mForwardPipeline;
    grfx::GraphicsPipelinePtr mShadowPipeline;
    grfx::MeshPtr             mMesh;
    grfx::TexturePtr          mAlbedoTexture;
    grfx::TexturePtr          mRoughnessTexture;
    grfx::TexturePtr          mNormalMapTexture;

    float3 mPos = float3(3000.0f, 100.0f, 0.0f);
    float3 mVel;
    float3 mDir;
};

#endif // SHARK_H
