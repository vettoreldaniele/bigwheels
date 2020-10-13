#ifndef ppx_grfx_constants_h
#define ppx_grfx_constants_h

// clang-format off
#define PPX_VALUE_IGNORED               UINT32_MAX

#define PPX_MAX_RENDER_TARGETS          8

#define PPX_ALL_MIP_LEVELS              UINT32_MAX
#define PPX_ALL_ARRAY_LAYERS            UINT32_MAX
#define PPX_ALL_SUBRESOURCES            0, PPX_ALL_MIP_LEVELS, 0, PPX_ALL_ARRAY_LAYERS

#define PPX_MAX_VERTEX_BINDINGS       16
#define PPX_APPEND_OFFSET_ALIGNED       UINT32_MAX

#define PPX_MAX_VIEWPORTS               16
#define PPX_MAX_SCISSORS                16

#define PPX_MAX_SETS_PER_POOL           1024
#define PPX_MAX_BOUND_DESCRIPTOR_SETS   32

#define PPX_WHOLE_SIZE                  UINT64_MAX
// clang-format on

namespace ppx {
namespace grfx {

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_constants_h
