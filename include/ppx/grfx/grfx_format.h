#ifndef ppx_grfx_format_h
#define ppx_grfx_format_h

#include <cstdint>

namespace ppx {
namespace grfx {

enum Format
{
    FORMAT_UNDEFINED = 0,

    // 8-bit signed normalized
    FORMAT_R8_SNORM,
    FORMAT_R8G8_SNORM,
    FORMAT_R8G8B8_SNORM,
    FORMAT_R8G8B8A8_SNORM,
    FORMAT_B8G8R8_SNORM,
    FORMAT_B8G8R8A8_SNORM,

    // 8-bit unsigned normalized
    FORMAT_R8_UNORM,
    FORMAT_R8G8_UNORM,
    FORMAT_R8G8B8_UNORM,
    FORMAT_R8G8B8A8_UNORM,
    FORMAT_B8G8R8_UNORM,
    FORMAT_B8G8R8A8_UNORM,

    // 8-bit signed integer
    FORMAT_R8_SINT,
    FORMAT_R8G8_SINT,
    FORMAT_R8G8B8_SINT,
    FORMAT_R8G8B8A8_SINT,
    FORMAT_B8G8R8_SINT,
    FORMAT_B8G8R8A8_SINT,

    // 8-bit unsigned integer
    FORMAT_R8_UINT,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8B8_UINT,
    FORMAT_R8G8B8A8_UINT,
    FORMAT_B8G8R8_UINT,
    FORMAT_B8G8R8A8_UINT,

    // 16-bit signed normalized
    FORMAT_R16_SNORM,
    FORMAT_R16G16_SNORM,
    FORMAT_R16G16B16_SNORM,
    FORMAT_R16G16B16A16_SNORM,

    // 16-bit unsigned normalized
    FORMAT_R16_UNORM,
    FORMAT_R16G16_UNORM,
    FORMAT_R16G16B16_UNORM,
    FORMAT_R16G16B16A16_UNORM,

    // 16-bit signed integer
    FORMAT_R16_SINT,
    FORMAT_R16G16_SINT,
    FORMAT_R16G16B16_SINT,
    FORMAT_R16G16B16A16_SINT,

    // 16-bit unsigned integer
    FORMAT_R16_UINT,
    FORMAT_R16G16_UINT,
    FORMAT_R16G16B16_UINT,
    FORMAT_R16G16B16A16_UINT,

    // 16-bit float
    FORMAT_R16_FLOAT,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16B16_FLOAT,
    FORMAT_R16G16B16A16_FLOAT,

    // 32-bit signed integer
    FORMAT_R32_SINT,
    FORMAT_R32G32_SINT,
    FORMAT_R32G32B32_SINT,
    FORMAT_R32G32B32A32_SINT,

    // 32-bit unsigned integer
    FORMAT_R32_UINT,
    FORMAT_R32G32_UINT,
    FORMAT_R32G32B32_UINT,
    FORMAT_R32G32B32A32_UINT,

    // 32-bit float
    FORMAT_R32_FLOAT,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32A32_FLOAT,

    // 8-bit unsigned integer stencil
    FORMAT_S8_UINT,

    // 16-bit unsigned normalized depth
    FORMAT_D16_UNORM,

    // 32-bit float depth
    FORMAT_D32_FLOAT,

    // Depth/stencil combinations
    FORMAT_D16_UNORM_S8_UINT,
    FORMAT_D24_UNORM_S8_UINT,
    FORMAT_D32_FLOAT_S8_UINT,

    // SRGB
    FORMAT_R8_SRGB,
    FORMAT_R8G8_SRGB,
    FORMAT_R8G8B8_SRGB,
    FORMAT_R8G8B8A8_SRGB,
    FORMAT_B8G8R8_SRGB,
    FORMAT_B8G8R8A8_SRGB,
};

uint32_t FormatSize(grfx::Format format);

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_format_h
