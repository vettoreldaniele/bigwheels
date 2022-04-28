#include "ppx/ppx.h"
#include "ppx/camera.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#if defined(PORTO_D3DCOMPILE)
#include "ppx/grfx/dx/d3dcompile_util.h"
#endif

#include "Entity.h"
#include "Material.h"
#include "Render.h"

// *** NOTE ***
//
// Pipeline queries do not work on DXIIVK yet.
//
#if !defined(PPX_DXIIVK)
#define ENABLE_GPU_QUERIES
#endif

#if defined(USE_DX11)
const grfx::Api kApi = grfx::API_DX_11_1;
#elif defined(USE_DX12)
const grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
const grfx::Api kApi = grfx::API_VK_1_1;
#endif

bool gUpdateOnce = false;

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void MouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons) override;
    virtual void Shutdown() override;
    virtual void Render() override;

private:
    struct PerFrame
    {
        ppx::grfx::CommandBufferPtr cmd;
        ppx::grfx::SemaphorePtr     imageAcquiredSemaphore;
        ppx::grfx::FencePtr         imageAcquiredFence;
        ppx::grfx::SemaphorePtr     renderCompleteSemaphore;
        ppx::grfx::FencePtr         renderCompleteFence;
#ifdef ENABLE_GPU_QUERIES
        ppx::grfx::QueryPtr         timestampQuery;
        ppx::grfx::QueryPtr         pipelineStatsQuery;
#endif
    };

    grfx::PipelineStatistics mPipelineStatistics = {};
    uint64_t                 mTotalGpuFrameTime  = 0;

    std::vector<PerFrame>        mPerFrame;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    PerspCamera                  mCamera;
    grfx::DescriptorSetLayoutPtr mSceneDataLayout;
    grfx::DescriptorSetPtr       mSceneDataSet;
    grfx::BufferPtr              mCpuSceneConstants;
    grfx::BufferPtr              mGpuSceneConstants;
    grfx::BufferPtr              mCpuLightConstants;
    grfx::BufferPtr              mGpuLightConstants;

    grfx::SamplerPtr mSampler;

    grfx::ModelPtr                mIBLModel;
    grfx::DescriptorSetLayoutPtr  mIBLLayout;
    grfx::DescriptorSetPtr        mIBLSet;
    grfx::BufferPtr               mIBLConstants;
    std::vector<grfx::TexturePtr> mIBLMaps;
    std::vector<grfx::TexturePtr> mIBLEnvMaps;
    grfx::PipelineInterfacePtr    mIBLPipelineInterface;
    grfx::GraphicsPipelinePtr     mIBLPipeline;

    grfx::DrawPassPtr            mGBufferRenderPass;
    grfx::TexturePtr             mGBufferLightRenderTarget;
    grfx::DrawPassPtr            mGBufferLightPass;
    grfx::DescriptorSetLayoutPtr mGBufferReadLayout;
    grfx::DescriptorSetPtr       mGBufferReadSet;
    grfx::BufferPtr              mGBufferDrawAttrConstants;
    bool                         mEnableIBL = true;
    bool                         mEnableEnv = true;
    grfx::FullscreenQuadPtr      mGBufferLightQuad;
    grfx::FullscreenQuadPtr      mDebugDrawQuad;

    grfx::DescriptorSetLayoutPtr mDrawToSwapchainLayout;
    grfx::DescriptorSetPtr       mDrawToSwapchainSet;
    grfx::FullscreenQuadPtr      mDrawToSwapchain;

    grfx::ModelPtr      mSphere;
    grfx::ModelPtr      mBox;
    std::vector<Entity> mEntities;

    float mCamSwing       = 0;
    float mTargetCamSwing = 0;

    bool               mDrawGBufferAttr  = false;
    uint32_t           mGBufferAttrIndex = 0;
    std::vector<const char*> mGBufferAttrNames = {
        "POSITION",
        "NORMAL",
        "ALBEDO",
        "F0",
        "ROUGHNESS",
        "METALNESS",
        "AMB_OCC",
        "IBL_STRENGTH",
        "ENV_STRENGTH",
    };

    uint32_t           mCurrentIBLIndex = UINT32_MAX;
    uint32_t           mTargetIBLIndex  = 0;
    std::vector<const char*> mIBLNames        = {
        "GoldenHour",
        "PaperMill",
        "UenoShrine",
        "TokyoBigSight",
        "TropicalBeach",
    };

private:
    void SetupPerFrame();
    void SetupEntities();
    void SetupIBLResources();
    void SetupGBufferPasses();
    void SetupGBufferLightQuad();
    void SetupDebugDraw();
    void SetupDrawToSwapchain();
    void UpdateConstants();
    void UpdateEnvDescriptors();
    void DrawGui();
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "gbuffer";
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
#if defined(USE_DXVK_SPV)
    settings.grfx.enableDXVKSPV = true;
#endif
}

void ProjApp::SetupPerFrame()
{
    // Per frame data
    {
        PerFrame frame = {};

        PPX_CHECKED_CALL(GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd));

        grfx::SemaphoreCreateInfo semaCreateInfo = {};
        PPX_CHECKED_CALL(GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore));

        grfx::FenceCreateInfo fenceCreateInfo = {};
        PPX_CHECKED_CALL(GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence));

        PPX_CHECKED_CALL(GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore));

        fenceCreateInfo = {true}; // Create signaled
        PPX_CHECKED_CALL(GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence));

#ifdef ENABLE_GPU_QUERIES
        grfx::QueryCreateInfo queryCreateInfo = {};
        queryCreateInfo.type                  = grfx::QUERY_TYPE_TIMESTAMP;
        queryCreateInfo.count                 = 2;
        PPX_CHECKED_CALL(GetDevice()->CreateQuery(&queryCreateInfo, &frame.timestampQuery));

        // Pipeline statistics query pool
        if (GetDevice()->PipelineStatsAvailable()) {
            queryCreateInfo       = {};
            queryCreateInfo.type  = grfx::QUERY_TYPE_PIPELINE_STATISTICS;
            queryCreateInfo.count = 1;
            PPX_CHECKED_CALL(GetDevice()->CreateQuery(&queryCreateInfo, &frame.pipelineStatsQuery));
        }
#endif

        mPerFrame.push_back(frame);
    }
}

void ProjApp::SetupEntities()
{
    TriMeshOptions options = TriMeshOptions().Indices().Normals().VertexColors().TexCoords().Tangents();
    TriMesh        mesh    = TriMesh::CreateSphere(1.0f, 128, 64, options);
    Geometry       geo;
    PPX_CHECKED_CALL(Geometry::Create(mesh, &geo));
    PPX_CHECKED_CALL(grfx_util::CreateModelFromGeometry(GetGraphicsQueue(), &geo, &mSphere));

    mEntities.resize(7);

    std::vector<Material*> materials;
    materials.push_back(Material::GetMaterialRustedIron());
    materials.push_back(Material::GetMaterialPaintedMetal());
    materials.push_back(Material::GetMaterialCopper());
    materials.push_back(Material::GetMaterialGold());
    materials.push_back(Material::GetMaterialTitanium());
    materials.push_back(Material::GetMaterialWhiteRoughPlastic());

    size_t n = 6;
    for (size_t i = 0; i < n; ++i) {
        size_t materialIndex = i % materials.size();

        EntityCreateInfo createInfo = {};
        createInfo.pModel           = mSphere;
        createInfo.pMaterial        = materials[materialIndex];
        PPX_CHECKED_CALL(mEntities[i].Create(GetGraphicsQueue(), mDescriptorPool, &createInfo));

        float t = i / static_cast<float>(n) * 2.0f * 3.141592f;
        float r = 3.0f;
        float x = r * cos(t);
        float y = 1.0f;
        float z = r * sin(t);
        mEntities[i].GetTransform().SetTranslation(float3(x, y, z));
    }

    // Box
    {
        Entity* pEntity = &mEntities[n];

        mesh = TriMesh::CreateCube(float3(10, 1, 10), TriMeshOptions(options).TexCoordScale(float2(5)));
        PPX_CHECKED_CALL(Geometry::Create(mesh, &geo));
        PPX_CHECKED_CALL(grfx_util::CreateModelFromGeometry(GetGraphicsQueue(), &geo, &mBox));

        EntityCreateInfo createInfo = {};
        createInfo.pModel           = mBox;
        createInfo.pMaterial        = Material::GetMaterialStoneTile();
        PPX_CHECKED_CALL(pEntity->Create(GetGraphicsQueue(), mDescriptorPool, &createInfo));
        pEntity->GetTransform().SetTranslation(float3(0, -0.5f, 0));
    }
}

void ProjApp::SetupIBLResources()
{
    Geometry geo;
    TriMesh  mesh = TriMesh::CreateSphere(32, 32, 16, TriMeshOptions().Indices().TexCoords());
    PPX_CHECKED_CALL(Geometry::Create(mesh, &geo));
    PPX_CHECKED_CALL(grfx_util::CreateModelFromGeometry(GetGraphicsQueue(), &geo, &mIBLModel));

    // Layout - the binding values here map to the Texture shader (Texture.hlsl)
    //
    grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
    layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(2, grfx::DESCRIPTOR_TYPE_SAMPLER));
    PPX_CHECKED_CALL(GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mIBLLayout));

    // Uniform buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mIBLConstants));
    }

    // Texture create options
    grfx_util::TextureOptions textureOptions = grfx_util::TextureOptions().MipLevelCount(PPX_REMAINING_MIP_LEVELS);

    // GoldenHour
    {
        grfx::TexturePtr env;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/GoldenHour/ibl.hdr"), &env, textureOptions));
        mIBLMaps.push_back(env);

        grfx::TexturePtr refl;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/GoldenHour/env.hdr"), &refl, textureOptions));
        mIBLEnvMaps.push_back(refl);
    }

    // PaperMill
    {
        grfx::TexturePtr env;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/PaperMill/ibl.hdr"), &env, textureOptions));
        mIBLMaps.push_back(env);

        grfx::TexturePtr refl;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/PaperMill/env.hdr"), &refl, textureOptions));
        mIBLEnvMaps.push_back(refl);
    }

    // UenoShrine
    {
        grfx::TexturePtr env;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/UenoShrine/ibl.hdr"), &env, textureOptions));
        mIBLMaps.push_back(env);

        grfx::TexturePtr refl;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/UenoShrine/env.hdr"), &refl, textureOptions));
        mIBLEnvMaps.push_back(refl);
    }

    // TokyoBigSight
    {
        grfx::TexturePtr env;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/TokyoBigSight/ibl.hdr"), &env, textureOptions));
        mIBLMaps.push_back(env);

        grfx::TexturePtr refl;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/TokyoBigSight/env.hdr"), &refl, textureOptions));
        mIBLEnvMaps.push_back(refl);
    }

    // TropicalBeach
    {
        grfx::TexturePtr env;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/TropicalBeach/ibl.hdr"), &env, textureOptions));
        mIBLMaps.push_back(env);

        grfx::TexturePtr refl;
        PPX_CHECKED_CALL(grfx_util::CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/TropicalBeach/env.hdr"), &refl, textureOptions));
        mIBLEnvMaps.push_back(refl);
    }

    // Allocate descriptor set
    PPX_CHECKED_CALL(GetDevice()->AllocateDescriptorSet(mDescriptorPool, mIBLLayout, &mIBLSet));
    mIBLSet->SetName("IBL");

    // Update IBL render descriptors - just constant buffer and sampler for
    // now. We'll update the image later.
    //
    {
        grfx::WriteDescriptor writes[2] = {};
        writes[0].binding               = 0;
        writes[0].type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].bufferOffset          = 0;
        writes[0].bufferRange           = PPX_WHOLE_SIZE;
        writes[0].pBuffer               = mIBLConstants;
        writes[1].binding               = 2;
        writes[1].type                  = grfx::DESCRIPTOR_TYPE_SAMPLER;
        writes[1].pSampler              = mSampler;
        PPX_CHECKED_CALL(mIBLSet->UpdateDescriptors(2, writes));
    }

    // Pipeline
    {
        grfx::ShaderModulePtr VS;
#if defined(PORTO_D3DCOMPILE)
        grfx::dx::ShaderIncludeHandler basicShaderIncludeHandler(
            GetAssetPath("basic/shaders"));
        std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "Texture", "vs_5_0", &basicShaderIncludeHandler);
#else
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "Texture.vs");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        grfx::ShaderModulePtr PS;
#if defined(PORTO_D3DCOMPILE)
        bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "Texture", "ps_5_0", &basicShaderIncludeHandler);
#else
        bytecode                   = LoadShader(GetAssetPath("basic/shaders"), "Texture.ps");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mIBLLayout;
        PPX_CHECKED_CALL(GetDevice()->CreatePipelineInterface(&piCreateInfo, &mIBLPipelineInterface));

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {VS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {PS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = mIBLModel->GetVertexBufferCount();
        gpCreateInfo.vertexInputState.bindings[0]       = *mIBLModel->GetVertexBinding(0);
        gpCreateInfo.vertexInputState.bindings[1]       = *mIBLModel->GetVertexBinding(1);
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_FRONT;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthReadEnable                    = true;
        gpCreateInfo.depthWriteEnable                   = false;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = mGBufferLightPass->GetRenderTargetTexture(0)->GeImageFormat();
        gpCreateInfo.outputState.depthStencilFormat     = mGBufferLightPass->GetDepthStencilTexture()->GeImageFormat();
        gpCreateInfo.pPipelineInterface                 = mIBLPipelineInterface;
        PPX_CHECKED_CALL(GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mIBLPipeline));
        GetDevice()->DestroyShaderModule(VS);
        GetDevice()->DestroyShaderModule(PS);
    }
}

void ProjApp::SetupGBufferPasses()
{
    // GBuffer render draw pass
    {
        // Usage flags for render target and depth stencil will automatically
        // be added during create. So we only need to specify the additional
        // usage flags here.
        //
        grfx::ImageUsageFlags        additionalUsageFlags = grfx::IMAGE_USAGE_SAMPLED;
        grfx::RenderTargetClearValue rtvClearValue        = {0, 0, 0, 0};
        grfx::DepthStencilClearValue dsvClearValue        = {1.0f, 0xFF};

        grfx::DrawPassCreateInfo createInfo     = {};
        createInfo.width                        = GetWindowWidth();
        createInfo.height                       = GetWindowHeight();
        createInfo.renderTargetCount            = 4;
        createInfo.renderTargetFormats[0]       = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.renderTargetFormats[1]       = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.renderTargetFormats[2]       = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.renderTargetFormats[3]       = grfx::FORMAT_R16G16B16A16_FLOAT;
        createInfo.depthStencilFormat           = grfx::FORMAT_D32_FLOAT;
        createInfo.renderTargetUsageFlags[0]    = additionalUsageFlags;
        createInfo.renderTargetUsageFlags[1]    = additionalUsageFlags;
        createInfo.renderTargetUsageFlags[2]    = additionalUsageFlags;
        createInfo.renderTargetUsageFlags[3]    = additionalUsageFlags;
        createInfo.depthStencilUsageFlags       = additionalUsageFlags;
        createInfo.renderTargetInitialStates[0] = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.renderTargetInitialStates[1] = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.renderTargetInitialStates[2] = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.renderTargetInitialStates[3] = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.depthStencilInitialState     = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.renderTargetClearValues[0]   = rtvClearValue;
        createInfo.renderTargetClearValues[1]   = rtvClearValue;
        createInfo.renderTargetClearValues[2]   = rtvClearValue;
        createInfo.renderTargetClearValues[3]   = rtvClearValue;
        createInfo.depthStencilClearValue       = dsvClearValue;

        PPX_CHECKED_CALL(GetDevice()->CreateDrawPass(&createInfo, &mGBufferRenderPass));
    }

    // GBuffer light render target
    {
        grfx::TextureCreateInfo createInfo         = {};
        createInfo.imageType                       = grfx::IMAGE_TYPE_2D;
        createInfo.width                           = mGBufferRenderPass->GetWidth();
        createInfo.height                          = mGBufferRenderPass->GetHeight();
        createInfo.depth                           = 1;
        createInfo.imageFormat                     = grfx::FORMAT_R8G8B8A8_UNORM;
        createInfo.sampleCount                     = grfx::SAMPLE_COUNT_1;
        createInfo.mipLevelCount                   = 1;
        createInfo.arrayLayerCount                 = 1;
        createInfo.usageFlags.bits.colorAttachment = true;
        createInfo.usageFlags.bits.sampled         = true;
        createInfo.memoryUsage                     = grfx::MEMORY_USAGE_GPU_ONLY;
        createInfo.initialState                    = grfx::RESOURCE_STATE_SHADER_RESOURCE;
        createInfo.RTVClearValue                   = {0, 0, 0, 0};
        createInfo.DSVClearValue                   = {1.0f, 0xFF}; // Not used - we won't write to depth

        PPX_CHECKED_CALL(GetDevice()->CreateTexture(&createInfo, &mGBufferLightRenderTarget));
    }

    // GBuffer light draw pass
    {
        grfx::DrawPassCreateInfo3 createInfo = {};
        createInfo.width                     = mGBufferRenderPass->GetWidth();
        createInfo.height                    = mGBufferRenderPass->GetHeight();
        createInfo.renderTargetCount         = 1;
        createInfo.pRenderTargetTextures[0]  = mGBufferLightRenderTarget;
        createInfo.pDepthStencilTexture      = mGBufferRenderPass->GetDepthStencilTexture();
        createInfo.depthStencilState         = grfx::RESOURCE_STATE_DEPTH_STENCIL_READ;

        PPX_CHECKED_CALL(GetDevice()->CreateDrawPass(&createInfo, &mGBufferLightPass));
    }
}

void ProjApp::SetupGBufferLightQuad()
{
    grfx::ShaderModulePtr VS;
#if defined(PORTO_D3DCOMPILE)
    grfx::dx::ShaderIncludeHandler gbufferShaderIncludeHandler(
        GetAssetPath("gbuffer/shaders"));
    std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("gbuffer/shaders"), "DeferredLight", "vs_5_0", &gbufferShaderIncludeHandler);
#else
    std::vector<char> bytecode = LoadShader(GetAssetPath("gbuffer/shaders"), "DeferredLight.vs");
#endif
    PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
    grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
    PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

    grfx::ShaderModulePtr PS;
#if defined(PORTO_D3DCOMPILE)
    bytecode = grfx::dx::CompileShader(GetAssetPath("gbuffer/shaders"), "DeferredLight", "ps_5_0", &gbufferShaderIncludeHandler);
#else
    bytecode                   = LoadShader(GetAssetPath("gbuffer/shaders"), "DeferredLight.ps");
#endif
    PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
    shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
    PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

    grfx::FullscreenQuadCreateInfo createInfo = {};
    createInfo.VS                             = VS;
    createInfo.PS                             = PS;
    createInfo.setCount                       = 2;
    createInfo.sets[0].set                    = 0;
    createInfo.sets[0].pLayout                = mSceneDataLayout;
    createInfo.sets[1].set                    = 1;
    createInfo.sets[1].pLayout                = mGBufferReadLayout;
    createInfo.renderTargetCount              = 1;
    createInfo.renderTargetFormats[0]         = mGBufferLightPass->GetRenderTargetTexture(0)->GeImageFormat();
    createInfo.depthStencilFormat             = mGBufferLightPass->GetDepthStencilTexture()->GeImageFormat();

    PPX_CHECKED_CALL(GetDevice()->CreateFullscreenQuad(&createInfo, &mGBufferLightQuad));
}

void ProjApp::SetupDebugDraw()
{
    grfx::ShaderModulePtr VS;
#if defined(PORTO_D3DCOMPILE)
    grfx::dx::ShaderIncludeHandler gbufferShaderIncludeHandler(
        GetAssetPath("gbuffer/shaders"));
    std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("gbuffer/shaders"), "DrawGBufferAttribute", "vs_5_0", &gbufferShaderIncludeHandler);
#else
    std::vector<char> bytecode = LoadShader(GetAssetPath("gbuffer/shaders"), "DrawGBufferAttribute.vs");
#endif
    PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
    grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
    PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

    grfx::ShaderModulePtr PS;
#if defined(PORTO_D3DCOMPILE)
    bytecode = grfx::dx::CompileShader(GetAssetPath("gbuffer/shaders"), "DrawGBufferAttribute", "ps_5_0", &gbufferShaderIncludeHandler);
#else
    bytecode                   = LoadShader(GetAssetPath("gbuffer/shaders"), "DrawGBufferAttribute.ps");
#endif
    PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
    shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
    PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

    grfx::FullscreenQuadCreateInfo createInfo = {};
    createInfo.VS                             = VS;
    createInfo.PS                             = PS;
    createInfo.setCount                       = 2;
    createInfo.sets[0].set                    = 0;
    createInfo.sets[0].pLayout                = mSceneDataLayout;
    createInfo.sets[1].set                    = 1;
    createInfo.sets[1].pLayout                = mGBufferReadLayout;
    createInfo.renderTargetCount              = 1;
    createInfo.renderTargetFormats[0]         = mGBufferLightPass->GetRenderTargetTexture(0)->GeImageFormat();
    createInfo.depthStencilFormat             = mGBufferLightPass->GetDepthStencilTexture()->GeImageFormat();

    PPX_CHECKED_CALL(GetDevice()->CreateFullscreenQuad(&createInfo, &mDebugDrawQuad));
}

void ProjApp::SetupDrawToSwapchain()
{
    // Descriptor set layout
    {
        // Descriptor set layout
        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLER));
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDrawToSwapchainLayout));
    }

    // Pipeline
    {
        grfx::ShaderModulePtr VS;
#if defined(PORTO_D3DCOMPILE)
        grfx::dx::ShaderIncludeHandler basicShaderIncludeHandler(
            GetAssetPath("basic/shaders"));
        std::vector<char> bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "FullScreenTriangle", "vs_5_0", &basicShaderIncludeHandler);
#else
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "FullScreenTriangle.vs");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        grfx::ShaderModulePtr PS;
#if defined(PORTO_D3DCOMPILE)
        bytecode = grfx::dx::CompileShader(GetAssetPath("basic/shaders"), "FullScreenTriangle", "ps_5_0", &basicShaderIncludeHandler);
#else
        bytecode                   = LoadShader(GetAssetPath("basic/shaders"), "FullScreenTriangle.ps");
#endif
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        grfx::FullscreenQuadCreateInfo createInfo = {};
        createInfo.VS                             = VS;
        createInfo.PS                             = PS;
        createInfo.setCount                       = 1;
        createInfo.sets[0].set                    = 0;
        createInfo.sets[0].pLayout                = mDrawToSwapchainLayout;
        createInfo.renderTargetCount              = 1;
        createInfo.renderTargetFormats[0]         = GetSwapchain()->GetColorFormat();
        createInfo.depthStencilFormat             = GetSwapchain()->GetDepthFormat();

        PPX_CHECKED_CALL(GetDevice()->CreateFullscreenQuad(&createInfo, &mDrawToSwapchain));
    }

    // Allocate descriptor set
    PPX_CHECKED_CALL(GetDevice()->AllocateDescriptorSet(mDescriptorPool, mDrawToSwapchainLayout, &mDrawToSwapchainSet));

    // Write descriptors
    {
        grfx::WriteDescriptor writes[2] = {};
        writes[0].binding               = 0;
        writes[0].arrayIndex            = 0;
        writes[0].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[0].pImageView            = mGBufferLightPass->GetRenderTargetTexture(0)->GetSampledImageView();

        writes[1].binding  = 1;
        writes[1].type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
        writes[1].pSampler = mSampler;

        PPX_CHECKED_CALL(mDrawToSwapchainSet->UpdateDescriptors(2, writes));
    }
}

void ProjApp::Setup()
{
    // Cameras
    {
        mCamera = PerspCamera(60.0f, GetWindowAspect());
    }

    // Create descriptor pool
    {
        grfx::DescriptorPoolCreateInfo createInfo = {};
        createInfo.sampler                        = 1000;
        createInfo.sampledImage                   = 1000;
        createInfo.uniformBuffer                  = 1000;
        createInfo.structuredBuffer               = 1000;

        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorPool(&createInfo, &mDescriptorPool));
    }

    // Sampler
    {
        grfx::SamplerCreateInfo createInfo = {};
        createInfo.magFilter               = grfx::FILTER_LINEAR;
        createInfo.minFilter               = grfx::FILTER_LINEAR;
        createInfo.mipmapMode              = grfx::SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.minLod                  = 0.0f;
        createInfo.maxLod                  = FLT_MAX;
        PPX_CHECKED_CALL(GetDevice()->CreateSampler(&createInfo, &mSampler));
    }

    // GBuffer passes
    SetupGBufferPasses();

    // GBuffer attribute selection buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mGBufferDrawAttrConstants));
    }

    // GBuffer read
    {
        // clang-format off
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_RT0_REGISTER,       grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE,  1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_RT1_REGISTER,       grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE,  1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_RT2_REGISTER,       grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE,  1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_RT3_REGISTER,       grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE,  1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_ENV_REGISTER,       grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE,  1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_IBL_REGISTER,       grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE,  1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_SAMPLER_REGISTER,   grfx::DESCRIPTOR_TYPE_SAMPLER,        1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{GBUFFER_CONSTANTS_REGISTER, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorSetLayout(&createInfo, &mGBufferReadLayout));
        // clang-format on

        // Allocate descriptor set
        PPX_CHECKED_CALL(GetDevice()->AllocateDescriptorSet(mDescriptorPool, mGBufferReadLayout, &mGBufferReadSet));
        mGBufferReadSet->SetName("GBuffer Read");

        // Write descriptors
        grfx::WriteDescriptor writes[6] = {};
        writes[0].binding               = GBUFFER_RT0_REGISTER;
        writes[0].arrayIndex            = 0;
        writes[0].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[0].pImageView            = mGBufferRenderPass->GetRenderTargetTexture(0)->GetSampledImageView();
        writes[1].binding               = GBUFFER_RT1_REGISTER;
        writes[1].arrayIndex            = 0;
        writes[1].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[1].pImageView            = mGBufferRenderPass->GetRenderTargetTexture(1)->GetSampledImageView();
        writes[2].binding               = GBUFFER_RT2_REGISTER;
        writes[2].arrayIndex            = 0;
        writes[2].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[2].pImageView            = mGBufferRenderPass->GetRenderTargetTexture(2)->GetSampledImageView();
        writes[3].binding               = GBUFFER_RT3_REGISTER;
        writes[3].arrayIndex            = 0;
        writes[3].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[3].pImageView            = mGBufferRenderPass->GetRenderTargetTexture(3)->GetSampledImageView();
        writes[4].binding               = GBUFFER_SAMPLER_REGISTER;
        writes[4].type                  = grfx::DESCRIPTOR_TYPE_SAMPLER;
        writes[4].pSampler              = mSampler;
        writes[5].binding               = GBUFFER_CONSTANTS_REGISTER;
        writes[5].type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[5].bufferOffset          = 0;
        writes[5].bufferRange           = PPX_WHOLE_SIZE;
        writes[5].pBuffer               = mGBufferDrawAttrConstants;

        PPX_CHECKED_CALL(mGBufferReadSet->UpdateDescriptors(6, writes));
    }

    // Create per frame objects
    SetupPerFrame();

    // Scene data
    {
        // Scene constants
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = PPX_MINIUM_CONSTANT_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mCpuSceneConstants));

        bufferCreateInfo.usageFlags.bits.transferDst   = true;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mGpuSceneConstants));

        // Light constants
        bufferCreateInfo                             = {};
        bufferCreateInfo.size                        = PPX_MINIUM_STRUCTURED_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        bufferCreateInfo.structuredElementStride     = 32;
        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mCpuLightConstants));

        bufferCreateInfo.structuredElementStride          = 32;
        bufferCreateInfo.usageFlags.bits.transferDst      = true;
        bufferCreateInfo.usageFlags.bits.structuredBuffer = true;
        bufferCreateInfo.memoryUsage                      = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mGpuLightConstants));

        // Descriptor set layout
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{SCENE_CONSTANTS_REGISTER, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{LIGHT_DATA_REGISTER, grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorSetLayout(&createInfo, &mSceneDataLayout));

        // Allocate descriptor set
        PPX_CHECKED_CALL(GetDevice()->AllocateDescriptorSet(mDescriptorPool, mSceneDataLayout, &mSceneDataSet));
        mSceneDataSet->SetName("Scene Data");

        // Update descriptor
        grfx::WriteDescriptor writes[2] = {};
        writes[0].binding               = SCENE_CONSTANTS_REGISTER;
        writes[0].type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].bufferOffset          = 0;
        writes[0].bufferRange           = PPX_WHOLE_SIZE;
        writes[0].pBuffer               = mGpuSceneConstants;

        writes[1].binding                = LIGHT_DATA_REGISTER;
        writes[1].arrayIndex             = 0;
        writes[1].type                   = grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER;
        writes[1].bufferOffset           = 0;
        writes[1].bufferRange            = PPX_WHOLE_SIZE;
        writes[1].structuredElementCount = 1;
        writes[1].pBuffer                = mGpuLightConstants;
        PPX_CHECKED_CALL(mSceneDataSet->UpdateDescriptors(2, writes));
    }

    // Create materials
    PPX_CHECKED_CALL(Material::CreateMaterials(GetGraphicsQueue(), mDescriptorPool));

    // Create pipelines
    PPX_CHECKED_CALL(Entity::CreatePipelines(mSceneDataLayout, mGBufferRenderPass));

    // Entities
    SetupEntities();

    // Setup IBL resources
    SetupIBLResources();

    // Setup GBuffer lighting
    SetupGBufferLightQuad();

    // Setup fullscreen quad for debug
    SetupDebugDraw();

    // Setup fullscreen quad to draw to swapchain
    SetupDrawToSwapchain();
}

void ProjApp::Shutdown()
{
}

void ProjApp::MouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons)
{
    if (buttons & ppx::MOUSE_BUTTON_LEFT) {
        mTargetCamSwing += 0.25f * dx;
    }
}

void ProjApp::UpdateConstants()
{
    const float kPi = 3.1451592f;

    // Scene constants
    {
        mCamSwing += (mTargetCamSwing - mCamSwing) * 0.1f;

        float t = glm::radians(mCamSwing - kPi / 2.0f);
        float x = 8.0f * cos(t);
        float z = 8.0f * sin(t);
        mCamera.LookAt(float3(x, 3, z), float3(0, 0.5f, 0));

        PPX_HLSL_PACK_BEGIN();
        struct HlslSceneData
        {
            hlsl_uint<4>      frameNumber;
            hlsl_float<12>    time;
            hlsl_float4x4<64> viewProjectionMatrix;
            hlsl_float3<12>   eyePosition;
            hlsl_uint<4>      lightCount;
            hlsl_float<4>     ambient;
            hlsl_float<4>     iblLevelCount;
            hlsl_float<4>     envLevelCount;
        };
        PPX_HLSL_PACK_END();

        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(mCpuSceneConstants->MapMemory(0, &pMappedAddress));

        HlslSceneData* pSceneData        = static_cast<HlslSceneData*>(pMappedAddress);
        pSceneData->viewProjectionMatrix = mCamera.GetViewProjectionMatrix();
        pSceneData->eyePosition          = mCamera.GetEyePosition();
        pSceneData->lightCount           = 5;
        pSceneData->ambient              = 0.0f;
        pSceneData->iblLevelCount        = static_cast<float>(mIBLMaps[mTargetIBLIndex]->GetMipLevelCount());
        pSceneData->envLevelCount        = static_cast<float>(mIBLEnvMaps[mTargetIBLIndex]->GetMipLevelCount());

        mCpuSceneConstants->UnmapMemory();

        grfx::BufferToBufferCopyInfo copyInfo = {mCpuSceneConstants->GetSize()};
        PPX_CHECKED_CALL(GetGraphicsQueue()->CopyBufferToBuffer(&copyInfo, mCpuSceneConstants, mGpuSceneConstants, grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_CONSTANT_BUFFER));
    }

    // Light constants
    {
        PPX_HLSL_PACK_BEGIN();
        struct HlslLight
        {
            hlsl_uint<4>    type;
            hlsl_float3<12> position;
            hlsl_float3<12> color;
            hlsl_float<4>   intensity;
        };
        PPX_HLSL_PACK_END();

        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(mCpuLightConstants->MapMemory(0, &pMappedAddress));

        float t = GetElapsedSeconds();

        HlslLight* pLight  = static_cast<HlslLight*>(pMappedAddress);
        pLight[0].position = float3(10, 5, 10) * float3(sin(t), 1, cos(t));
        pLight[1].position = float3(-10, 2, 5) * float3(cos(t), 1, sin(t / 4 + kPi / 2));
        pLight[2].position = float3(1, 10, 3) * float3(sin(t / 2), 1, cos(t / 2));
        pLight[3].position = float3(-1, 0, 15) * float3(sin(t / 3), 1, cos(t / 3));
        pLight[4].position = float3(-1, 2, -5) * float3(sin(t / 4), 1, cos(t / 4));
        pLight[5].position = float3(-6, 3, -4) * float3(sin(t / 5), 1, cos(t / 5));

        mCpuLightConstants->UnmapMemory();

        grfx::BufferToBufferCopyInfo copyInfo = {mCpuLightConstants->GetSize()};
        GetGraphicsQueue()->CopyBufferToBuffer(&copyInfo, mCpuLightConstants, mGpuLightConstants, grfx::RESOURCE_STATE_CONSTANT_BUFFER, grfx::RESOURCE_STATE_CONSTANT_BUFFER);
    }

    // Model constants
    for (size_t i = 0; i < mEntities.size(); ++i) {
        mEntities[i].UpdateConstants(GetGraphicsQueue());
    }

    // GBuffer constants
    {
        PPX_HLSL_PACK_BEGIN();
        struct HlslGBufferData
        {
            hlsl_uint<4> enableIBL;
            hlsl_uint<4> enableEnv;
            hlsl_uint<4> debugAttrIndex;
        };
        PPX_HLSL_PACK_END();

        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(mGBufferDrawAttrConstants->MapMemory(0, &pMappedAddress));

        HlslGBufferData* pGBufferData = static_cast<HlslGBufferData*>(pMappedAddress);

        pGBufferData->enableIBL      = mEnableIBL;
        pGBufferData->enableEnv      = mEnableEnv;
        pGBufferData->debugAttrIndex = mGBufferAttrIndex;

        mGBufferDrawAttrConstants->UnmapMemory();
    }

    // Update IBL environment constants
    {
        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(mIBLConstants->MapMemory(0, &pMappedAddress));

        const float4x4& VP  = mCamera.GetViewProjectionMatrix();
        float4x4        M   = glm::rotate(0.0f, float3(0, 1, 0));
        float4x4        mat = VP * M;
        memcpy(pMappedAddress, &mat, sizeof(mat));

        mIBLConstants->UnmapMemory();
    }
}

void ProjApp::UpdateEnvDescriptors()
{
    if (mCurrentIBLIndex != mTargetIBLIndex) {
        mCurrentIBLIndex = mTargetIBLIndex;

        // Update GBUffer descriptors
        {
            // Update descriptor set
            grfx::WriteDescriptor writes[2] = {};
            writes[0].binding               = GBUFFER_ENV_REGISTER;
            writes[0].arrayIndex            = 0;
            writes[0].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            writes[0].pImageView            = mIBLMaps[mCurrentIBLIndex]->GetSampledImageView();
            writes[1].binding               = GBUFFER_IBL_REGISTER;
            writes[1].arrayIndex            = 0;
            writes[1].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            writes[1].pImageView            = mIBLEnvMaps[mCurrentIBLIndex]->GetSampledImageView();

            PPX_CHECKED_CALL(mGBufferReadSet->UpdateDescriptors(2, writes));
        }

        // Update IBL render descriptors
        {
            grfx::WriteDescriptor write = {};
            write.binding               = 0;
            write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.bufferOffset          = 0;
            write.bufferRange           = PPX_WHOLE_SIZE;
            write.pBuffer               = mIBLConstants;
            PPX_CHECKED_CALL(mIBLSet->UpdateDescriptors(1, &write));

            write            = {};
            write.binding    = 1;
            write.type       = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write.pImageView = mIBLEnvMaps[mCurrentIBLIndex]->GetSampledImageView();
            PPX_CHECKED_CALL(mIBLSet->UpdateDescriptors(1, &write));
        }
    }
}

void ProjApp::Render()
{
    PerFrame& frame  = mPerFrame[0];

    grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(frame.imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(frame.renderCompleteFence->WaitAndReset());

    // Update constants
    UpdateConstants();

    // Update descriptors
    UpdateEnvDescriptors();

#ifdef ENABLE_GPU_QUERIES
    // Read query results
    if (GetFrameCount() > 0) {
        uint64_t data[2] = {0};
        PPX_CHECKED_CALL(frame.timestampQuery->GetData(data, 2 * sizeof(uint64_t)));
        mTotalGpuFrameTime = data[1] - data[0];
        if (GetDevice()->PipelineStatsAvailable()) {
            PPX_CHECKED_CALL(frame.pipelineStatsQuery->GetData(&mPipelineStatistics, sizeof(grfx::PipelineStatistics)));
        }
    }

    // Reset query
    frame.timestampQuery->Reset(0, 2);
    if (GetDevice()->PipelineStatsAvailable()) {
        frame.pipelineStatsQuery->Reset(0, 1);
    }
#endif

    // Build command buffer
    PPX_CHECKED_CALL(frame.cmd->Begin());
    {
        frame.cmd->SetScissors(mGBufferRenderPass->GetScissor());
        frame.cmd->SetViewports(mGBufferRenderPass->GetViewport());

        // =====================================================================
        //  GBuffer render
        // =====================================================================
        frame.cmd->TransitionImageLayout(
            mGBufferRenderPass,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_RENDER_TARGET,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE);
        frame.cmd->BeginRenderPass(mGBufferRenderPass, grfx::DRAW_PASS_CLEAR_FLAG_CLEAR_RENDER_TARGETS | grfx::DRAW_PASS_CLEAR_FLAG_CLEAR_DEPTH);
        {
#ifdef ENABLE_GPU_QUERIES
            frame.cmd->WriteTimestamp(frame.timestampQuery, grfx::PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0);
#endif

#ifdef ENABLE_GPU_QUERIES
            if (GetDevice()->PipelineStatsAvailable()) {
                frame.cmd->BeginQuery(frame.pipelineStatsQuery, 0);
            }
#endif
            for (size_t i = 0; i < mEntities.size(); ++i) {
                mEntities[i].Draw(mSceneDataSet, frame.cmd);
            }
#ifdef ENABLE_GPU_QUERIES
            if (GetDevice()->PipelineStatsAvailable()) {
                frame.cmd->EndQuery(frame.pipelineStatsQuery, 0);
            }
#endif
        }
        frame.cmd->EndRenderPass();
        frame.cmd->TransitionImageLayout(
            mGBufferRenderPass,
            grfx::RESOURCE_STATE_RENDER_TARGET,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE,
            grfx::RESOURCE_STATE_SHADER_RESOURCE);

        // =====================================================================
        //  GBuffer light
        // =====================================================================
        frame.cmd->TransitionImageLayout(
            mGBufferLightPass,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_RENDER_TARGET,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_DEPTH_STENCIL_READ);
        frame.cmd->BeginRenderPass(mGBufferLightPass, grfx::DRAW_PASS_CLEAR_FLAG_CLEAR_RENDER_TARGETS);
        {
            // Light scene using gbuffer data
            //
            grfx::DescriptorSet* sets[2] = {nullptr};
            sets[0]                      = mSceneDataSet;
            sets[1]                      = mGBufferReadSet;

            grfx::FullscreenQuad* pDrawQuad = mGBufferLightQuad;
            if (mDrawGBufferAttr) {
                pDrawQuad = mDebugDrawQuad;
            }
            frame.cmd->Draw(pDrawQuad, 2, sets);

            // Draw IBL environment
            //
            if (!mDrawGBufferAttr) {
                frame.cmd->SetScissors(mGBufferLightPass->GetScissor());
                frame.cmd->SetViewports(mGBufferLightPass->GetViewport());

                frame.cmd->BindGraphicsDescriptorSets(mIBLPipelineInterface, 1, &mIBLSet);
                frame.cmd->BindGraphicsPipeline(mIBLPipeline);
                frame.cmd->BindIndexBuffer(mIBLModel);
                frame.cmd->BindVertexBuffers(mIBLModel);
                frame.cmd->DrawIndexed(mIBLModel->GetIndexCount());
            }
        }
        frame.cmd->EndRenderPass();
#ifdef ENABLE_GPU_QUERIES
        frame.cmd->WriteTimestamp(frame.timestampQuery, grfx::PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 1);
#endif

        frame.cmd->TransitionImageLayout(
            mGBufferLightPass,
            grfx::RESOURCE_STATE_RENDER_TARGET,
            grfx::RESOURCE_STATE_SHADER_RESOURCE,
            grfx::RESOURCE_STATE_DEPTH_STENCIL_READ,
            grfx::RESOURCE_STATE_SHADER_RESOURCE);

        // =====================================================================
        //  Blit to swapchain
        // =====================================================================
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "swapchain render pass object is null");

        frame.cmd->SetScissors(renderPass->GetScissor());
        frame.cmd->SetViewports(renderPass->GetViewport());

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(renderPass);
        {
            // Draw gbuffer light output to swapchain
            frame.cmd->Draw(mDrawToSwapchain, 1, &mDrawToSwapchainSet);

            //// Draw ImGui
            //DrawDebugInfo([this]() { this->DrawGui(); });
            //DrawImGui(frame.cmd);
        }
        frame.cmd->EndRenderPass();
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_RENDER_TARGET, grfx::RESOURCE_STATE_PRESENT);
    }
#ifdef ENABLE_GPU_QUERIES
    // Resolve queries
    frame.cmd->ResolveQueryData(frame.timestampQuery, 0, 2);
    if (GetDevice()->PipelineStatsAvailable()) {
        frame.cmd->ResolveQueryData(frame.pipelineStatsQuery, 0, 1);
    }
#endif
    PPX_CHECKED_CALL(frame.cmd->End());

    grfx::SubmitInfo submitInfo     = {};
    submitInfo.commandBufferCount   = 1;
    submitInfo.ppCommandBuffers     = &frame.cmd;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.ppWaitSemaphores     = &frame.imageAcquiredSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.ppSignalSemaphores   = &frame.renderCompleteSemaphore;
    submitInfo.pFence               = frame.renderCompleteFence;

    PPX_CHECKED_CALL(GetGraphicsQueue()->Submit(&submitInfo));

    PPX_CHECKED_CALL(swapchain->Present(imageIndex, 1, &frame.renderCompleteSemaphore));
}

void ProjApp::DrawGui()
{
    ImGui::Separator();

    ImGui::Checkbox("Draw GBuffer Attribute", &mDrawGBufferAttr);

    static const char* currentGBufferAttrName = mGBufferAttrNames[mGBufferAttrIndex];
    if (ImGui::BeginCombo("GBuffer Attribute", currentGBufferAttrName)) {
        for (size_t i = 0; i < mGBufferAttrNames.size(); ++i) {
            bool isSelected = (currentGBufferAttrName == mGBufferAttrNames[i]);
            if (ImGui::Selectable(mGBufferAttrNames[i], isSelected)) {
                currentGBufferAttrName = mGBufferAttrNames[i];
                mGBufferAttrIndex      = static_cast<uint32_t>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    ImGui::Checkbox("Enable IBL", &mEnableIBL);
    ImGui::Checkbox("Enable Environment Map", &mEnableEnv);

    static const char* currentIBLName = "GoldenHour";
    if (ImGui::BeginCombo("PBR IBL Textures", currentIBLName)) {
        for (size_t i = 0; i < mIBLNames.size(); ++i) {
            bool isSelected = (currentIBLName == mIBLNames[i]);
            if (ImGui::Selectable(mIBLNames[i], isSelected)) {
                currentIBLName  = mIBLNames[i];
                mTargetIBLIndex = static_cast<uint32_t>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    ImGui::Columns(2);

    uint64_t frequency = 0;
    GetGraphicsQueue()->GetTimestampFrequency(&frequency);
    ImGui::Text("Previous GPU Frame Time");
    ImGui::NextColumn();
    ImGui::Text("%f ms ", static_cast<float>(mTotalGpuFrameTime / static_cast<double>(frequency)) * 1000.0f);
    ImGui::NextColumn();

    ImGui::Separator();

    ImGui::Text("IAVertices");
    ImGui::NextColumn();
    ImGui::Text("%lu", mPipelineStatistics.IAVertices);
    ImGui::NextColumn();

    ImGui::Text("IAPrimitives");
    ImGui::NextColumn();
    ImGui::Text("%lu", mPipelineStatistics.IAPrimitives);
    ImGui::NextColumn();

    ImGui::Text("VSInvocations");
    ImGui::NextColumn();
    ImGui::Text("%lu", mPipelineStatistics.VSInvocations);
    ImGui::NextColumn();

    ImGui::Text("CInvocations");
    ImGui::NextColumn();
    ImGui::Text("%lu", mPipelineStatistics.CInvocations);
    ImGui::NextColumn();

    ImGui::Text("CPrimitives");
    ImGui::NextColumn();
    ImGui::Text("%lu", mPipelineStatistics.CPrimitives);
    ImGui::NextColumn();

    ImGui::Text("PSInvocations");
    ImGui::NextColumn();
    ImGui::Text("%lu", mPipelineStatistics.PSInvocations);
    ImGui::NextColumn();

    ImGui::Columns(1);
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
