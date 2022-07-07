#ifndef PPX_SHADER_TRANSLATOR_H
#define PPX_SHADER_TRANSLATOR_H

#include "ppx/ppx.h"

#include <array>
#include <string>
#include <vector>

namespace ppx {

// Represents the result of translating an input shader to another bytecode format.
struct TranslationResult
{
    Result            result = Result::ERROR_FAILED;
    std::string       log;
    std::vector<char> bytecode;
};

enum class ShaderFormat
{
    SPIRV,
    DXIL,
    DXBC,
    HLSL,
    UNKNOWN
};

// Detects the shader bytecode |shaderCode| format based on the raw shader code.
// Returns `ShaderFormat::UNKNOWN` when the format cannot be determined or the shader code is
// determined to be illformed/corrupted.
ShaderFormat DetectShaderFormat(const std::vector<char>& shaderCode);

// Translates the input |shaderCode| to SPIR-V assembly. Assumes that the input shader is of the
// |inputFormat|. If the |inputFormat| is already a SPIR-V, this is a noop. Translation failure
// is communicated to the caller via the `.result` field.
TranslationResult TranslateToSpirv(const std::vector<char>& shaderCode, ShaderFormat inputFormat);

// Translates the input |shaderCode| to SPIR-V assembly. Detects the input byte format automatically.
inline TranslationResult TranslateToSpirv(const std::vector<char>& shaderCode)
{
    return TranslateToSpirv(shaderCode, DetectShaderFormat(shaderCode));
}

namespace internal {
// Represents a magic number used to determine blob data type.
struct Magic
{
    std::array<char, 4> bytes;

    // Returns true iff the |other| magic is the same. Currently, this does not account for endianness.
    bool Matches(const Magic& other) const
    {
        return bytes == other.bytes;
    }
};
static_assert(sizeof(Magic) == sizeof(uint32_t), "Magic is supposed to be word-sized");
static_assert(alignof(Magic) <= alignof(uint32_t), "Magic is overaligned");

// DX Container representations based on
// https://github.com/Microsoft/DirectXShaderCompiler/blob/main/docs/DXIL.rst#dxil-container-format.
constexpr size_t DXContainerHashSize = 16;

struct DXContainerHeader
{
    Magic    headerFourCC;
    uint8_t  digest[DXContainerHashSize];
    uint16_t majorVersion;
    uint16_t minorVersion;
    uint32_t containerSizeInBytes;
    uint32_t partCount;
    // Followed by uint32[PartCount] partOffsets; // Absolute
};

struct DXPartHeader
{
    Magic    partFourCC;
    uint32_t partSizeInBytes;
    // Followed by uint8[partSizeInBytes] partData;
};
} // namespace internal

} // namespace ppx

#endif // PPX_SHADER_TRANSLATOR_H
