#include "ppx/platform.h"

namespace ppx {

Platform GetPlatform()
{
#if defined(PPX_GGP)
    return ppx::PLATFORM_GGP;
#elif defined(PPX_LINUX)
    return ppx::PLATFORM_LINUX;
#elif defined(PPX_MSW)
    return ppx::PLATFORM_MSW;
#else
    return ppx::PLATFORM_UNDEFINED;
#endif
}

} // namespace ppx
