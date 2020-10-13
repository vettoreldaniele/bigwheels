#include "ppx/ppx.h"
using namespace ppx;

#if defined(USE_DX)
grfx::Api kApi = grfx::API_DX_12_0;
#elif defined(USE_VK)
grfx::Api kApi = grfx::API_VK_1_1;
#endif

int main(int argc, char** argv)
{
    grfx::InstanceCreateInfo createInfo = {};
    createInfo.api                      = kApi;
    createInfo.createDevices            = true; // Tells the insteance to automatically create a device for each GPU it finds
    createInfo.enableDebug              = true; // Enable layers

    grfx::InstancePtr instance;
    Result            ppxres = grfx::CreateInstance(&createInfo, &instance);
    if (ppxres != ppx::SUCCESS) {
        PPX_ASSERT_MSG(false, "grfx::CreateInstance failed");
        return EXIT_FAILURE;
    }

    grfx::DestroyInstance(instance);

    return 0;
}
