#ifndef ppx_grfx_helper_h
#define ppx_grfx_helper_h

#include <cstdint>

namespace ppx {
namespace grfx {

enum BufferUsageFlagBits
{
    BUFFER_USAGE_TRANSFER_SRC                      = 0x00000001,
    BUFFER_USAGE_TRANSFER_DST                      = 0x00000002,
    BUFFER_USAGE_UNIFORM_TEXEL_BUFFER              = 0x00000004,
    BUFFER_USAGE_STORAGE_TEXEL_BUFFER              = 0x00000008,
    BUFFER_USAGE_UNIFORM_BUFFER                    = 0x00000010,
    BUFFER_USAGE_STORAGE_BUFFER                    = 0x00000020,
    BUFFER_USAGE_INDEX_BUFFER                      = 0x00000040,
    BUFFER_USAGE_VERTEX_BUFFER                     = 0x00000080,
    BUFFER_USAGE_INDIRECT_BUFFER                   = 0x00000100,
    BUFFER_USAGE_CONDITIONAL_RENDERING             = 0x00000200,
    BUFFER_USAGE_RAY_TRACING                       = 0x00000400,
    BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER         = 0x00000800,
    BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER = 0x00001000,
    BUFFER_USAGE_SHADER_DEVICE_ADDRESS             = 0x00002000,
};

struct BufferUsageFlags
{
    // clang-format off
    union
    {
        struct
        {
            bool transferSrc                    : 1;
            bool transferDst                    : 1;
            bool uniformTexelBuffer             : 1;
            bool storageTexelBuffer             : 1;
            bool uniformBuffer                  : 1;
            bool storageBuffer                  : 1;
            bool indexBuffer                    : 1;
            bool vertexBuffer                   : 1;
            bool indirectBuffer                 : 1;
            bool conditionalRendering           : 1;
            bool rayTracing                     : 1;
            bool transformFeedbackBuffer        : 1;
            bool transformFeedbackCounterBuffer : 1;
            bool shaderDeviceAddress            : 1;
        } bits;
        uint32_t flags;
    };
    // clang-format on

    BufferUsageFlags()
        : flags(0) {}

    BufferUsageFlags(uint32_t _flags)
        : flags(_flags) {}

    BufferUsageFlags& operator=(uint32_t rhs)
    {
        this->flags = rhs;
    }

    operator uint32_t() const
    {
        return flags;
    }
};

// -------------------------------------------------------------------------------------------------

enum ImageUsageFlagBits
{
    IMAGE_USAGE_TRANSFER_SRC             = 0x00000001,
    IMAGE_USAGE_TRANSFER_DST             = 0x00000002,
    IMAGE_USAGE_SAMPLED                  = 0x00000004,
    IMAGE_USAGE_STORAGE                  = 0x00000008,
    IMAGE_USAGE_COLOR_ATTACHMENT         = 0x00000010,
    IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000020,
    IMAGE_USAGE_TRANSIENT_ATTACHMENT     = 0x00000040,
    IMAGE_USAGE_INPUT_ATTACHMENT         = 0x00000080,
    IMAGE_USAGE_SHADING_RATE_IMAGE_NV    = 0x00000100,
    IMAGE_USAGE_FRAGMENT_DENSITY_MAP     = 0x00000200,
};

struct ImageUsageFlags
{
    // clang-format off
    union
    {
        struct
        {
            bool transferSrc            : 1;
            bool transferDst            : 1;
            bool sampled                : 1;
            bool storage                : 1;
            bool colorAttachment        : 1;
            bool depthStencilAttachment : 1;
            bool transientAattachment   : 1;
            bool inputAttachment        : 1;
            bool shadingRateImageNv     : 1;
            bool fragmentDensityMap     : 1;
        } bits;
        uint32_t flags;
    };
    // clang-format on

    ImageUsageFlags()
        : flags(0) {}

    ImageUsageFlags(uint32_t _flags)
        : flags(_flags) {}

    ImageUsageFlags& operator=(uint32_t rhs)
    {
        this->flags = rhs;
    }

    operator uint32_t() const
    {
        return flags;
    }
};


} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_helper_h
