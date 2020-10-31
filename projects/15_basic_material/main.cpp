#include "ppx/ppx.h"
#include "ppx/camera.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#if defined(USE_DX)
grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
grfx::Api kApi = grfx::API_VK_1_1;
#endif

#define kWindowWidth  1280
#define kWindowHeight 720

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Shutdown() override;
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

    std::vector<PerFrame>   mPerFrame;
    PerspCamera             mCamera;
    grfx::DescriptorPoolPtr mDescriptorPool;
    grfx::ModelPtr          mModel;

    // Descriptor Set 0 - Scene Data
    grfx::DescriptorSetLayoutPtr mSceneDataLayout;
    grfx::DescriptorSetPtr       mSceneDataSet;
    grfx::BufferPtr              mCpuSceneConstants;
    grfx::BufferPtr              mGpuSceneConstants;
    grfx::BufferPtr              mCpuLightConstants;
    grfx::BufferPtr              mGpuLightConstants;

    // Descriptor Set 1 - MaterialData Resources
    grfx::DescriptorSetLayoutPtr mMaterialResourcesLayout;

    struct MaterialResources
    {
        grfx::DescriptorSetPtr set;
        grfx::TexturePtr       albedoTexture;
        grfx::TexturePtr       roughnessTexture;
        grfx::TexturePtr       metalnessTexture;
        grfx::TexturePtr       normalMapTexture;
    };

    grfx::TexturePtr                  mEnvironmentMap;
    grfx::TexturePtr                  mReflectionMap;
    grfx::SamplerPtr                  mSampler;
    MaterialResources                 mRustedIron;
    MaterialResources                 mSciFiMetal;
    MaterialResources                 mPaintedMetal;
    std::vector<grfx::DescriptorSet*> mMaterialResourcesSets;

    // Descriptor Set 2 - MaterialData Data
    grfx::DescriptorSetLayoutPtr mMaterialDataLayout;
    grfx::DescriptorSetPtr       mMaterialDataSet;
    grfx::BufferPtr              mCpuMaterialConstants;
    grfx::BufferPtr              mGpuMaterialConstants;

    // Descriptor Set 3 - Model Data
    grfx::DescriptorSetLayoutPtr mModelDataLayout;
    grfx::DescriptorSetPtr       mModelDataSet;
    grfx::BufferPtr              mCpuModelConstants;
    grfx::BufferPtr              mGpuModelConstants;

    grfx::PipelineInterfacePtr           mPipelineInterface;
    grfx::GraphicsPipelinePtr            mGouraudPipeline;
    grfx::GraphicsPipelinePtr            mPhongPipeline;
    grfx::GraphicsPipelinePtr            mBlinnPhongPipeline;
    grfx::GraphicsPipelinePtr            mPBRPipeline;
    std::vector<grfx::GraphicsPipeline*> mShaderPipelines;

    struct MaterialData
    {
        float3 albedo           = float3(0.4f, 0.4f, 0.7f);
        float  roughness        = 0.5f; // 0 = smooth, 1 = rough
        float  metalness        = 0.5f; // 0 = dielectric, 1 = metal
        bool   albedoSelect     = 0;    // 0 = value, 1 = texture
        bool   roughnessSelect  = 0;    // 0 = value, 1 = texture
        bool   metalnessSelect  = 0;    // 0 = value, 1 = texture
        bool   normalSelect     = 0;    // 0 = attrb, 1 = texture
        bool   iblSelect        = 0;    // 0 = white, 1 = texture
        bool   reflectionSelect = 0;    // 0 = none,  1 = texture
    };

    float mRotY    = 0;
    float mAmbient = 0;

    MaterialData mMaterialData = {};

    uint32_t           mMaterialIndex = 0;
    std::vector<char*> mMaterialNames = {
        "Rusted Iron",
        "SciFi Metal",
        "Painted Metal",
    };

    uint32_t           mShaderIndex = 0;
    std::vector<char*> mShaderNames = {
        "Gouraud",
        "Phong",
        "Blinn",
        "PBR",
    };

private:
    void SetupMaterialResources(
        const fs::path&    albedoPath,
        const fs::path&    roughnessPath,
        const fs::path&    metalnessPath,
        const fs::path&    normalMapPath,
        MaterialResources& materialResources);
    void SetupMaterials();
    void DrawGui();
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName                    = "basic_material";
    settings.window.width               = kWindowWidth;
    settings.window.height              = kWindowHeight;
    settings.grfx.api                   = kApi;
    settings.grfx.swapchain.depthFormat = grfx::FORMAT_D32_FLOAT;
    settings.grfx.enableDebug           = true;
    settings.grfx.numFramesInFlight     = 1;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::SetupMaterialResources(
    const fs::path&    albedoPath,
    const fs::path&    roughnessPath,
    const fs::path&    metalnessPath,
    const fs::path&    normalMapPath,
    MaterialResources& materialResources)
{
    Result ppxres = ppx::SUCCESS;

    PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mMaterialResourcesLayout, &materialResources.set));

    // Albedo
    {
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath(albedoPath), &materialResources.albedoTexture));

        grfx::WriteDescriptor write = {};
        write.binding               = 0;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = materialResources.albedoTexture->GetSampledImageView();
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }

    // Roughness
    {
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath(roughnessPath), &materialResources.roughnessTexture));

        grfx::WriteDescriptor write = {};
        write.binding               = 1;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = materialResources.roughnessTexture->GetSampledImageView();
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }

    // Metalness
    {
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath(metalnessPath), &materialResources.metalnessTexture));

        grfx::WriteDescriptor write = {};
        write.binding               = 2;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = materialResources.metalnessTexture->GetSampledImageView();
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }

    // Normal map
    {
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath(normalMapPath), &materialResources.normalMapTexture));

        grfx::WriteDescriptor write = {};
        write.binding               = 3;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = materialResources.normalMapTexture->GetSampledImageView();
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }

    // Environment map
    {
        grfx::WriteDescriptor write = {};
        write.binding               = 6;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = mEnvironmentMap->GetSampledImageView();
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }

    // Reflection map
    {
        grfx::WriteDescriptor write = {};
        write.binding               = 7;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = mReflectionMap->GetSampledImageView();
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }

    // Sampler
    {
        grfx::WriteDescriptor write = {};
        write.binding               = 8;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLER;
        write.pSampler              = mSampler;
        PPX_CHECKED_CALL(ppxres = materialResources.set->UpdateDescriptors(1, &write));
    }
}

void ProjApp::SetupMaterials()
{
    Result ppxres = ppx::SUCCESS;

    // Layout
    grfx::DescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.bindings.push_back({grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    createInfo.bindings.push_back({grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    createInfo.bindings.push_back({grfx::DescriptorBinding{2, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    createInfo.bindings.push_back({grfx::DescriptorBinding{3, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    createInfo.bindings.push_back({grfx::DescriptorBinding{6, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    createInfo.bindings.push_back({grfx::DescriptorBinding{7, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    createInfo.bindings.push_back({grfx::DescriptorBinding{8, grfx::DESCRIPTOR_TYPE_SAMPLER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mMaterialResourcesLayout));

    // Environment map for IBL
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/GoldenHour/env.hdr"), &mEnvironmentMap));

    // Reflection map
    PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("materials/ibl/GoldenHour/refl.hdr"), &mReflectionMap));

    // Sampler
    grfx::SamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.magFilter               = grfx::FILTER_LINEAR;
    samplerCreateInfo.minFilter               = grfx::FILTER_LINEAR;
    samplerCreateInfo.mipmapMode              = grfx::SAMPLER_MIPMAP_MODE_LINEAR;
    PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&samplerCreateInfo, &mSampler));

    // RustedIron
    {
        SetupMaterialResources(
            "materials/textures/RustedIron/albedo.png",
            "materials/textures/RustedIron/roughness.png",
            "materials/textures/RustedIron/metalness.png",
            "materials/textures/RustedIron/normal.png",
            mRustedIron);
        mMaterialResourcesSets.push_back(mRustedIron.set);
    };

    // SciFiMetal
    {
        SetupMaterialResources(
            "materials/textures/SciFiMetal/albedo.jpg",
            "materials/textures/SciFiMetal/roughness.jpg",
            "materials/textures/SciFiMetal/metalness.jpg",
            "materials/textures/SciFiMetal/normal.jpg",
            mSciFiMetal);
        mMaterialResourcesSets.push_back(mSciFiMetal.set);
    }

    // PaintedMetal
    {
        SetupMaterialResources(
            "materials/textures/PaintedMetal/albedo.jpg",
            "materials/textures/PaintedMetal/roughness.jpg",
            "materials/textures/PaintedMetal/metalness.jpg",
            "materials/textures/PaintedMetal/normal.jpg",
            mPaintedMetal);
        mMaterialResourcesSets.push_back(mPaintedMetal.set);
    }
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Cameras
    {
        mCamera = PerspCamera(60.0f, GetWindowAspect());
    }

    {
        grfx::DescriptorPoolCreateInfo createInfo = {};
        createInfo.sampler                        = 1000;
        createInfo.sampledImage                   = 1000;
        createInfo.uniformBuffer                  = 1000;
        createInfo.structuredBuffer               = 1000;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&createInfo, &mDescriptorPool));
    }

    // Entity
    {
        TriMesh::Options options = TriMesh::Options().Indices().VertexColors().Normals().TexCoords().Tangents();
        TriMesh          mesh    = TriMesh::CreateFromOBJ(GetAssetPath("basic/models/material_sphere.obj"), options);

        Geometry geo;
        PPX_CHECKED_CALL(ppxres = Geometry::Create(mesh, &geo));
        PPX_CHECKED_CALL(ppxres = CreateModelFromGeometry(GetGraphicsQueue(), &geo, &mModel));
    }

    // Scene data
    {
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{1, grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mSceneDataLayout));

        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mSceneDataLayout, &mSceneDataSet));

        // Scene constants
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = PPX_MINIUM_CONSTANT_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mCpuSceneConstants));

        bufferCreateInfo.usageFlags.bits.transferDst   = true;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mGpuSceneConstants));

        // HlslLight constants
        bufferCreateInfo                             = {};
        bufferCreateInfo.size                        = PPX_MINIUM_STRUCTURED_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mCpuLightConstants));

        bufferCreateInfo.usageFlags.bits.transferDst      = true;
        bufferCreateInfo.usageFlags.bits.structuredBuffer = true;
        bufferCreateInfo.memoryUsage                      = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mGpuLightConstants));

        grfx::WriteDescriptor write = {};
        write.binding               = 0;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset          = 0;
        write.bufferRange           = PPX_WHOLE_SIZE;
        write.pBuffer               = mGpuSceneConstants;
        PPX_CHECKED_CALL(ppxres = mSceneDataSet->UpdateDescriptors(1, &write));

        write                         = {};
        write.binding                 = 1;
        write.arrayIndex              = 0;
        write.type                    = grfx::DESCRIPTOR_TYPE_STRUCTURED_BUFFER;
        write.bufferOffset            = 0;
        write.bufferRange             = PPX_WHOLE_SIZE;
        write.structuredElementCount  = 1;
        write.structuredElementStride = 64;
        write.pBuffer                 = mGpuLightConstants;
        PPX_CHECKED_CALL(ppxres = mSceneDataSet->UpdateDescriptors(1, &write));
    }

    // MaterialData resources
    SetupMaterials();

    // MaterialData data
    {
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mMaterialDataLayout));

        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mMaterialDataLayout, &mMaterialDataSet));

        // MaterialData constants
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = PPX_MINIUM_CONSTANT_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mCpuMaterialConstants));

        bufferCreateInfo.usageFlags.bits.transferDst   = true;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mGpuMaterialConstants));

        grfx::WriteDescriptor write = {};
        write.binding               = 0;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset          = 0;
        write.bufferRange           = PPX_WHOLE_SIZE;
        write.pBuffer               = mGpuMaterialConstants;
        PPX_CHECKED_CALL(ppxres = mMaterialDataSet->UpdateDescriptors(1, &write));
    }

    // Model data
    {
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&createInfo, &mModelDataLayout));

        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mModelDataLayout, &mModelDataSet));

        // Model constants
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = PPX_MINIUM_CONSTANT_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mCpuModelConstants));

        bufferCreateInfo.usageFlags.bits.transferDst   = true;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mGpuModelConstants));

        grfx::WriteDescriptor write = {};
        write.binding               = 0;
        write.arrayIndex            = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset          = 0;
        write.bufferRange           = PPX_WHOLE_SIZE;
        write.pBuffer               = mGpuModelConstants;
        PPX_CHECKED_CALL(ppxres = mModelDataSet->UpdateDescriptors(1, &write));
    }

    // Pipeline Interface
    {
        grfx::PipelineInterfaceCreateInfo createInfo = {};
        createInfo.setCount                          = 4;
        createInfo.sets[0].set                       = 0;
        createInfo.sets[0].pLayout                   = mSceneDataLayout;
        createInfo.sets[1].set                       = 1;
        createInfo.sets[1].pLayout                   = mMaterialResourcesLayout;
        createInfo.sets[2].set                       = 2;
        createInfo.sets[2].pLayout                   = mMaterialDataLayout;
        createInfo.sets[3].set                       = 3;
        createInfo.sets[3].pLayout                   = mModelDataLayout;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&createInfo, &mPipelineInterface));
    }

    // Pipeline
    {
        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.vertexInputState.bindingCount      = mModel->GetVertexBufferCount();
        gpCreateInfo.vertexInputState.bindings[0]       = *mModel->GetVertexBinding(0);
        gpCreateInfo.vertexInputState.bindings[1]       = *mModel->GetVertexBinding(1);
        gpCreateInfo.vertexInputState.bindings[2]       = *mModel->GetVertexBinding(2);
        gpCreateInfo.vertexInputState.bindings[3]       = *mModel->GetVertexBinding(3);
        gpCreateInfo.vertexInputState.bindings[4]       = *mModel->GetVertexBinding(4);
        gpCreateInfo.vertexInputState.bindings[5]       = *mModel->GetVertexBinding(5);
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

        grfx::ShaderModulePtr VS;
        std::vector<char>     bytecode = LoadShader(GetAssetPath("materials/shaders"), "VertexShader.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        // Gouraud
        {
            grfx::ShaderModulePtr PS;
            bytecode = LoadShader(GetAssetPath("materials/shaders"), "Gouraud.ps");
            PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
            shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

            gpCreateInfo.VS = {VS.Get(), "vsmain"};
            gpCreateInfo.PS = {PS.Get(), "psmain"};

            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mGouraudPipeline));
            GetDevice()->DestroyShaderModule(PS);

            mShaderPipelines.push_back(mGouraudPipeline);
        }

        // Phong
        {
            grfx::ShaderModulePtr PS;
            bytecode = LoadShader(GetAssetPath("materials/shaders"), "Phong.ps");
            PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
            shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

            gpCreateInfo.VS = {VS.Get(), "vsmain"};
            gpCreateInfo.PS = {PS.Get(), "psmain"};

            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mPhongPipeline));
            GetDevice()->DestroyShaderModule(PS);

            mShaderPipelines.push_back(mPhongPipeline);
        }

        // BlinnPhong
        {
            grfx::ShaderModulePtr PS;
            bytecode = LoadShader(GetAssetPath("materials/shaders"), "BlinnPhong.ps");
            PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
            shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

            gpCreateInfo.VS = {VS.Get(), "vsmain"};
            gpCreateInfo.PS = {PS.Get(), "psmain"};

            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mBlinnPhongPipeline));
            GetDevice()->DestroyShaderModule(PS);

            mShaderPipelines.push_back(mBlinnPhongPipeline);
        }

        // PBR
        {
            grfx::ShaderModulePtr PS;
            bytecode = LoadShader(GetAssetPath("materials/shaders"), "PBR.ps");
            PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
            shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

            gpCreateInfo.VS = {VS.Get(), "vsmain"};
            gpCreateInfo.PS = {PS.Get(), "psmain"};

            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mPBRPipeline));
            GetDevice()->DestroyShaderModule(PS);

            mShaderPipelines.push_back(mPBRPipeline);
        }
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

void ProjApp::Shutdown()
{
}

void ProjApp::Render()
{
    Result ppxres = ppx::SUCCESS;

    uint32_t  iffIndex = GetInFlightFrameIndex();
    PerFrame& frame    = mPerFrame[0];

    grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(ppxres = swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(ppxres = frame.imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(ppxres = frame.renderCompleteFence->WaitAndReset());

    // Update camera(s)
    mCamera.LookAt(float3(0, 0, 5), float3(0, 0, 0));

    // Update scene constants
    {
        PPX_HLSL_PACK_BEGIN();
        struct HlslSceneData
        {
            hlsl_uint<4>      frameNumber;
            hlsl_float<12>    time;
            hlsl_float4x4<64> viewProjectionMatrix;
            hlsl_float3<12>   eyePosition;
            hlsl_uint<4>      lightCount;
            hlsl_float<4>     ambient;
        };
        PPX_HLSL_PACK_END();

        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(ppxres = mCpuSceneConstants->MapMemory(0, &pMappedAddress));

        HlslSceneData* pSceneData        = static_cast<HlslSceneData*>(pMappedAddress);
        pSceneData->viewProjectionMatrix = mCamera.GetViewProjectionMatrix();
        pSceneData->eyePosition          = mCamera.GetEyePosition();
        pSceneData->lightCount           = 4;
        pSceneData->ambient              = mAmbient;

        mCpuSceneConstants->UnmapMemory();

        grfx::BufferToBufferCopyInfo copyInfo = {mCpuSceneConstants->GetSize()};
        GetGraphicsQueue()->CopyBufferToBuffer(&copyInfo, mCpuSceneConstants, mGpuSceneConstants);
    }

    // Lights
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
        PPX_CHECKED_CALL(ppxres = mCpuLightConstants->MapMemory(0, &pMappedAddress));

        HlslLight* pLight  = static_cast<HlslLight*>(pMappedAddress);
        pLight[0].position = float3(10, 5, 10);
        pLight[1].position = float3(-10, 0, 5);
        pLight[2].position = float3(1, 10, 3);
        pLight[4].position = float3(-1, 0, 15);

        mCpuLightConstants->UnmapMemory();

        grfx::BufferToBufferCopyInfo copyInfo = {mCpuLightConstants->GetSize()};
        GetGraphicsQueue()->CopyBufferToBuffer(&copyInfo, mCpuLightConstants, mGpuLightConstants);
    }

    // MaterialData constatns
    {
        PPX_HLSL_PACK_BEGIN();
        struct HlslMaterial
        {
            hlsl_float3<12> albedo;
            hlsl_float<4>   roughness;
            hlsl_float<4>   metalness;
            hlsl_uint<4>    albedoSelect;
            hlsl_uint<4>    roughnessSelect;
            hlsl_uint<4>    metalnessSelect;
            hlsl_uint<4>    normalSelect;
            hlsl_uint<4>    iblSelect;
            hlsl_uint<4>    reflectionSelect;
        };
        PPX_HLSL_PACK_END();

        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(ppxres = mCpuMaterialConstants->MapMemory(0, &pMappedAddress));

        HlslMaterial* pMaterial     = static_cast<HlslMaterial*>(pMappedAddress);
        pMaterial->albedo           = mMaterialData.albedo;
        pMaterial->roughness        = mMaterialData.roughness;
        pMaterial->metalness        = mMaterialData.metalness;
        pMaterial->albedoSelect     = mMaterialData.albedoSelect;
        pMaterial->roughnessSelect  = mMaterialData.roughnessSelect;
        pMaterial->metalnessSelect  = mMaterialData.metalnessSelect;
        pMaterial->normalSelect     = mMaterialData.normalSelect;
        pMaterial->iblSelect        = mMaterialData.iblSelect;
        pMaterial->reflectionSelect = mMaterialData.reflectionSelect;

        mCpuMaterialConstants->UnmapMemory();

        grfx::BufferToBufferCopyInfo copyInfo = {mCpuMaterialConstants->GetSize()};
        GetGraphicsQueue()->CopyBufferToBuffer(&copyInfo, mCpuMaterialConstants, mGpuMaterialConstants);
    }

    // Update model constants
    {
        float t = GetElapsedSeconds();

        float4x4 R = glm::rotate(glm::radians(mRotY + 180.0f), float3(0, 1, 0));
        float4x4 S = glm::scale(float3(3.0f));
        float4x4 M = R * S;

        PPX_HLSL_PACK_BEGIN();
        struct HlslModelData
        {
            hlsl_float4x4<64> modelMatrix;
            hlsl_float4x4<64> normalMatrix;
            hlsl_float3<12>   debugColor;
        };
        PPX_HLSL_PACK_END();

        void* pMappedAddress = nullptr;
        PPX_CHECKED_CALL(ppxres = mCpuModelConstants->MapMemory(0, &pMappedAddress));

        HlslModelData* pModelData = static_cast<HlslModelData*>(pMappedAddress);
        pModelData->modelMatrix   = M;
        pModelData->normalMatrix  = glm::inverseTranspose(M);

        mCpuModelConstants->UnmapMemory();

        grfx::BufferToBufferCopyInfo copyInfo = {mCpuModelConstants->GetSize()};
        GetGraphicsQueue()->CopyBufferToBuffer(&copyInfo, mCpuModelConstants, mGpuModelConstants);
    }

    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        // =====================================================================
        //  Render scene
        // =====================================================================
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(renderPass);
        {
            frame.cmd->SetScissors(GetScissor());
            frame.cmd->SetViewports(GetViewport());

            grfx::DescriptorSet* sets[4] = {nullptr};
            sets[0]                      = mSceneDataSet;
            sets[1]                      = mMaterialResourcesSets[mMaterialIndex];
            sets[2]                      = mMaterialDataSet;
            sets[3]                      = mModelDataSet;
            frame.cmd->BindGraphicsDescriptorSets(mPipelineInterface, 4, sets);

            frame.cmd->BindGraphicsPipeline(mShaderPipelines[mShaderIndex]);

            frame.cmd->BindIndexBuffer(mModel);
            frame.cmd->BindVertexBuffers(mModel);
            frame.cmd->DrawIndexed(mModel->GetIndexCount());

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

    ImGui::SliderFloat("Rot Y", &mRotY, -180.0f, 180.0f, "%.03f degrees");

    ImGui::Separator();

    ImGui::SliderFloat("Ambient", &mAmbient, 0.0f, 1.0f, "%.03f");

    ImGui::Separator();

    static const char* currentMaterialName = mMaterialNames[0];
    if (ImGui::BeginCombo("Material Textures", currentMaterialName)) {
        for (size_t i = 0; i < mMaterialNames.size(); ++i) {
            bool isSelected = (currentMaterialName == mMaterialNames[i]);
            if (ImGui::Selectable(mMaterialNames[i], isSelected)) {
                currentMaterialName = mMaterialNames[i];
                mMaterialIndex      = static_cast<uint32_t>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    static const char* currentShaderName = mShaderNames[0];
    if (ImGui::BeginCombo("Shader Pipeline", currentShaderName)) {
        for (size_t i = 0; i < mShaderNames.size(); ++i) {
            bool isSelected = (currentShaderName == mShaderNames[i]);
            if (ImGui::Selectable(mShaderNames[i], isSelected)) {
                currentShaderName = mShaderNames[i];
                mShaderIndex      = static_cast<uint32_t>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SliderFloat("Roughness", &mMaterialData.roughness, 0.0f, 1.0f, "%.03f degrees");
    ImGui::SliderFloat("Metalness", &mMaterialData.metalness, 0.0f, 1.0f, "%.03f degrees");
    ImGui::Checkbox("Use Roughness Texture", &mMaterialData.roughnessSelect);
    ImGui::Checkbox("Use Metalness Texture", &mMaterialData.metalnessSelect);
    ImGui::Checkbox("Use IBL", &mMaterialData.iblSelect);
    ImGui::Checkbox("Use Reflection Map", &mMaterialData.reflectionSelect);
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
