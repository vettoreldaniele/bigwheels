// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ppx/ppx.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#if defined(USE_DX11)
const grfx::Api kApi = grfx::API_DX_11_1;
#elif defined(USE_DX12)
const grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
const grfx::Api kApi = grfx::API_VK_1_1;
#endif

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

private:
    MultiObject<grfx::Buffer>        mUniformBuffer;
    MultiObject<grfx::DescriptorSet> mDescriptorSet;
    MultiObject<grfx::CommandBuffer> cmd;
    MultiObject<grfx::Semaphore>     imageAcquiredSemaphore;
    MultiObject<grfx::Fence>         imageAcquiredFence;
    MultiObject<grfx::Semaphore>     renderCompleteSemaphore;
    MultiObject<grfx::Fence>         renderCompleteFence;

    grfx::ShaderModulePtr        mVS;
    grfx::ShaderModulePtr        mPS;
    grfx::PipelineInterfacePtr   mPipelineInterface;
    grfx::GraphicsPipelinePtr    mPipeline;
    grfx::BufferPtr              mVertexBuffer;
    grfx::DescriptorPoolPtr      mDescriptorPool;
    grfx::DescriptorSetLayoutPtr mDescriptorSetLayout;
    grfx::ImagePtr               mImage;
    grfx::SamplerPtr             mSampler;
    grfx::SampledImageViewPtr    mSampledImageView;
    grfx::Viewport               mViewport;
    grfx::Rect                   mScissorRect;
    grfx::VertexBinding          mVertexBinding;
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "03_square_textured";
    settings.enableImGui      = true;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = false;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

void ProjApp::Setup()
{
    // Uniform buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = PPX_MINIMUM_UNIFORM_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(CreatePerFrameBuffer(&bufferCreateInfo, mUniformBuffer));
    }

    // Texture image, view, and sampler
    {
        grfx_util::ImageOptions options = grfx_util::ImageOptions().MipLevelCount(PPX_REMAINING_MIP_LEVELS);
        PPX_CHECKED_CALL(grfx_util::CreateImageFromFile(GetDevice()->GetGraphicsQueue(), GetAssetPath("basic/textures/box_panel.jpg"), &mImage, options, false));

        grfx::SampledImageViewCreateInfo viewCreateInfo = grfx::SampledImageViewCreateInfo::GuessFromImage(mImage);
        PPX_CHECKED_CALL(GetDevice()->CreateSampledImageView(&viewCreateInfo, &mSampledImageView));

        grfx::SamplerCreateInfo samplerCreateInfo = {};
        PPX_CHECKED_CALL(GetDevice()->CreateSampler(&samplerCreateInfo, &mSampler));
    }

    // Descriptor
    {
        grfx::DescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.uniformBuffer                  = GetNumFramesInFlight();
        poolCreateInfo.sampledImage                   = GetNumFramesInFlight();
        poolCreateInfo.sampler                        = GetNumFramesInFlight();
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorPool(&poolCreateInfo, &mDescriptorPool));

        grfx::DescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER));
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(1, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE));
        layoutCreateInfo.bindings.push_back(grfx::DescriptorBinding(2, grfx::DESCRIPTOR_TYPE_SAMPLER));
        PPX_CHECKED_CALL(GetDevice()->CreateDescriptorSetLayout(&layoutCreateInfo, &mDescriptorSetLayout));

        PPX_CHECKED_CALL(AllocatePerFrameDescriptorSet(mDescriptorPool, mDescriptorSetLayout, mDescriptorSet));

        PerFrameWriteDescriptor perFrameWrite = {};
        perFrameWrite.binding                 = 0;
        perFrameWrite.type                    = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        perFrameWrite.bufferOffset            = 0;
        perFrameWrite.bufferRange             = PPX_WHOLE_SIZE;
        perFrameWrite.pBuffer                 = mUniformBuffer;
        PPX_CHECKED_CALL(UpdatePerFrameDescriptors(mDescriptorSet, 1, &perFrameWrite));

        perFrameWrite            = {};
        perFrameWrite.binding    = 1;
        perFrameWrite.type       = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        perFrameWrite.pImageView = mSampledImageView;
        PPX_CHECKED_CALL(UpdatePerFrameDescriptors(mDescriptorSet, 1, &perFrameWrite));

        perFrameWrite          = {};
        perFrameWrite.binding  = 2;
        perFrameWrite.type     = grfx::DESCRIPTOR_TYPE_SAMPLER;
        perFrameWrite.pSampler = mSampler;
        PPX_CHECKED_CALL(UpdatePerFrameDescriptors(mDescriptorSet, 1, &perFrameWrite));
    }

    // Pipeline
    {
        std::vector<char> bytecode = LoadShader("basic/shaders", "Texture.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

        bytecode = LoadShader("basic/shaders", "Texture.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        piCreateInfo.setCount                          = 1;
        piCreateInfo.sets[0].set                       = 0;
        piCreateInfo.sets[0].pLayout                   = mDescriptorSetLayout;
        PPX_CHECKED_CALL(GetDevice()->CreatePipelineInterface(&piCreateInfo, &mPipelineInterface));

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
        gpCreateInfo.depthReadEnable                    = false;
        gpCreateInfo.depthWriteEnable                   = false;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.pPipelineInterface                 = mPipelineInterface;
        PPX_CHECKED_CALL(GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mPipeline));
    }

    // Per frame data
    {
        PPX_CHECKED_CALL(CreatePerFrameCommandBuffer(GetGraphicsQueue(), cmd));

        grfx::SemaphoreCreateInfo semaCreateInfo = {};
        PPX_CHECKED_CALL(CreatePerFrameSemaphore(&semaCreateInfo, imageAcquiredSemaphore));

        grfx::FenceCreateInfo fenceCreateInfo = {};
        PPX_CHECKED_CALL(CreatePerFrameFence(&fenceCreateInfo, imageAcquiredFence));

        PPX_CHECKED_CALL(CreatePerFrameSemaphore(&semaCreateInfo, renderCompleteSemaphore));

        fenceCreateInfo = {true}; // Create signaled
        PPX_CHECKED_CALL(CreatePerFrameFence(&fenceCreateInfo, renderCompleteFence));
    }

    // Vertex buffer and geometry data
    {
        // clang-format off
        std::vector<float> vertexData = {
            // position           // tex coords
            -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
             0.5f, -0.5f, 0.0f,   1.0f, 1.0f,

            -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
        };
        // clang-format on
        uint32_t dataSize = ppx::SizeInBytesU32(vertexData);

        grfx::BufferCreateInfo bufferCreateInfo       = {};
        bufferCreateInfo.size                         = dataSize;
        bufferCreateInfo.usageFlags.bits.vertexBuffer = true;
        bufferCreateInfo.memoryUsage                  = grfx::MEMORY_USAGE_CPU_TO_GPU;

        PPX_CHECKED_CALL(GetDevice()->CreateBuffer(&bufferCreateInfo, &mVertexBuffer));

        void* pAddr = nullptr;
        PPX_CHECKED_CALL(mVertexBuffer->MapMemory(0, &pAddr));
        memcpy(pAddr, vertexData.data(), dataSize);
        mVertexBuffer->UnmapMemory();
    }

    // Viewport and scissor rect
    mViewport    = {0, 0, float(GetWindowWidth()), float(GetWindowHeight()), 0, 1};
    mScissorRect = {0, 0, GetWindowWidth(), GetWindowHeight()};
}

void ProjApp::Render()
{
    grfx::SwapchainPtr swapchain = GetSwapchain();

    uint32_t imageIndex = UINT32_MAX;
    PPX_CHECKED_CALL(swapchain->AcquireNextImage(UINT64_MAX, imageAcquiredSemaphore, imageAcquiredFence, &imageIndex));

    // Wait for and reset image acquired fence
    PPX_CHECKED_CALL(imageAcquiredFence->WaitAndReset());

    // Wait for and reset render complete fence
    PPX_CHECKED_CALL(renderCompleteFence->WaitAndReset());

    // Update uniform buffer
    {
        float    t   = GetElapsedSeconds();
        float4x4 mat = glm::rotate(t, float3(0, 0, 1));

        void* pData = nullptr;
        PPX_CHECKED_CALL(mUniformBuffer->MapMemory(0, &pData));
        memcpy(pData, &mat, sizeof(mat));
        mUniformBuffer->UnmapMemory();
    }

    // Build command buffer
    PPX_CHECKED_CALL(cmd->Begin());
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{0, 0, 0, 0}};

        cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        cmd->BeginRenderPass(&beginInfo);
        {
            cmd->SetScissors(1, &mScissorRect);
            cmd->SetViewports(1, &mViewport);
            cmd->BindGraphicsDescriptorSets(mPipelineInterface, 1, &mDescriptorSet);
            cmd->BindGraphicsPipeline(mPipeline);
            cmd->BindVertexBuffers(1, &mVertexBuffer, &mVertexBinding.GetStride());
            cmd->Draw(6, 1, 0, 0);

            // Draw ImGui
            DrawDebugInfo();
            DrawImGui(cmd);
        }
        cmd->EndRenderPass();
        cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_RENDER_TARGET, grfx::RESOURCE_STATE_PRESENT);
    }
    PPX_CHECKED_CALL(cmd->End());

    grfx::SubmitInfo submitInfo     = {};
    submitInfo.commandBufferCount   = 1;
    submitInfo.ppCommandBuffers     = &cmd;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.ppWaitSemaphores     = &imageAcquiredSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.ppSignalSemaphores   = &renderCompleteSemaphore;
    submitInfo.pFence               = renderCompleteFence;

    PPX_CHECKED_CALL(GetGraphicsQueue()->Submit(&submitInfo));

    PPX_CHECKED_CALL(swapchain->Present(imageIndex, 1, &renderCompleteSemaphore));
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
