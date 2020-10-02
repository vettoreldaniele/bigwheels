#ifndef ppx_platform_h
#define ppx_platform_h

namespace ppx {

enum Platform
{
    PLATFORM_UNDEFINED = 0,
    PLATFORM_GGP,
    PLATFORM_LINUX,
    PLATFORM_MSW,
};

Platform GetPlatform();

} // namespace ppx

#endif // ppx_platform_h
