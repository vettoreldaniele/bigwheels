#include "ppx/grfx/grfx_helper.h"

namespace ppx {
namespace grfx {

// -------------------------------------------------------------------------------------------------
// ColorComponentFlags
// -------------------------------------------------------------------------------------------------
ColorComponentFlags ColorComponentFlags::RGBA()
{
    ColorComponentFlags flags = ColorComponentFlags(COLOR_COMPONENT_R | COLOR_COMPONENT_G | COLOR_COMPONENT_B | COLOR_COMPONENT_A);
    return flags;
}

// -------------------------------------------------------------------------------------------------
// ImageUsageFlags
// -------------------------------------------------------------------------------------------------
ImageUsageFlags ImageUsageFlags::SampledImage()
{
    ImageUsageFlags flags = ImageUsageFlags(grfx::IMAGE_USAGE_SAMPLED);
    return flags;
}

// -------------------------------------------------------------------------------------------------
// VertexBinding
// -------------------------------------------------------------------------------------------------
bool VertexBinding::GetAttribute(uint32_t index, const grfx::VertexAttribute** ppAttribute) const
{
    if (index >= mAttributes.size()) {
        return false;
    }
    *ppAttribute = &mAttributes[index];
    return true;
}

void VertexBinding::AppendAttribute(const grfx::VertexAttribute& attribute)
{
    mAttributes.push_back(attribute);

    if (mInputRate == grfx::VertexBinding::kInvalidVertexInputRate) {
        mInputRate = attribute.inputRate;
    }

    // Caluclate offset for inserted attribute
    if (mAttributes.size() > 1) {
        size_t i1 = mAttributes.size() - 1;
        size_t i0 = i1 - 1;
        if (mAttributes[i1].offset == PPX_APPEND_OFFSET_ALIGNED) {
            uint32_t prevOffset    = mAttributes[i0].offset;
            uint32_t prevSize      = grfx::FormatSize(mAttributes[i0].format);
            mAttributes[i1].offset = prevOffset + prevSize;
        }
    }
    // Size of mAttributes should be 1
    else {
        if (mAttributes[0].offset == PPX_APPEND_OFFSET_ALIGNED) {
            mAttributes[0].offset = 0;
        }
    }

    // Calculate stride
    mStride = 0;
    for (auto& elem : mAttributes) {
        uint32_t size = grfx::FormatSize(elem.format);
        mStride += size;
    }
}

grfx::VertexBinding& VertexBinding::operator+=(const grfx::VertexAttribute& rhs)
{
    AppendAttribute(rhs);
    return *this;
}

} // namespace grfx
} // namespace ppx
