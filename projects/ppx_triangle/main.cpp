#include "ppx/ppx.h"
using namespace ppx;

class ProjApp
    : public ppx::Application
{
public:
    virtual void Config(ppx::ApplicationSettings& settings) override;
    virtual void Setup() override;
    virtual void Render() override;

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
    ppx::grfx::ShaderModulePtr      mVS;
    ppx::grfx::ShaderModulePtr      mPS;
    ppx::grfx::PipelineInterfacePtr mPipelineInterface;
    ppx::grfx::GraphicsPipelinePtr  mPipeline;
    ppx::grfx::BufferPtr            mVertexBuffer;
    grfx::Viewport                  mViewport;
    grfx::Rect                      mScissorRect;
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "PPX Triangle";
    settings.grfx.api         = ppx::grfx::API_VK;
    settings.grfx.enableDebug = true;
}

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    // Pipeline
    {
        std::vector<char> bytecode = fs::load_file("C:\\code\\hai\\BigWheels\\assets\\shaders\\spv\\VertexColor.vs.spv");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mVS));

        bytecode = fs::load_file("C:\\code\\hai\\BigWheels\\assets\\shaders\\spv\\VertexColor.PS.spv");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateShaderModule(&shaderCreateInfo, &mPS));

        grfx::PipelineInterfaceCreateInfo piCreateInfo = {};
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreatePipelineInterface(&piCreateInfo, &mPipelineInterface));

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo  = {};
        gpCreateInfo.VS                                 = {mVS.Get(), "VSMain"};
        gpCreateInfo.PS                                 = {mPS.Get(), "PSMain"};
        gpCreateInfo.vertexInputState.attributeCount    = 2;
        gpCreateInfo.vertexInputState.attributes[0]     = {0, grfx::FORMAT_R32G32B32A32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX};
        gpCreateInfo.vertexInputState.attributes[1]     = {1, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, grfx::VERTEX_INPUT_RATE_VERTEX};
        gpCreateInfo.topology                           = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                        = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                           = grfx::CULL_MODE_NONE;
        gpCreateInfo.frontFace                          = grfx::FRONT_FACE_CCW;
        gpCreateInfo.blendModes[0]                      = grfx::BLEND_MODE_NONE;
        gpCreateInfo.outputState.renderTargetCount      = 1;
        gpCreateInfo.outputState.renderTargetFormats[0] = GetSwapchain()->GetColorFormat();
        gpCreateInfo.pPipelineInterface                 = mPipelineInterface;
        PPX_CHECKED_CALL(ppxres = GetDevice()->CreateGraphicsPipeline(&gpCreateInfo, &mPipeline));
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

    // Buffer and geometry data
    {
        // clang-format off
        std::vector<float> vertexData = {
             0.0f,  0.5f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
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

    mViewport    = {0, 0, 640, 480, 0, 1};
    mScissorRect = {0, 0, 640, 480};
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

    // Build command buffer
    PPX_CHECKED_CALL(ppxres = frame.cmd->Begin());
    {
        grfx::RenderPassPtr renderPass = swapchain->GetRenderPass(imageIndex);
        PPX_ASSERT_MSG(!renderPass.IsNull(), "render pass object is null");

        grfx::RenderPassBeginInfo beginInfo = {};
        beginInfo.pRenderPass               = renderPass;
        beginInfo.renderArea                = renderPass->GetRenderArea();
        beginInfo.RTVClearCount             = 1;
        beginInfo.RTVClearValues[0]         = {{1, 0, 0, 1}};

        frame.cmd->TransitionImageLayout(renderPass->GetRenderTargetImage(0), PPX_ALL_SUBRESOURCES, grfx::RESOURCE_STATE_PRESENT, grfx::RESOURCE_STATE_RENDER_TARGET);
        frame.cmd->BeginRenderPass(&beginInfo);
        {
            frame.cmd->SetScissors(1, &mScissorRect);
            frame.cmd->SetViewports(1, &mViewport);
            frame.cmd->BindVertexBuffers(1, &mVertexBuffer);
            frame.cmd->BindGraphicsPipeline(mPipeline);
            frame.cmd->Draw(3, 1, 0, 0);
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

    PPX_CHECKED_CALL(ppxres = GetGraphicsQueue()->Present(swapchain, imageIndex, 1, &frame.renderCompleteSemaphore));
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
