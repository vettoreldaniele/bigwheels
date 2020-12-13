#ifndef ppx_platform_h
#define ppx_platform_h

#include "ppx/000_config.h"

namespace ppx {

enum Platform
{
    PLATFORM_UNDEFINED = 0,
    PLATFORM_GGP,
    PLATFORM_LINUX,
    PLATFORM_MSW,
};

class CpuInfo
{
public:
    struct Features
    {
        bool sse                 = false;
        bool sse2                = false;
        bool sse3                = false;
        bool ssse3               = false;
        bool sse4_1              = false;
        bool sse4_2              = false;
        bool sse4a               = false;
        bool avx                 = false;
        bool avx2                = false;
        bool avx512f             = false;
        bool avx512cd            = false;
        bool avx512er            = false;
        bool avx512pf            = false;
        bool avx512bw            = false;
        bool avx512dq            = false;
        bool avx512vl            = false;
        bool avx512ifma          = false;
        bool avx512vbmi          = false;
        bool avx512vbmi2         = false;
        bool avx512vnni          = false;
        bool avx512bitalg        = false;
        bool avx512vpopcntdq     = false;
        bool avx512_4vnniw       = false;
        bool avx512_4vbmi2       = false;
        bool avx512_second_fma   = false;
        bool avx512_4fmaps       = false;
        bool avx512_bf16         = false;
        bool avx512_vp2intersect = false;
        bool amx_bf16            = false;
        bool amx_tile            = false;
        bool amx_int8            = false;
    };

    // ---------------------------------------------------------------------------------------------

    CpuInfo() {}
    ~CpuInfo() {}

    const std::string& GetBrandString() const { return mBrandString; }
    const std::string& GetVendorString() const { return mVendorString; }
    const std::string& GetArchitectureString() const { return mArchitectureString; }
    uint32_t           GetL1CacheSize() const { return mL1CacheSize; }
    uint32_t           GetL2CacheSize() const { return mL2CacheSize; }
    uint32_t           GetL3CacheSize() const { return mL3CacheSize; }
    uint32_t           GetL1CacheLineSize() const { return mL1CacheLineSize; }
    uint32_t           GetL2CacheLineSize() const { return mL2CacheLineSize; }
    uint32_t           GetL3CacheLineSize() const { return mL3CacheLineSize; }
    const Features&    GetFeatures() const { return mFeatures; }

private:
    friend CpuInfo GetCpuInfo();

private:
    std::string mBrandString;
    std::string mVendorString;
    std::string mArchitectureString;
    uint32_t    mL1CacheSize     = 0;
    uint32_t    mL2CacheSize     = 0;
    uint32_t    mL3CacheSize     = 0;
    uint32_t    mL1CacheLineSize = 0;
    uint32_t    mL2CacheLineSize = 0;
    uint32_t    mL3CacheLineSize = 0;
    Features    mFeatures        = {0};
};

Platform GetPlatform();
CpuInfo  GetCpuInfo();

} // namespace ppx

#endif // ppx_platform_h
