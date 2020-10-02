#include "ppx/grfx/grfx_texture.h"
#include "ppx/grfx/grfx_image.h"

namespace ppx {
namespace grfx {

uint32_t Texture::GetWidth() const
{
    PPX_ASSERT_NULL_ARG(mImage.Get());
    return mImage->GetWidth();
}

uint32_t Texture::GetHeight() const
{
    PPX_ASSERT_NULL_ARG(mImage.Get());
    return mImage->GetHeight();
}

uint32_t Texture::GetDepth() const
{
    PPX_ASSERT_NULL_ARG(mImage.Get());
    return mImage->GetDepth();
}

} // namespace grfx
} // namespace ppx
