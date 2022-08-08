#include "ppx/000_config.h"
#include "ppx/shader_translator.h"

#include "gmock/gmock.h"
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace ppx {
namespace {

using ::testing::ElementsAreArray;

using internal::DXContainerHeader;
using internal::DXPartHeader;
using internal::Magic;

TEST(ShaderTranslator, DetectShaderFormatEmpty)
{
    EXPECT_EQ(DetectShaderFormat({}), ShaderFormat::UNKNOWN);
}

TEST(ShaderTranslator, DetectShaderFormatSpirv)
{
    EXPECT_EQ(DetectShaderFormat({0x03, 0x02, 0x23, 0x07}), ShaderFormat::SPIRV);
    EXPECT_EQ(DetectShaderFormat({0x03, 0x02, 0x23, 0x07, 0x13, 0x37}), ShaderFormat::SPIRV);

    // Currently, we do not support big endian SPIR-V inputs.
    EXPECT_EQ(DetectShaderFormat({0x07, 0x23, 0x02, 0x03}), ShaderFormat::UNKNOWN);
}

// Appends |data| to the end of the |blob| vector, resizing it and potentially reallocating.
template <typename T>
void AppendData(std::vector<char>& blob, const T& data)
{
    static_assert(std::is_trivially_copyable_v<T>, "data must be of a POD type");
    const size_t initialSize = blob.size();
    blob.resize(initialSize + sizeof(data));
    std::memcpy(blob.data() + initialSize, &data, sizeof(data));
}

// Creates a new valid DX container blob based on the list of parts, |partHeaders|.
std::vector<char> CreateDXContainer(std::vector<DXPartHeader> partHeaders)
{
    std::vector<char> blob;
    auto              blobSize = [&blob] { return static_cast<uint32_t>(blob.size()); };
    DXContainerHeader header   = {
        .headerFourCC = {'D', 'X', 'B', 'C'},
        .majorVersion = 1,
        // .containerSizeInBytes will be updated at the very end.
        .partCount = static_cast<uint32_t>(partHeaders.size()),
    };

    // Reserve space for part offsets.
    AppendData(blob, header);
    const uint32_t partOffsetsBegin = blobSize();
    for (const DXPartHeader& partHeader : partHeaders)
        AppendData(blob, static_cast<uint32_t>(0));
    const uint32_t partOffsetsEnd = blobSize();

    // Append part headers, keeping track of their offsets.
    std::vector<uint32_t> calculatedPartOffsets;
    calculatedPartOffsets.reserve(partHeaders.size());

    for (const DXPartHeader& partHeader : partHeaders) {
        calculatedPartOffsets.push_back(blobSize());
        AppendData(blob, partHeader);
        blob.resize(blobSize() + partHeader.partSizeInBytes);
    }

    std::memcpy(blob.data() + partOffsetsBegin, calculatedPartOffsets.data(), partOffsetsEnd - partOffsetsBegin);
    auto& finalHeader                = *reinterpret_cast<DXContainerHeader*>(blob.data());
    finalHeader.containerSizeInBytes = blobSize();

    return blob;
};

TEST(ShaderTranslator, DetectShaderFormatDxbcOnePart)
{
    DXPartHeader            rdef     = {.partFourCC = {'R', 'D', 'E', 'F'}, .partSizeInBytes = 4};
    const std::vector<char> dxbcBlob = CreateDXContainer({rdef});

    EXPECT_EQ(DetectShaderFormat(dxbcBlob), ShaderFormat::DXBC);

    std::vector<char> badBlob(dxbcBlob);
    auto&             header = *reinterpret_cast<DXContainerHeader*>(badBlob.data());

    // Bad magic.
    header.headerFourCC.bytes[0] = 'X';
    EXPECT_EQ(DetectShaderFormat(badBlob), ShaderFormat::UNKNOWN);

    // Bad container size.
    badBlob = dxbcBlob;
    header.containerSizeInBytes -= 1;
    EXPECT_EQ(DetectShaderFormat(badBlob), ShaderFormat::UNKNOWN);

    // Bad version.
    badBlob             = dxbcBlob;
    header.majorVersion = 2;
    EXPECT_EQ(DetectShaderFormat(badBlob), ShaderFormat::UNKNOWN);

    // Bad part offset.
    badBlob          = dxbcBlob;
    header.partCount = 2;
    EXPECT_EQ(DetectShaderFormat(badBlob), ShaderFormat::UNKNOWN);

    // Check that restoring the buffer to the original state works.
    badBlob = dxbcBlob;
    EXPECT_EQ(DetectShaderFormat(badBlob), ShaderFormat::DXBC);
}

TEST(ShaderTranslator, DetectShaderFormatDxbcTwoParts)
{
    DXPartHeader            rdef     = {.partFourCC = {'R', 'D', 'E', 'F'}, .partSizeInBytes = 4};
    DXPartHeader            isgn     = {.partFourCC = {'I', 'S', 'G', 'N'}, .partSizeInBytes = 12};
    const std::vector<char> dxbcBlob = CreateDXContainer({rdef, isgn});
    EXPECT_EQ(DetectShaderFormat(dxbcBlob), ShaderFormat::DXBC);
}

TEST(ShaderTranslator, DetectShaderFormatDxil)
{
    DXPartHeader            sfi0     = {.partFourCC = {'S', 'F', 'I', '0'}, .partSizeInBytes = 4};
    DXPartHeader            dxil     = {.partFourCC = {'D', 'X', 'I', 'L'}, .partSizeInBytes = 128};
    const std::vector<char> dxilBlob = CreateDXContainer({sfi0, dxil});
    EXPECT_EQ(DetectShaderFormat(dxilBlob), ShaderFormat::DXIL);
}

TEST(ShaderTranslator, DetectShaderFormatText)
{
    EXPECT_EQ(DetectShaderFormat({'t', 'e', 'x', 't', '\0'}), ShaderFormat::UNKNOWN);
    // We do not support detecting HLSL.
    const std::string hlsl = R"(
        [numthreads(64, 1, 1)]
        void main(uint3 tid : SV_DispatchThreadId) {}
    )";
    EXPECT_EQ(DetectShaderFormat(std::vector<char>(hlsl.begin(), hlsl.end())), ShaderFormat::UNKNOWN);
}

TEST(ShaderTranslator, TranslateUnknownShaderFormat)
{
    TranslationResult result = TranslateToSpirv({0x03, 0x02, 0x23, 0x07}, ShaderFormat::UNKNOWN);
    EXPECT_TRUE(Failed(result.result));
}

TEST(ShaderTranslator, TranslateSpirvShaderFormat)
{
    const std::vector<char> inputSpirv = {0x03, 0x02, 0x23, 0x07};
    TranslationResult       result     = TranslateToSpirv(inputSpirv, ShaderFormat::SPIRV);
    EXPECT_EQ(result.result, Result::SUCCESS);
    EXPECT_THAT(result.bytecode, ElementsAreArray(inputSpirv));
}

TEST(ShaderTranslator, TranslateSpirvDetectShaderFormat)
{
    const std::vector<char> inputSpirv = {0x03, 0x02, 0x23, 0x07};
    TranslationResult       result     = TranslateToSpirv(inputSpirv);
    EXPECT_EQ(result.result, Result::SUCCESS);
    EXPECT_THAT(result.bytecode, ElementsAreArray(inputSpirv));
}

} // namespace
} // namespace ppx
