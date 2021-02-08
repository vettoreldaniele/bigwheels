#include "ppx/ppx.h"
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
    virtual void Render() override;

private:

    Result TryAllocateRange(uint32_t rangeStart, uint32_t rangeEnd, uint32_t usageFlags);
    ppx::grfx::PipelineInterfacePtr mPipelineInterface;
    ppx::grfx::GraphicsPipelinePtr  mPipeline;
    ppx::grfx::BufferPtr            mVertexBuffer;
};

void ProjApp::Config(ppx::ApplicationSettings& settings)
{
    settings.appName          = "alloc";
    settings.window.width     = kWindowWidth;
    settings.window.height    = kWindowHeight;
    settings.grfx.api         = kApi;
    settings.grfx.enableDebug = true;
#if defined(USE_DXIL)
    settings.grfx.enableDXIL = true;
#endif
}

static void YayOrNay(uint32_t first, uint32_t last, const char* status)
{
    if (first == last)
    {
        fprintf(stderr, "Size %d %s.\n", first, status);
    }
    else
    {
        fprintf(stderr, "Sizes %d through %d %s.\n", first, last, status);
    }

}

static void PrintRange(bool state, uint32_t first, uint32_t last)
{
    if (state)
    {
        YayOrNay(first, last, "succeeded");
    }
    else
    {
        YayOrNay(first, last, "failed");
    }
}

Result ProjApp::TryAllocateRange(uint32_t rangeStart, uint32_t rangeEnd, uint32_t usageFlags)
{
    Result ppxres = ppx::SUCCESS;
    bool state;
    uint32_t first;
    uint32_t last;
    for (uint32_t i = rangeStart; i <= rangeEnd; i *= 2)
    {
        grfx::BufferCreateInfo bufferCreateInfo       = {};
        bufferCreateInfo.size                         = i;
        bufferCreateInfo.usageFlags.flags             = usageFlags;
        bufferCreateInfo.memoryUsage                  = grfx::MEMORY_USAGE_CPU_TO_GPU;
        //        bufferCreateInfo.initialState                 = grfx::RESOURCE_STATE_UNIFORM_BUFFER;
        // We allocate a buffer of size i and see if it succeeds. If there's
        // a change, we print the range of unchanging success/failure runs
        // that came before.
        ppx::grfx::BufferPtr            buffer;
        ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &buffer);
        bool didSucceed = (ppxres == ppx::SUCCESS) ? true : false;
        fprintf(stderr, "%d ", didSucceed);

        if (i == rangeStart)
        {
            state = didSucceed;
            first = i;
            continue;
        }
        if (state != didSucceed)
        {
            PrintRange(state, first, last);
            first = i;
        }
        state = didSucceed;
        last = i;
        GetDevice()->DestroyBuffer(buffer);
    }
    PrintRange(state, first, last);
    return ppx::SUCCESS;
}

struct Range {
    uint32_t start;
    uint32_t end;
};

void ProjApp::Setup()
{
    Result ppxres = ppx::SUCCESS;

    grfx::BufferUsageFlags usageFlags;

    Range range = { 4, 256 * 1024 * 1024 };  // 4, 256MB
    fprintf(stderr, "Trying uniform buffer allocations in [%d %d] in powers of 2.\n", range.start, range.end);
    usageFlags.bits.uniformBuffer = 1;
    TryAllocateRange(range.start, range.end, usageFlags);

    range.start = 4;
    range.end = 256;

    usageFlags.flags = 0;
    fprintf(stderr, "Trying storage texel buffer allocations in [%d %d] in powers of 2.\n", range.start, range.end);
    usageFlags.bits.storageTexelBuffer = 1;
    TryAllocateRange(range.start, range.end, usageFlags);
    usageFlags.flags = 0;
    fprintf(stderr, "Trying storage buffer allocations in [%d %d] in powers of 2.\n", range.start, range.end);
    usageFlags.bits.storageBuffer = 1;
    TryAllocateRange(range.start, range.end, usageFlags);
    usageFlags.flags = 0;
    fprintf(stderr, "Trying uniform texel buffer allocations in [%d %d] in powers of 2.\n", range.start, range.end);
    usageFlags.bits.uniformTexelBuffer = 1;
    TryAllocateRange(range.start, range.end, usageFlags);
}

void ProjApp::Render()
{
}

int main(int argc, char** argv)
{
    ProjApp app;

    int res = app.Run(argc, argv);

    return res;
}
