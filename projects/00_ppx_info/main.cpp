#include "ppx/ppx.h"
using namespace ppx;

#if defined(USE_DX11)
const grfx::Api kApi = grfx::API_DX_11_1;
#elif defined(USE_DX12)
const grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
const grfx::Api kApi = grfx::API_VK_1_1;
#endif

int main(int argc, char** argv)
{
    grfx::InstanceCreateInfo createInfo = {};
    createInfo.api                      = kApi;
    createInfo.createDevices            = true; // Tells the instance to automatically create a device for each GPU it finds.
    createInfo.enableDebug              = true; // Enable layers.

    grfx::InstancePtr instance;
    Result            ppxres = grfx::CreateInstance(&createInfo, &instance);
    if (ppxres != ppx::SUCCESS) {
        PPX_ASSERT_MSG(false, "grfx::CreateInstance failed");
        return EXIT_FAILURE;
    }

    ppx::Log::Initialize(LOG_MODE_CONSOLE);
    PPX_LOG_INFO("Graphics instance and devices created successfully.");

    grfx::DestroyInstance(instance);

    return 0;
}
