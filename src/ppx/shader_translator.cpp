#include "ppx/shader_translator.h"
#include "ppx/000_config.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>

namespace ppx {

namespace {

using internal::DXContainerHeader;
using internal::DXPartHeader;
using internal::Magic;

namespace magic {
constexpr Magic DXBC  = {'D', 'X', 'B', 'C'};
constexpr Magic DXIL  = {'D', 'X', 'I', 'L'};
constexpr Magic SPIRV = {0x03, 0x02, 0x23, 0x07};
} // namespace magic

// Given a raw blob |dxContainer|, attempts to parse it as a DX container. Returns the vector of
// all part headers on success, or an empty vector on failure.
std::vector<DXPartHeader> FindDXPartHeaders(const std::vector<char>& dxContainer)
{
    if (dxContainer.size() < sizeof(DXContainerHeader))
        return {}; // Not a DX container.

    // A safe wrapper to memcpy data out of the DX container.
    auto readData = [&dxContainer](void* dest, const char*& source, size_t numBytes) {
        if (source < dxContainer.data() || (source + numBytes) > (dxContainer.data() + dxContainer.size()))
            return Result::ERROR_OUT_OF_RANGE;

        std::memcpy(dest, source, numBytes);
        source += numBytes;
        return Result::SUCCESS;
    };

    DXContainerHeader header   = {};
    const char*       currData = dxContainer.data();
    if (Failed(readData(&header, currData, sizeof(header))))
        return {}; // Not a DX container.
    if (header.majorVersion != 1 || header.minorVersion != 0)
        return {}; // Unknown DX container format.
    if (header.containerSizeInBytes != static_cast<uint32_t>(dxContainer.size()))
        return {}; // Corrupted container.

    std::vector<uint32_t> partOffsets(header.partCount);
    if (Failed(readData(partOffsets.data(), currData, sizeof(uint32_t) * header.partCount)))
        return {}; // Malformed container.

    std::vector<DXPartHeader> parts;
    parts.reserve(header.partCount);

    for (uint32_t offset : partOffsets) {
        const char*  pos        = dxContainer.data() + offset;
        DXPartHeader partHeader = {};
        if (Failed(readData(&partHeader, pos, sizeof(partHeader))))
            return {}; // Bad offset.
        parts.push_back(partHeader);
    }

    return parts;
}

} // namespace

ShaderFormat DetectShaderFormat(const std::vector<char>& shaderCode)
{
    if (shaderCode.size() < sizeof(uint32_t))
        return ShaderFormat::UNKNOWN;

    // We only support shaders encoded as Little Endian, which should always be the case on x86.
    Magic firstWord;
    std::memcpy(&firstWord, shaderCode.data(), sizeof(firstWord));

    if (firstWord.Matches(magic::SPIRV))
        return ShaderFormat::SPIRV;

    if (firstWord.Matches(magic::DXBC)) {
        // We need to check the contents to see if this is a DXBC or DXIL container.
        const std::vector<DXPartHeader> parts = FindDXPartHeaders(shaderCode);
        if (parts.empty())
            return ShaderFormat::UNKNOWN; // Most likely a bad DX shader.

        // DXIL shaders have one part that starts with the "DXIL" magic.
        for (const DXPartHeader& part : parts) {
            if (part.partFourCC.Matches(magic::DXIL)) {
                return ShaderFormat::DXIL;
            }
        }
        return ShaderFormat::DXBC;
    }

    return ShaderFormat::UNKNOWN;
}

TranslationResult TranslateToSpirv(const std::vector<char>& shaderCode, ShaderFormat inputFormat)
{
    if (inputFormat == ShaderFormat::UNKNOWN)
        return {.result = Result::ERROR_UNSUPPORTED_API};

    if (inputFormat == ShaderFormat::SPIRV)
        return {.result = Result::SUCCESS, .bytecode = shaderCode};

    // Shader translation is not implemented yet.
    // TODO(kubak@): Implement shader translation for DXBC and DXIL.
    return {};
}

} // namespace ppx
