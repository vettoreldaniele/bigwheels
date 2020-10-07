#include "ppx/ppx.h"
using namespace ppx;

int main(int argc, char** argv)
{
    grfx::InstanceCreateInfo createInfo = {};
    createInfo.api                      = grfx::API_VK_1_1;
    createInfo.createDevices            = true;
    createInfo.enableDebug              = true;

    grfx::InstancePtr instance;
    Result            res = grfx::CreateInstance(&createInfo, &instance);
    if (res != ppx::SUCCESS) {
        return EXIT_FAILURE;
    }

    grfx::DestroyInstance(instance);

    return 0;
}
