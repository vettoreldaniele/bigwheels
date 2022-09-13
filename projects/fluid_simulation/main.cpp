// Fluid simulation.
//
// This is adapted from https://github.com/PavelDoGreat/WebGL-Fluid-Simulation.

#include "ppx/000_config.h"
#include "ppx/000_math_config.h"
#include "ppx/graphics_util.h"
#include "ppx/grfx/000_grfx_config.h"
#include "ppx/grfx/grfx_enums.h"
#include "ppx/log.h"
#include "ppx/ppx.h"

#if defined(USE_DX11)
const ppx::grfx::Api kApi = ppx::grfx::API_DX_11_1;
#elif defined(USE_DX12)
const ppx::grfx::Api kApi = ppx::grfx::API_DX_12_0;
#elif defined(USE_VK)
const ppx::grfx::Api kApi = ppx::grfx::API_VK_1_1;
#endif

class ProjApp;

namespace {

class FilterShader
{
public:
    FilterShader(ProjApp* app, const std::string& shaderFile, ppx::float2 filterStep);

    // Move the filter area to a new location.  When it reaches any window border, make
    // it bounce by reversing direction.
    void UpdateFilterLocation();

    // Create a compute descriptor set inside @param ds that uses @param srcImage to sample
    // and @param dstImage to produce output.
    void CreateComputeDescriptor(ppx::grfx::DescriptorSet** ds, ppx::grfx::DescriptorSetLayoutPtr dsLayout, ppx::grfx::ImagePtr srcImage, ppx::grfx::ImagePtr dstImage);

    const ppx::grfx::PipelineInterfacePtr GetComputePipelineInterface() const { return mPipelineInterface; };
    const ppx::grfx::ComputePipelinePtr   GetComputePipeline() const { return mPipeline; };
    const ppx::float2&                    GetArea() const { return mCSInput.filterArea; }

    ppx::grfx::DescriptorSet** GetComputeDescriptorSetPtr(size_t ix)
    {
        PPX_ASSERT_MSG(ix < 2, "Invalid descriptor set index " << ix);
        return &mDescriptorSets[ix];
    }

private:
    ProjApp*                        mApp;
    std::string                     mShaderFile;
    ppx::grfx::ShaderModulePtr      mCS;
    ppx::grfx::DescriptorSet*       mDescriptorSets[2];
    ppx::grfx::PipelineInterfacePtr mPipelineInterface;
    ppx::grfx::ComputePipelinePtr   mPipeline;

    // Filtering parameters for the compute shader.
    ppx::grfx::BufferPtr mParams;

    // The sampler used by the compute shader to filter the image.
    ppx::grfx::SamplerPtr mSampler;

    // Parameters for the compute shader.
    struct alignas(16) ComputeShaderInput
    {
        ppx::float2 filterCenter;
        ppx::float2 filterArea;
        ppx::float2 filterTopLeft;
        ppx::float2 texelSize;

        // Used by bloom_final.
        float intensity;

        // Curve and threshold - used by bloom_prefilter.hlsl.
        ppx::float3 curve;
        float       threshold;

        // Aspect ratio - used by checkerboard.hlsl.
        float aspectRatio;

        // Clear value - used by clear.hlsl.
        float clearValue;

        // Color value - used by color.hlsl.
        ppx::float4 color;

        // Dither scale - used by display.hlsl.
        ppx::float2 ditherScale;

        // Dye texel size, dt and dissipation - used by advection.hlsl.
        ppx::float2 dyeTexelSize;
        float       dissipation;

        // used by advection.hlsl and vorticity.hlsl
        float dt;

        // Coordinate and radius - used by splat.hlsl
        ppx::float2 coordinate;
        float       radius;

        // Weight - used by sunrays.hlsl
        float weight;

        // curl - used by vorticity.hlsl.
        float curl;
    };
    ComputeShaderInput mCSInput;

    // The center of the filtered area moves around the window and "bounces off" the
    // window edges.  This represents how far (in pixels) the center of the filter moves
    // at each step of the simulation.
    ppx::float2 mFilterStep;
};

} // namespace

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

    ppx::grfx::ImagePtr          GetOriginalImage() { return mOriginalImage; }
    ppx::grfx::DescriptorPoolPtr GetDescriptorPool() { return mDescriptorPool; }
    ppx::grfx::ImagePtr          GetFilteredImage() { return mFilteredImage; }

private:
    struct PerFrame
    {
        ppx::grfx::CommandBufferPtr cmd;
        ppx::grfx::SemaphorePtr     imageAcquiredSemaphore;
        ppx::grfx::FencePtr         imageAcquiredFence;
        ppx::grfx::SemaphorePtr     renderCompleteSemaphore;
        ppx::grfx::FencePtr         renderCompleteFence;
    };

    std::vector<PerFrame>           mPerFrame;
    ppx::grfx::DescriptorPoolPtr    mDescriptorPool;
    ppx::grfx::ShaderModulePtr      mVS;
    ppx::grfx::ShaderModulePtr      mPS;
    ppx::grfx::PipelineInterfacePtr mPipelineInterface;
    ppx::grfx::GraphicsPipelinePtr  mPipeline;
    ppx::grfx::BufferPtr            mVertexBuffer;
    ppx::grfx::Viewport             mViewport;
    ppx::grfx::Rect                 mScissorRect;
    ppx::grfx::VertexBinding        mVertexBinding;

    // Textures loaded from disk.
    ppx::grfx::ImagePtr            mOriginalImage;
    ppx::grfx::SampledImageViewPtr mPresentImageView;

    // This is the image transferred from mOriginalImage (loaded from disk) which is
    // filtered by the compute shader. It's passed to the compute shader, filtered and
    // then retrieved to pass on to the full screen draw pipeline to be presented.
    ppx::grfx::ImagePtr mFilteredImage;

    // For drawing into the swapchain
    ppx::grfx::DescriptorSetLayoutPtr mDrawToSwapchainLayout;
    ppx::grfx::DescriptorSetPtr       mDrawToSwapchainSet;
    ppx::grfx::FullscreenQuadPtr      mDrawToSwapchain;
    ppx::grfx::SamplerPtr             mDrawToSwapchainSampler;

    // Filters to apply.
    std::vector<std::unique_ptr<FilterShader>> mFilters;

    void SetupDrawToSwapchain();
    void SetupFilters();
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName                        = "fluid_simulation";
    settings.enableImGui                    = false;
    settings.grfx.api                       = kApi;
    settings.grfx.enableDebug               = true;
    settings.grfx.device.graphicsQueueCount = 1;
    settings.grfx.numFramesInFlight         = 1;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::Setup()
{
    // Create descriptor pool (for both pipelines)
    {
        ppx::grfx::DescriptorPoolCreateInfo createInfo = {};
        createInfo.sampler                             = 20;
        createInfo.sampledImage                        = 20;
        createInfo.uniformBuffer                       = 20;
        createInfo.storageImage                        = 20;
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorPool(&createInfo, &mDescriptorPool));
    }

    // To present the image on screen.
    SetupDrawToSwapchain();

    // Per frame data
    {
        PerFrame frame = {};

        PPX_CHECKED_CALL(GetGraphicsQueue()->CreateCommandBuffer(&frame.cmd));

        ppx::grfx::SemaphoreCreateInfo semaCreateInfo = {};
        PPX_CHECKED_CALL(GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.imageAcquiredSemaphore));

        ppx::grfx::FenceCreateInfo fenceCreateInfo = {};
        PPX_CHECKED_CALL(GetDevice()->CreateFence(&fenceCreateInfo, &frame.imageAcquiredFence));

        PPX_CHECKED_CALL(GetDevice()->CreateSemaphore(&semaCreateInfo, &frame.renderCompleteSemaphore));

        fenceCreateInfo = {true}; // Create signaled
        PPX_CHECKED_CALL(GetDevice()->CreateFence(&fenceCreateInfo, &frame.renderCompleteFence));

        mPerFrame.push_back(frame);
    }

    mViewport    = {0, 0, float(GetWindowWidth()), float(GetWindowHeight()), 0, 1};
    mScissorRect = {0, 0, GetWindowWidth(), GetWindowHeight()};

    // Set up all the filters to use.
    SetupFilters();
}

void FilterShader::CreateComputeDescriptor(ppx::grfx::DescriptorSet** ds, ppx::grfx::DescriptorSetLayoutPtr dsLayout, ppx::grfx::ImagePtr srcImage, ppx::grfx::ImagePtr dstImage)
{
    PPX_CHECKED_CALL(mApp->GetDevice()->AllocateDescriptorSet(mApp->GetDescriptorPool(), dsLayout, ds));

    constexpr int              numBindings = 4;
    ppx::grfx::WriteDescriptor write[4];

    write[0].binding      = 0;
    write[0].type         = ppx::grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write[0].bufferOffset = 0;
    write[0].bufferRange  = PPX_WHOLE_SIZE;
    write[0].pBuffer      = mParams;

    write[1].binding  = 1;
    write[1].type     = ppx::grfx::DESCRIPTOR_TYPE_SAMPLER;
    write[1].pSampler = mSampler;

    ppx::grfx::SampledImageViewPtr        inputView;
    ppx::grfx::SampledImageViewCreateInfo sampledViewCreateInfo = ppx::grfx::SampledImageViewCreateInfo::GuessFromImage(srcImage);
    PPX_CHECKED_CALL(mApp->GetDevice()->CreateSampledImageView(&sampledViewCreateInfo, &inputView));
    write[2].binding    = 2;
    write[2].type       = ppx::grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write[2].pImageView = inputView;

    ppx::grfx::StorageImageViewPtr        outputView;
    ppx::grfx::StorageImageViewCreateInfo storageViewCreateInfo = ppx::grfx::StorageImageViewCreateInfo::GuessFromImage(dstImage);
    PPX_CHECKED_CALL(mApp->GetDevice()->CreateStorageImageView(&storageViewCreateInfo, &outputView));
    write[3].binding    = 3;
    write[3].type       = ppx::grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write[3].pImageView = outputView;
    PPX_CHECKED_CALL((*ds)->UpdateDescriptors(numBindings, write));
}

FilterShader::FilterShader(ProjApp* app, const std::string& shaderFile, ppx::float2 filterStep)
    : mApp(app), mShaderFile(shaderFile), mFilterStep(filterStep)
{
    // Uniform buffer for parameters.
    {
        ppx::grfx::BufferCreateInfo bufferCreateInfo   = {};
        bufferCreateInfo.size                          = PPX_MINIMUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = ppx::grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(mApp->GetDevice()->CreateBuffer(&bufferCreateInfo, &mParams));
    }

    // Texture sampler.
    {
        ppx::grfx::SamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.magFilter                    = ppx::grfx::FILTER_NEAREST;
        samplerCreateInfo.minFilter                    = ppx::grfx::FILTER_NEAREST;
        samplerCreateInfo.mipmapMode                   = ppx::grfx::SAMPLER_MIPMAP_MODE_NEAREST;
        samplerCreateInfo.minLod                       = 0.0f;
        samplerCreateInfo.maxLod                       = FLT_MAX;
        PPX_CHECKED_CALL(mApp->GetDevice()->CreateSampler(&samplerCreateInfo, &mSampler));
    }

    // Descriptor set layout.
    ppx::grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    ppx::grfx::DescriptorSetLayoutPtr        dsLayout;
    layoutCreateInfo.bindings.push_back(ppx::grfx::DescriptorBinding(0, ppx::grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    layoutCreateInfo.bindings.push_back(ppx::grfx::DescriptorBinding(1, ppx::grfx::DESCRIPTOR_TYPE_SAMPLER));
    layoutCreateInfo.bindings.push_back(ppx::grfx::DescriptorBinding(2, ppx::grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
    layoutCreateInfo.bindings.push_back(ppx::grfx::DescriptorBinding(3, ppx::grfx::DESCRIPTOR_TYPE_STORAGE_IMAGE));
    PPX_CHECKED_CALL(mApp->GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &dsLayout));

    // Compute descriptors.  The first one is for sampling from the original image and
    // output into the filtered image.  The second samples from the filtered image and writes
    // to the original image.
    CreateComputeDescriptor(&mDescriptorSets[0], dsLayout, mApp->GetOriginalImage(), mApp->GetFilteredImage());
    CreateComputeDescriptor(&mDescriptorSets[1], dsLayout, mApp->GetFilteredImage(), mApp->GetOriginalImage());

    // Compute pipeline.
    {
        std::vector<char> bytecode = mApp->LoadShader(mApp->GetAssetPath("fluid_simulation/shaders"), mShaderFile + ".cs");
        PPX_ASSERT_MSG(!bytecode.empty(), "CS shader bytecode load failed");
        ppx::grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(mApp->GetDevice()->CreateShaderModule(&shaderCreateInfo, &mCS));

        ppx::grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                               = 1;
        piCreateInfo.sets[0].set                            = 0;
        piCreateInfo.sets[0].pLayout                        = dsLayout;
        PPX_CHECKED_CALL(mApp->GetDevice()->CreatePipelineInterface(&piCreateInfo, &mPipelineInterface));

        ppx::grfx::ComputePipelineCreateInfo cpCreateInfo = {};
        cpCreateInfo.CS                                   = {mCS.Get(), "csmain"};
        cpCreateInfo.pPipelineInterface                   = mPipelineInterface;
        PPX_CHECKED_CALL(mApp->GetDevice()->CreateComputePipeline(&cpCreateInfo, &mPipeline));
    }

    // Initialize values for the compute shader arguments.
    mCSInput.texelSize.x  = 1.0f / float(mApp->GetFilteredImage()->GetWidth());
    mCSInput.texelSize.y  = 1.0f / float(mApp->GetFilteredImage()->GetHeight());
    mCSInput.filterCenter = ppx::float2(mApp->GetFilteredImage()->GetWidth() / 2, mApp->GetFilteredImage()->GetHeight() / 2);
    mCSInput.filterArea   = ppx::float2(100, 100);
    mCSInput.intensity    = 0.0;
    mCSInput.curve        = ppx::float3(1, 8, 3);
    mCSInput.threshold    = 9;
    mCSInput.aspectRatio  = 2.8;
    mCSInput.clearValue   = 1.3;
    mCSInput.color        = ppx::float4(0.0, 0.1, 0.1, 1);
    mCSInput.ditherScale  = ppx::float2(1.1, 3.4);
    mCSInput.dyeTexelSize = mCSInput.texelSize * 1.2f;
    mCSInput.dt           = 0.3f;
    mCSInput.dissipation  = 3.4f;
    mCSInput.coordinate   = ppx::float2(0.3, -0.2);
    mCSInput.radius       = 0.8;
    mCSInput.weight       = 42.0;
    mCSInput.curl         = 32.3;
}

void ProjApp::SetupFilters()
{
    mFilters.push_back(std::make_unique<FilterShader>(this, "advection", ppx::float2(2, 2)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "bloom_blur", ppx::float2(1, -1)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "bloom_final", ppx::float2(3, -1)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "bloom_prefilter", ppx::float2(-1, -1)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "blur", ppx::float2(-3, -2)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "blur", ppx::float2(3, 2)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "checkerboard", ppx::float2(9, -4)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "clear", ppx::float2(1, -12)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "color", ppx::float2(10, -7)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "copy", ppx::float2(9, -6)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "curl", ppx::float2(4, -17)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "display", ppx::float2(19, -13)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "divergence", ppx::float2(7, -5)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "gradient_subtract", ppx::float2(8, -16)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "pressure", ppx::float2(23, -4)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "splat", ppx::float2(-5, -18)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "sunrays", ppx::float2(12.12, -16)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "sunrays_mask", ppx::float2(15, -20)));
    mFilters.push_back(std::make_unique<FilterShader>(this, "vorticity", ppx::float2(23, -22)));
    for (auto& filter : mFilters) {
        filter->UpdateFilterLocation();
    }
}

void ProjApp::SetupDrawToSwapchain()
{
    // Create original image.
    ppx::grfx_util::ImageOptions options = ppx::grfx_util::ImageOptions().AdditionalUsage(ppx::grfx::IMAGE_USAGE_STORAGE).MipLevelCount(1);
    PPX_CHECKED_CALL(ppx::grfx_util::CreateImageFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("benchmarks/textures/test_image_1280x720.jpg"), &mOriginalImage, options, false));

    // Create filtered image.
    {
        ppx::grfx::ImageCreateInfo ci  = {};
        ci.type                        = ppx::grfx::IMAGE_TYPE_2D;
        ci.width                       = mOriginalImage->GetWidth();
        ci.height                      = mOriginalImage->GetHeight();
        ci.depth                       = 1;
        ci.format                      = mOriginalImage->GetFormat();
        ci.sampleCount                 = ppx::grfx::SAMPLE_COUNT_1;
        ci.mipLevelCount               = mOriginalImage->GetMipLevelCount();
        ci.arrayLayerCount             = 1;
        ci.usageFlags.bits.transferDst = true;
        ci.usageFlags.bits.transferSrc = true; // For CS
        ci.usageFlags.bits.sampled     = true;
        ci.usageFlags.bits.storage     = true; // For CS
        ci.memoryUsage                 = ppx::grfx::MEMORY_USAGE_GPU_ONLY;
        ci.initialState                = ppx::grfx::RESOURCE_STATE_SHADER_RESOURCE;

        PPX_CHECKED_CALL(GetDevice()->CreateImage(&ci, &mFilteredImage));
    }

    // Image and sampler.
    {
        ppx::grfx::SampledImageViewCreateInfo presentViewCreateInfo = ppx::grfx::SampledImageViewCreateInfo::GuessFromImage(mFilteredImage);
        PPX_CHECKED_CALL(GetDevice()->CreateSampledImageView(&presentViewCreateInfo, &mPresentImageView));

        ppx::grfx::SamplerCreateInfo createInfo = {};
        createInfo.magFilter                    = ppx::grfx::FILTER_NEAREST;
        createInfo.minFilter                    = ppx::grfx::FILTER_NEAREST;
        createInfo.mipmapMode                   = ppx::grfx::SAMPLER_MIPMAP_MODE_NEAREST;
        createInfo.minLod                       = 0.0f;
        createInfo.maxLod                       = FLT_MAX;
        PPX_CHECKED_CALL(GetDevice()->CreateSampler(&createInfo, &mDrawToSwapchainSampler));
    }

    // Descriptor set layout
    {
        ppx::grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(ppx::grfx::DescriptorBinding(0, ppx::grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
        layoutCreateInfo.bindings.push_back(ppx::grfx::DescriptorBinding(1, ppx::grfx::DESCRIPTOR_TYPE_SAMPLER));
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDrawToSwapchainLayout));
    }

    // Pipeline
    {
        ppx::grfx::ShaderModulePtr VS;
        std::vector<char>          bytecode = LoadShader(GetAssetPath("basic/shaders"), "FullScreenTriangle.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        ppx::grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &VS));

        ppx::grfx::ShaderModulePtr PS;
        bytecode = LoadShader(GetAssetPath("basic/shaders"), "FullScreenTriangle.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &PS));

        ppx::grfx::FullscreenQuadCreateInfo createInfo = {};
        createInfo.VS                                  = VS;
        createInfo.PS                                  = PS;
        createInfo.setCount                            = 1;
        createInfo.sets[0].set                         = 0;
        createInfo.sets[0].pLayout                     = mDrawToSwapchainLayout;
        createInfo.renderTargetCount                   = 1;
        createInfo.renderTargetFormats[0]              = GetSwapchain()->GetColorFormat();
        createInfo.depthStencilFormat                  = GetSwapchain()->GetDepthFormat();

        PPX_CHECKED_CALL(GetDevice()->CreateFullscreenQuad(&createInfo, &mDrawToSwapchain));
    }

    // Allocate descriptor set
    PPX_CHECKED_CALL(GetDevice()->AllocateDescriptorSet(mDescriptorPool, mDrawToSwapchainLayout, &mDrawToSwapchainSet));

    // Update descriptors
    {
        ppx::grfx::WriteDescriptor writes[2] = {};
        writes[0].binding                    = 0;
        writes[0].arrayIndex                 = 0;
        writes[0].type                       = ppx::grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[0].pImageView                 = mPresentImageView;

        writes[1].binding  = 1;
        writes[1].type     = ppx::grfx::DESCRIPTOR_TYPE_SAMPLER;
        writes[1].pSampler = mDrawToSwapchainSampler;

        PPX_CHECKED_CALL(mDrawToSwapchainSet->UpdateDescriptors(2, writes));
    }
}

void FilterShader::UpdateFilterLocation()
{
    uint32_t maxWidth  = mApp->GetFilteredImage()->GetWidth();
    uint32_t maxHeight = mApp->GetFilteredImage()->GetHeight();

    // Move the filter.  If it reaches the edge, make it bounce.
    mCSInput.filterCenter.x += mFilterStep.x;
    mCSInput.filterCenter.y += mFilterStep.y;

    // Bounce back from the right and left edges.
    if (mCSInput.filterCenter.x + mCSInput.filterArea.x / 2 > maxWidth) {
        mFilterStep.x *= -1;
        mCSInput.filterCenter.x = maxWidth - mCSInput.filterArea.x / 2;
    }
    else if (mCSInput.filterCenter.x - mCSInput.filterArea.x / 2 < 0) {
        mFilterStep.x *= -1;
        mCSInput.filterCenter.x = mCSInput.filterArea.x / 2;
    }

    // Bounce back from the top and bottom edges.
    if (mCSInput.filterCenter.y + mCSInput.filterArea.y / 2 > maxHeight) {
        mFilterStep.y *= -1;
        mCSInput.filterCenter.y = maxHeight - mCSInput.filterArea.y / 2;
    }
    else if (mCSInput.filterCenter.y - mCSInput.filterArea.y / 2 < 0) {
        mFilterStep.y *= -1;
        mCSInput.filterCenter.y = mCSInput.filterArea.y / 2;
    }

    // Compute the top left corner of the filter area.
    mCSInput.filterTopLeft = mCSInput.filterCenter - mCSInput.filterArea / 2.0f;

    // Update intensity.
    mCSInput.intensity += .1;
    if (mCSInput.intensity > 3)
        mCSInput.intensity = 0.2;
    mCSInput.curve += 0.5f;
    mCSInput.threshold += .05;

    // Update the color.
    mCSInput.color += .01f;
    if (mCSInput.color.r > 1.0f)
        mCSInput.color = ppx::float4(0.0, 0.1, 0.1, 1);

    // Update the uniform buffer with the new coordinates.
    void* pData = nullptr;
    PPX_CHECKED_CALL(mParams->MapMemory(0, &pData));
    memcpy(pData, &mCSInput, sizeof(mCSInput));
    mParams->UnmapMemory();
}

void ProjApp::Render()
{
    PerFrame& frame = mPerFrame[0];

    ppx::grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(swapchain->AcquireNextImage(UINT64_MAX, frame.imageAcquiredSemaphore, frame.imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(frame.imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(frame.renderCompleteFence->WaitAndReset());

    // Update the uniform buffer with parameters for the compute shader.
    for (auto& filter : mFilters) {
        filter->UpdateFilterLocation();
    }

    // Build command buffer
    PPX_CHECKED_CALL(frame.cmd->Begin());
    {
        // Filter the image with the compute shader.
        size_t dsIx = 0;
        for (auto& filter : mFilters) {
            if (dsIx == 0) {
                frame.cmd->TransitionImageLayout(mFilteredImage, PPX_ALL_SUBRESOURCES, ppx::grfx::RESOURCE_STATE_SHADER_RESOURCE, ppx::grfx::RESOURCE_STATE_UNORDERED_ACCESS);
            }
            else {
                frame.cmd->TransitionImageLayout(mOriginalImage, PPX_ALL_SUBRESOURCES, ppx::grfx::RESOURCE_STATE_SHADER_RESOURCE, ppx::grfx::RESOURCE_STATE_UNORDERED_ACCESS);
            }
            frame.cmd->BindComputeDescriptorSets(filter->GetComputePipelineInterface(), 1, filter->GetComputeDescriptorSetPtr(dsIx));
            frame.cmd->BindComputePipeline(filter->GetComputePipeline());
            frame.cmd->Dispatch(filter->GetArea().x, filter->GetArea().y, 1);
            if (dsIx == 0) {
                frame.cmd->TransitionImageLayout(mFilteredImage, PPX_ALL_SUBRESOURCES, ppx::grfx::RESOURCE_STATE_UNORDERED_ACCESS, ppx::grfx::RESOURCE_STATE_SHADER_RESOURCE);
            }
            else {
                frame.cmd->TransitionImageLayout(mOriginalImage, PPX_ALL_SUBRESOURCES, ppx::grfx::RESOURCE_STATE_UNORDERED_ACCESS, ppx::grfx::RESOURCE_STATE_SHADER_RESOURCE);
            }
            dsIx = (dsIx + 1) % 2;
        }

        ppx::grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");
        frame.cmd->SetScissors(renderPass->GetScissor());
        frame.cmd->SetViewports(renderPass->GetViewport());
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, ppx::grfx::RESOURCE_STATE_PRESENT, ppx::grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(renderPass);
        {
            // Draw the render target output to the swapchain.
            frame.cmd->Draw(mDrawToSwapchain, 1, &mDrawToSwapchainSet);
        }
        frame.cmd->EndRenderPass();

        // Resolve queries.
        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, ppx::grfx::RESOURCE_STATE_RENDER_TARGET, ppx::grfx::RESOURCE_STATE_PRESENT);
    }
    PPX_CHECKED_CALL(frame.cmd->End());

    ppx::grfx::SubmitInfo submitInfo = {};
    submitInfo.commandBufferCount    = 1;
    submitInfo.ppCommandBuffers      = &frame.cmd;
    submitInfo.waitSemaphoreCount    = 1;
    submitInfo.ppWaitSemaphores      = &frame.imageAcquiredSemaphore;
    submitInfo.signalSemaphoreCount  = 1;
    submitInfo.ppSignalSemaphores    = &frame.renderCompleteSemaphore;
    submitInfo.pFence                = frame.renderCompleteFence;

    PPX_CHECKED_CALL(GetGraphicsQueue()->Submit(&submitInfo));

    PPX_CHECKED_CALL(swapchain->Present(imageIndex, 1, &frame.renderCompleteSemaphore));
}

int main(int argc, char** argv)
{
    ProjApp app;
    return app.Run(argc, argv);
}
