#ifndef ppx_grfx_constants_h
#define ppx_grfx_constants_h

// clang-format off
#define PPX_VALUE_IGNORED                       UINT32_MAX

#define PPX_MAX_SAMPLER_DESCRIPTORS             2048
#define PPX_DEFAULT_RESOURCE_DESCRIPTOR_COUNT   8192
#define PPX_DEFAULT_SAMPLE_DESCRIPTOR_COUNT     PPX_MAX_SAMPLER_DESCRIPTORS

#define PPX_MAX_RENDER_TARGETS                  8

#define PPX_ALL_MIP_LEVELS                      UINT32_MAX
#define PPX_ALL_ARRAY_LAYERS                    UINT32_MAX
#define PPX_ALL_SUBRESOURCES                    0, PPX_ALL_MIP_LEVELS, 0, PPX_ALL_ARRAY_LAYERS

#define PPX_MAX_VERTEX_BINDINGS                 16
#define PPX_APPEND_OFFSET_ALIGNED               UINT32_MAX

#define PPX_MAX_VIEWPORTS                       16
#define PPX_MAX_SCISSORS                        16

#define PPX_MAX_SETS_PER_POOL                   1024
#define PPX_MAX_BOUND_DESCRIPTOR_SETS           32

#define PPX_WHOLE_SIZE                          UINT64_MAX

//
// Vulkan dynamic uniform/storage buffers requires that offsets are aligned
// to VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment. 
// Based on vulkan.gpuinfo.org, the range of this value [1, 256]
// Meaning that 256 should cover all offset cases.
// 
// D3D12 on most(all?) GPUs require that the minimum constant buffer
// size to be 256.
//
#define PPX_CONSTANT_BUFFER_ALIGNMENT           256
#define PPX_UNIFORM_BUFFER_ALIGNMENT            PPX_CONSTANT_BUFFER_ALIGNMENT
#define PPX_STORAGE_BUFFER_ALIGNMENT            PPX_CONSTANT_BUFFER_ALIGNMENT
#define PPX_STUCTURED_BUFFER_ALIGNMENT          PPX_CONSTANT_BUFFER_ALIGNMENT

#define PPX_MINIUM_CONSTANT_BUFFER_SIZE         PPX_CONSTANT_BUFFER_ALIGNMENT
#define PPX_MINIUM_UNIFORM_BUFFER_SIZE          PPX_CONSTANT_BUFFER_ALIGNMENT
#define PPX_MINIUM_STORAGE_BUFFER_SIZE          PPX_CONSTANT_BUFFER_ALIGNMENT
#define PPX_MINIUM_STRUCTURED_BUFFER_SIZE       PPX_CONSTANT_BUFFER_ALIGNMENT
// clang-format on

#define PPX_MAX_MODEL_TEXTURES_IN_CREATE_INFO 16

// D3D12 requires buffer to image copies to have a row pitch that's
// aligned to D3D12_TEXTURE_DATA_PITCH_ALIGNMENT(256).
//
#define PPX_D3D12_TEXTURE_DATA_PITCH_ALIGNMENT 256

// PPX standard attribute semantic names
#define PPX_SEMANTIC_NAME_POSITION  "POSITION"
#define PPX_SEMANTIC_NAME_NORMAL    "NORMAL"
#define PPX_SEMANTIC_NAME_COLOR     "COLOR"
#define PPX_SEMANTIC_NAME_TEXCOORD  "TEXCOORD"
#define PPX_SEMANTIC_NAME_TANGENT   "TANGENT"
#define PPX_SEMANTIC_NAME_BITANGENT "BITANGENT"

namespace ppx {
namespace grfx {

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_constants_h
