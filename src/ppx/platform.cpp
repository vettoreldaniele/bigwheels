#include "ppx/platform.h"
#include "cpuinfo_x86.h"

#include <intrin.h>

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

CpuInfo GetCpuInfo()
{
    cpu_features::X86Info              info      = cpu_features::GetX86Info();
    cpu_features::X86Microarchitecture arch      = cpu_features::GetX86Microarchitecture(&info);
    cpu_features::CacheInfo            cacheInfo = cpu_features::GetX86CacheInfo();

    char brandString[49] = {0};
    cpu_features::FillX86BrandString(brandString);

    CpuInfo cpuInfo             = {};
    cpuInfo.mBrandString        = brandString;
    cpuInfo.mVendorString       = info.vendor;
    cpuInfo.mArchitectureString = cpu_features::GetX86MicroarchitectureName(arch);

    if (cacheInfo.size >= 1) {
        cpuInfo.mL1CacheSize = cacheInfo.levels[0].cache_size;
    }

    cpuInfo.mFeatures.sse                 = static_cast<bool>(info.features.sse);
    cpuInfo.mFeatures.sse2                = static_cast<bool>(info.features.sse2);
    cpuInfo.mFeatures.sse3                = static_cast<bool>(info.features.sse3);
    cpuInfo.mFeatures.ssse3               = static_cast<bool>(info.features.ssse3);
    cpuInfo.mFeatures.sse4_1              = static_cast<bool>(info.features.sse4_1);
    cpuInfo.mFeatures.sse4_2              = static_cast<bool>(info.features.sse4_2);
    cpuInfo.mFeatures.sse4a               = static_cast<bool>(info.features.sse4a);
    cpuInfo.mFeatures.avx                 = static_cast<bool>(info.features.avx);
    cpuInfo.mFeatures.avx2                = static_cast<bool>(info.features.avx2);
    cpuInfo.mFeatures.avx512f             = static_cast<bool>(info.features.avx512f);
    cpuInfo.mFeatures.avx512cd            = static_cast<bool>(info.features.avx512cd);
    cpuInfo.mFeatures.avx512er            = static_cast<bool>(info.features.avx512er);
    cpuInfo.mFeatures.avx512pf            = static_cast<bool>(info.features.avx512pf);
    cpuInfo.mFeatures.avx512bw            = static_cast<bool>(info.features.avx512bw);
    cpuInfo.mFeatures.avx512dq            = static_cast<bool>(info.features.avx512dq);
    cpuInfo.mFeatures.avx512vl            = static_cast<bool>(info.features.avx512vl);
    cpuInfo.mFeatures.avx512ifma          = static_cast<bool>(info.features.avx512ifma);
    cpuInfo.mFeatures.avx512vbmi          = static_cast<bool>(info.features.avx512vbmi);
    cpuInfo.mFeatures.avx512vbmi2         = static_cast<bool>(info.features.avx512vbmi2);
    cpuInfo.mFeatures.avx512vnni          = static_cast<bool>(info.features.avx512vnni);
    cpuInfo.mFeatures.avx512bitalg        = static_cast<bool>(info.features.avx512bitalg);
    cpuInfo.mFeatures.avx512vpopcntdq     = static_cast<bool>(info.features.avx512vpopcntdq);
    cpuInfo.mFeatures.avx512_4vnniw       = static_cast<bool>(info.features.avx512_4vnniw);
    cpuInfo.mFeatures.avx512_4vbmi2       = static_cast<bool>(info.features.avx512_4vbmi2);
    cpuInfo.mFeatures.avx512_second_fma   = static_cast<bool>(info.features.avx512_second_fma);
    cpuInfo.mFeatures.avx512_4fmaps       = static_cast<bool>(info.features.avx512_4fmaps);
    cpuInfo.mFeatures.avx512_bf16         = static_cast<bool>(info.features.avx512_bf16);
    cpuInfo.mFeatures.avx512_vp2intersect = static_cast<bool>(info.features.avx512_vp2intersect);
    cpuInfo.mFeatures.amx_bf16            = static_cast<bool>(info.features.amx_bf16);
    cpuInfo.mFeatures.amx_tile            = static_cast<bool>(info.features.amx_tile);
    cpuInfo.mFeatures.amx_int8            = static_cast<bool>(info.features.amx_int8);

    return cpuInfo;
}

} // namespace ppx
