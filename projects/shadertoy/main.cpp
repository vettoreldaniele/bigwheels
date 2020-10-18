#include "ppx/ppx.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#if defined(USE_DX)
grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
grfx::Api kApi = grfx::API_VK_1_1;
#endif

#define kWindowWidth          1280
#define kWindowHeight         720
#define kNumThreadsX          8
#define kNumThreadsY          8
#define kDefaultShaderToy     4
#define kShaderToyRenderScale 1.0f

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

private:
    void DrawGui();

private:
    struct PerFrame
    {
        grfx::CommandBufferPtr cmd;
        grfx::SemaphorePtr     imageAcquiredSemaphore;
        grfx::FencePtr         imageAcquiredFence;
        grfx::SemaphorePtr     renderCompleteSemaphore;
        grfx::FencePtr         renderCompleteFence;
    };

    struct ShaderToy
    {
        grfx::ComputePipelinePtr pipeline;
    };

    std::vector<PerFrame>        mPerFrame;
    grfx::VertexBinding          mFullScreenVertexBinding;
    grfx::DescriptorSetLayoutPtr mFullScreenDescriptorSetLayout;
    grfx::PipelineInterfacePtr   mFullScreenPipelineInterface;
    grfx::GraphicsPipelinePtr    mFullScreenPipeline;
    grfx::DescriptorSetPtr       mFullScreenDescriptorSet;
    grfx::BufferPtr              mFullScreenVertexBuffer;
    grfx::ImagePtr               mOutputImage;
    grfx::SamplerPtr             mFullScreenSampler;
    grfx::SampledImageViewPtr    mFullScreenSampledImageView;
    grfx::StorageImageViewPtr    mOutputStorageImageView;
    std::vector<const char*>     mShaderToyNames;
    grfx::BufferPtr              mShaderToyUniformBuffer;
    grfx::DescriptorSetLayoutPtr mShaderToyDescriptorSetLayout;
    grfx::DescriptorSetPtr       mShaderToyDescriptorSet;
    grfx::PipelineInterfacePtr   mShaderToyPipelineInterface;
    uint32_t                     mShaderToyIndex = kDefaultShaderToy;
    std::vector<ShaderToy>       mShaderToys;
    grfx::DescriptorPoolPtr      mDescriptorPool;
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "shadertoy";
    settings.window.width     = kWindowWidth;
    settings.window.height    = kWindowHeight;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Fullscreen
    {
        // Vertex binding
        mFullScreenVertexBinding.AppendAttribute({"POSITION", 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});
        mFullScreenVertexBinding.AppendAttribute({"TEXCOORD", 1, grfx::FORMAT_R32G32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});

        // Descriptor set layout
        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLER));
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mFullScreenDescriptorSetLayout));

        // Pipeline interface
        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mFullScreenDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mFullScreenPipelineInterface));

        // Load VS bytecode
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "StaticTexture.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        // Create VS
        grfx::ShaderModulePtr        VS;
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));
        // Load PS bytecode
        bytecode = LoadShader(GetAssetPath("basic/shaders"), "StaticTexture.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        // Create PS
        grfx::ShaderModulePtr PS;
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        // Pipeline
        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {VS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {PS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = 1;
        gpCreateInfo.vertexInputState.bindings[0]       = mFullScreenVertexBinding;
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = false;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.pPipelineInterface                 = mFullScreenPipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mFullScreenPipeline));

        // Destroy VS and PS
        GetDevice()->DestroyShaderModule(VS);
        GetDevice()->DestroyShaderModule(PS);

        // Vertex data
        {
            // clang-format off
            std::vector<float> vertexData = {
                // position           // tex coords
                -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,
                 1.0f, -1.0f, 0.0f,   1.0f, 1.0f,

                -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
                 1.0f, -1.0f, 0.0f,   1.0f, 1.0f,
                 1.0f,  1.0f, 0.0f,   1.0f, 0.0f,
            };
            // clang-format on
            uint32_t dataSize = ppx::SizeInBytesU32(vertexData);

            grfx::BufferCreateInfo bufferCreateInfo       = {};
            bufferCreateInfo.size                         = dataSize;
            bufferCreateInfo.usageFlags.bits.vertexBuffer = true;
            bufferCreateInfo.memoryUsage                  = grfx::MEMORY_USAGE_CPU_TO_GPU;

            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mFullScreenVertexBuffer));

            void* pAddr = nullptr;
            PPX_CHECKED_CALL(ppxres = ppxres = mFullScreenVertexBuffer->MapMemory(0, &pAddr));
            memcpy(pAddr, vertexData.data(), dataSize);
            mFullScreenVertexBuffer->UnmapMemory();
        }
    }

    // Shader toys
    {
        // Descriptor set layout
        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER));
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE));
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mShaderToyDescriptorSetLayout));

        // Pipeline interface
        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mShaderToyDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mShaderToyPipelineInterface));

        // Shader names
        mShaderToyNames = {
            "Fractal_Land.cs",
            "Geodesic_Tiling.cs",
            "Happy_Jumping.cs",
            "Protean_Clouds.cs",
            "Seascape.cs",
            "Xyptonjtroz.cs",
        };

        // Create toy
        for (auto& name : mShaderToyNames) {
            ShaderToy toy = {};

            // Loader CS bytecode
            std::vector<char> bytecode = LoadShader(GetAssetPath("shadertoy/shaders"), name);
            PPX_ASSERT_MSG(!bytecode.empty(), "CS shader bytecode load failed: " << name);
            grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};

            // Create CS
            grfx::ShaderModulePtr CS;
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &CS));

            // Create pipeline
            grfx::ComputePipelineCreateInfo cpCreateInfo = {};
            cpCreateInfo.CS                              = {CS.Get(), "csmain"};
            cpCreateInfo.pPipelineInterface              = mShaderToyPipelineInterface;
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateComputePipeline(&cpCreateInfo, &toy.pipeline));

            // Destroy CS
            GetDevice()->DestroyShaderModule(CS);

            // Store toy
            mShaderToys.push_back(toy);
        }
    }

    // Ouput image
    {
        float    scale  = std::max<float>(0.1f, std::min<float>(1.0f, kShaderToyRenderScale));
        uint32_t width  = static_cast<uint32_t>(GetWindowWidth() * scale);
        uint32_t height = static_cast<uint32_t>(GetWindowHeight() * scale);

        grfx::ImageCreateInfo imageCreateInfo   = grfx::ImageCreateInfo::SampledImage2D(width, height, grfx::FORMAT_R8G8B8A8_UNORM);
        imageCreateInfo.usageFlags.bits.storage = true;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateImage(&imageCreateInfo, &mOutputImage));

        grfx::SampledImageViewCreateInfo sampledViewCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mOutputImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampledImageView(&sampledViewCreateInfo, &mFullScreenSampledImageView));

        grfx::StorageImageViewCreateInfo storageViewCreateInfo = grfx::StorageImageViewCreateInfo::GuessFromImage(mOutputImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateStorageImageView(&storageViewCreateInfo, &mOutputStorageImageView));

        grfx::SamplerCreateInfo samplerCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&samplerCreateInfo, &mFullScreenSampler));
    }

    // Uniform buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mShaderToyUniformBuffer));
    }

    // Allocate descriptor sets
    {
        // Create descriptor pool
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = 1;
        poolCreateInfo.sampledImage                   = 1;
        poolCreateInfo.sampler                        = 1;
        poolCreateInfo.storageImage                   = 1;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

        // Full screen descriptor set
        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mFullScreenDescriptorSetLayout, &mFullScreenDescriptorSet));

        // Shader toy descriptor set
        PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mShaderToyDescriptorSetLayout, &mShaderToyDescriptorSet));
    }

    // Update descriptors
    {
        // Full screen sampled image view
        grfx::WriteDescriptor write = {};
        write.binding               = 0;
        write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageView            = mFullScreenSampledImageView;
        PPX_CHECKED_CALL(ppxres = mFullScreenDescriptorSet->UpdateDescriptors(1, &write));

        // Full screen sampler
        write          = {};
        write.binding  = 1;
        write.type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
        write.pSampler = mFullScreenSampler;
        PPX_CHECKED_CALL(ppxres = mFullScreenDescriptorSet->UpdateDescriptors(1, &write));

        // Shader toy uniform buffer
        write              = {};
        write.binding      = 0;
        write.type         = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset = 0;
        write.bufferRange  = PPX_WHOLE_SIZE;
        write.pBuffer      = mShaderToyUniformBuffer;
        PPX_CHECKED_CALL(ppxres = mShaderToyDescriptorSet->UpdateDescriptors(1, &write));

        // Shader toy output image
        write            = {};
        write.binding    = 1;
        write.type       = grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE;
        write.pImageView = mOutputStorageImageView;
        PPX_CHECKED_CALL(ppxres = mShaderToyDescriptorSet->UpdateDescriptors(1, &write));
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

    /*
    // Uniform buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mUniformBuffer));
    }

    // Texture image, view, and sampler
    {
        grfx::ImageCreateInfo imageCreateInfo   = grfx::ImageCreateInfo::SampledImage2D(kWindowWidth / 2, kWindowHeight / 2, grfx::FORMAT_R8G8B8A8_UNORM);
        imageCreateInfo.usageFlags.bits.storage = true;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateImage(&imageCreateInfo, &mImage));

        grfx::SampledImageViewCreateInfo sampledViewCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampledImageView(&sampledViewCreateInfo, &mSampledImageView));

        grfx::StorageImageViewCreateInfo storageViewCreateInfo = grfx::StorageImageViewCreateInfo::GuessFromImage(mImage);
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateStorageImageView(&storageViewCreateInfo, &mStorageImageView));

        grfx::SamplerCreateInfo samplerCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateSampler(&samplerCreateInfo, &mSampler));
    }

    // Descriptors
    {
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = 1;
        poolCreateInfo.sampledImage                   = 1;
        poolCreateInfo.sampler                        = 1;
        poolCreateInfo.storageImage                   = 1;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

        // Compute
        {
            grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER));
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE));
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mComputeDescriptorSetLayout));

            PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mComputeDescriptorSetLayout, &mComputeDescriptorSet));

            grfx::WriteDescriptor write = {};
            write.binding               = 0;
            write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.bufferOffset          = 0;
            write.bufferRange           = PPX_WHOLE_SIZE;
            write.pBuffer               = mUniformBuffer;
            PPX_CHECKED_CALL(ppxres = mComputeDescriptorSet->UpdateDescriptors(1, &write));

            write            = {};
            write.binding    = 1;
            write.type       = grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE;
            write.pImageView = mStorageImageView;
            PPX_CHECKED_CALL(ppxres = mComputeDescriptorSet->UpdateDescriptors(1, &write));
        }

        // Graphics
        {
            grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
            layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLER));
            PPX_CHECKED_CALL(ppxres = GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mGraphicsDescriptorSetLayout));

            PPX_CHECKED_CALL(ppxres = GetDevice()->AllocateDescriptorSet(mDescriptorPool, mGraphicsDescriptorSetLayout, &mGraphicsDescriptorSet));

            grfx::WriteDescriptor write = {};
            write.binding               = 0;
            write.type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write.pImageView            = mSampledImageView;
            PPX_CHECKED_CALL(ppxres = mGraphicsDescriptorSet->UpdateDescriptors(1, &write));

            write          = {};
            write.binding  = 1;
            write.type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
            write.pSampler = mSampler;
            PPX_CHECKED_CALL(ppxres = mGraphicsDescriptorSet->UpdateDescriptors(1, &write));
        }
    }

    // Compute pipeline
    {
        std::vector<char> bytecode = LoadShader(GetAssetPath("shadertoy/shaders"), "Fractal_Land.cs");
        PPX_ASSERT_MSG(!bytecode.empty(), "CS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mCS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mComputeDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mComputePipelineInterface));

        grfx::ComputePipelineCreateInfo cpCreateInfo = {};
        cpCreateInfo.CS                              = {mCS.Get(), "csmain"};
        cpCreateInfo.pPipelineInterface              = mComputePipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateComputePipeline(&cpCreateInfo, &mComputePipeline));
    }

    // Graphics pipeline
    {
        std::vector<char> bytecode = LoadShader(GetAssetPath("basic/shaders"), "StaticTexture.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

        bytecode = LoadShader(GetAssetPath("basic/shaders"), "StaticTexture.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mGraphicsDescriptorSetLayout;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mGraphicsPipelineInterface));

        mVertexBinding.AppendAttribute({"POSITION", 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});
        mVertexBinding.AppendAttribute({"TEXCOORD", 1, grfx::FORMAT_R32G32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX});

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {mVS.Get(), "vsmain"};
        gpCreateInfo.PS                                 = {mPS.Get(), "psmain"};
        gpCreateInfo.vertexInputState.bindingCount      = 1;
        gpCreateInfo.vertexInputState.bindings[0]       = mVertexBinding;
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthEnable                        = false;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.pPipelineInterface                 = mGraphicsPipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mGraphicsPipeline));
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

    // Vertex buffer and geometry data
    {
        // clang-format off
        std::vector<float> vertexData = {
            // position           // tex coords
            -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,
             1.0f, -1.0f, 0.0f,   1.0f, 1.0f,

            -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
             1.0f, -1.0f, 0.0f,   1.0f, 1.0f,
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f,
        };
        // clang-format on
        uint32_t dataSize = ppx::SizeInBytesU32(vertexData);

        grfx::BufferCreateInfo bufferCreateInfo       = {};
        bufferCreateInfo.size                         = dataSize;
        bufferCreateInfo.usageFlags.bits.vertexBuffer = true;
        bufferCreateInfo.memoryUsage                  = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mVertexBuffer));

        void* pAddr = nullptr;
        PPX_CHECKED_CALL(ppxres = ppxres = mVertexBuffer->MapMemory(0, &pAddr));
        memcpy(pAddr, vertexData.data(), dataSize);
        mVertexBuffer->UnmapMemory();
    }

    // Viewport and scissor rect
    mViewport    = {0, 0, kWindowWidth, kWindowHeight, 0, 1};
    mScissorRect = {0, 0, kWindowWidth, kWindowHeight};
 */
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
        float    t   = GetElapsedSeconds();
        float4x4 mat = glm::rotate(t, float3(0, 0, 1));

        char* pData = nullptr;
        PPX_CHECKED_CALL(ppxres = mShaderToyUniformBuffer->MapMemory(0, reinterpret_cast<void**>(&pData)));
        {
            float2* iMouse      = reinterpret_cast<float2*>(pData + 0);
            float2* iResolution = reinterpret_cast<float2*>(pData + 16);
            float*  iTime       = reinterpret_cast<float*>(pData + 32);
            float*  iFrame      = reinterpret_cast<float*>(pData + 36);

            *iResolution = float2(mOutputImage->GetWidth(), mOutputImage->GetHeight());
            *iTime       = t;
            *iFrame      = static_cast<float>(GetFrameCount());
        }
        mShaderToyUniformBuffer->UnmapMemory();
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

        // Fill image with red
        frame.cmd->TransitionImageLayout(mOutputImage, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_SHADER_RESOURCE, grfx::RESOURCE_STATE_UNORDERED_ACCESS);
        frame.cmd->BindComputeDescriptorSets(mShaderToyPipelineInterface, 1, &mShaderToyDescriptorSet);
        PPX_ASSERT_MSG(mShaderToyIndex < mShaderToyNames.size(), "invalid sahder toy index: " << mShaderToyIndex);
        frame.cmd->BindComputePipeline(mShaderToys[mShaderToyIndex].pipeline);
        frame.cmd->Dispatch(mOutputImage->GetWidth() / kNumThreadsX, mOutputImage->GetHeight() / kNumThreadsY, 1);
        frame.cmd->TransitionImageLayout(mOutputImage, PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_UNORDERED_ACCESS, grfx::RESOURCE_STATE_SHADER_RESOURCE);

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            // Draw texture
            frame.cmd->SetScissors(grfx::Rect(0, 0, GetWindowWidth(), GetWindowHeight()));
            frame.cmd->SetViewports(grfx::Viewport(0, 0, static_cast<float>(GetWindowWidth()), static_cast<float>(GetWindowHeight())));
            frame.cmd->BindGraphicsDescriptorSets(mFullScreenPipelineInterface, 1, &mFullScreenDescriptorSet);
            frame.cmd->BindGraphicsPipeline(mFullScreenPipeline);
            frame.cmd->BindVertexBuffers(1, &mFullScreenVertexBuffer, &mFullScreenVertexBinding.GetStride());
            frame.cmd->Draw(6, 1, 0, 0);

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

    static const char* currentShaderToy = mShaderToyNames[kDefaultShaderToy];

    if (ImGui::BeginCombo("Shader Toy", currentShaderToy)) {
        for (size_t i = 0; i < mShaderToyNames.size(); ++i) {
            bool isSelected = (currentShaderToy == mShaderToyNames[i]);
            if (ImGui::Selectable(mShaderToyNames[i], isSelected)) {
                currentShaderToy = mShaderToyNames[i];
                mShaderToyIndex  = static_cast<uint32_t>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
