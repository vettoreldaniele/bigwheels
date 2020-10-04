#ifndef ppx_grfx_helper_h
#define ppx_grfx_helper_h

#include "ppx/grfx/grfx_constants.h"
#include "ppx/grfx/grfx_enums.h"
#include "ppx/grfx/grfx_format.h"

#include <cstdint>

namespace ppx {
namespace grfx {

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

struct ColorComponentFlags
{
    // clang-format off
    union
    {
        struct
        {
            bool r : 1;
            bool g : 1;
            bool b : 1;
            bool a : 1;
        } bits;
        uint32_t flags;
    };
    // clang-format on

    ColorComponentFlags()
        : flags(0) {}

    ColorComponentFlags(uint32_t _flags)
        : flags(_flags) {}

    ColorComponentFlags& operator=(uint32_t rhs)
    {
        this->flags = rhs;
    }

    operator uint32_t() const
    {
        return flags;
    }

    static ColorComponentFlags RGBA();
};

// -------------------------------------------------------------------------------------------------

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

    static ImageUsageFlags SampledImage();
};

// -------------------------------------------------------------------------------------------------

struct VertexAttribute
{
    uint32_t              location  = 0;                              // @TODO: Find a way to handle between DX and VK
    grfx::Format          format    = grfx::FORMAT_UNDEFINED;         //
    uint32_t              binding   = 0;                              // Valid range is [0, 15]
    uint32_t              offset    = PPX_APPEND_OFFSET_ALIGNED;      // Use PPX_APPEND_OFFSET_ALIGNED to auto calculate offsets
    grfx::VertexInputRate inputRate = grfx::VERTEX_INPUT_RATE_VERTEX; //
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_helper_h
