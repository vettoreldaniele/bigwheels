#ifndef FISHTORNADO_H
#define FISHTORNADO_H

#include "ppx/ppx.h"
#include "ppx/camera.h"
using namespace ppx;

#include "Buffer.h"
#include "Flocking.h"
#include "Ocean.h"
#include "ShaderConfig.h"
#include "Shark.h"

#if defined(USE_DX)
constexpr grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
constexpr grfx::Api kApi = grfx::API_VK_1_1;
#endif

class FishTornadoApp
    : public ppx::Application
{
public:
    static FishTornadoApp* GetThisApp();
    float                  GetTime() const { return mTime; }
    float                  GetDt() const { return mDt; }
    const PerspCamera*     GetCamera() const { return &mCamera; }
    const Shark*           GetShark() const { return &mShark; }

    grfx::DescriptorPoolPtr      GetDescriptorPool() const { return mDescriptorPool; }
    grfx::DescriptorSetLayoutPtr GetSceneDataSetLayout() const { return mSceneDataSetLayout; }
    grfx::DescriptorSetLayoutPtr GetModelDataSetLayout() const { return mModelDataSetLayout; }
    grfx::DescriptorSetLayoutPtr GetMaterialSetLayout() const { return mMaterialSetLayout; }
    grfx::DescriptorSetPtr       GetSceneSet(uint32_t frameIndex) const;
    grfx::SamplerPtr             GetClampedSampler() const { return mClampedSampler; }
    grfx::PipelineInterfacePtr   GetForwardPipelineInterface() const { return mForwardPipelineInterface; }
    grfx::GraphicsPipelinePtr    GetDebugDrawPipeline() const { return mDebugDrawPipeline; }

    grfx::GraphicsPipelinePtr CreateForwardPipeline(
        const fs::path&          baseDir,
        const std::string&       vsBaseName,
        const std::string&       psBaseName,
        grfx::PipelineInterface* pPipelineInterface = nullptr);

    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Shutdown() override;
    virtual void Scroll(float dx, float dy) override;
    virtual void Render() override;

private:
    void SetupDescriptorPool();
    void SetupSetLayouts();
    void SetupPipelineInterfaces();
    void SetupPerFrame();
    void SetupSamplers();
    void SetupDebug();
    void SetupScene();
    void UpdateTime();
    void UpdateScene(uint32_t frameIndex);

private:
    struct PerFrame
    {
        grfx::CommandBufferPtr cmd;
        grfx::SemaphorePtr     imageAcquiredSemaphore;
        grfx::FencePtr         imageAcquiredFence;
        grfx::SemaphorePtr     renderCompleteSemaphore;
        grfx::FencePtr         renderCompleteFence;
        ConstantBuffer         sceneConstants;
        grfx::DescriptorSetPtr sceneSet;
    };

    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mSceneDataSetLayout;
    grfx::DescriptorSetLayoutPtr mModelDataSetLayout;
    grfx::DescriptorSetLayoutPtr mMaterialSetLayout;
    std::vector<PerFrame>        mPerFrame;
    grfx::SamplerPtr             mClampedSampler;
    grfx::PipelineInterfacePtr   mForwardPipelineInterface;
    grfx::GraphicsPipelinePtr    mDebugDrawPipeline;
    PerspCamera                  mCamera;
    float                        mTime = 0;
    float                        mDt   = 0;
    Flocking                     mFlocking;
    Ocean                        mOcean;
    Shark                        mShark;
};

#endif // FISHTORNADO_H
