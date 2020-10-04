#ifndef ppx_grfx_constants_h
#define ppx_grfx_constants_h

// clang-format off
#define PPX_VALUE_IGNORED           UINT32_MAX

#define PPX_MAX_RENDER_TARGETS      8

#define PPX_ALL_MIP_LEVELS          UINT32_MAX
#define PPX_ALL_ARRAY_LAYERS        UINT32_MAX
#define PPX_ALL_SUBRESOURCES        0, PPX_ALL_MIP_LEVELS, 0, PPX_ALL_ARRAY_LAYERS

#define PPX_MAX_VERTEX_ATTRIBUTES   16
#define PPX_MAX_VIEWPORTS           16
#define PPX_APPEND_OFFSET_ALIGNED   UINT32_MAX
// clang-format on

namespace ppx {
namespace grfx {

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_constants_h
