#ifndef ppx_grfx_helper_h
#define ppx_grfx_helper_h

#include "ppx/grfx/grfx_constants.h"
#include "ppx/grfx/grfx_enums.h"
#include "ppx/grfx/grfx_format.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ppx {
namespace grfx {

struct BufferUsageFlags
{
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
            bool structuredBuffer               : 1;
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
}; // -------------------------------------------------------------------------------------------------

struct DrawPassClearFlags
{
    union
    {
        struct
        {
            bool clearRenderTargets : 1;
            bool clearDepth         : 1;
            bool clearStencil       : 1;

        } bits;
        uint32_t flags;
    };

    DrawPassClearFlags()
        : flags(0) {}

    DrawPassClearFlags(uint32_t flags_)
        : flags(flags_) {}

    DrawPassClearFlags& operator=(uint32_t rhs)
    {
        this->flags = rhs;
    }

    operator uint32_t() const
    {
        return flags;
    }
};

// -------------------------------------------------------------------------------------------------

struct ImageUsageFlags
{
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

    ImageUsageFlags()
        : flags(0) {}

    ImageUsageFlags(uint32_t flags_)
        : flags(flags_) {}

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

struct Range
{
    uint32_t start = 0;
    uint32_t end   = 0;
};

// -------------------------------------------------------------------------------------------------

struct ShaderStageFlags
{
    union
    {
        struct
        {
            bool VS : 1;
            bool HS : 1;
            bool DS : 1;
            bool GS : 1;
            bool PS : 1;
            bool CS : 1;

        } bits;
        uint32_t flags;
    };

    ShaderStageFlags()
        : flags(0) {}

    ShaderStageFlags(uint32_t _flags)
        : flags(_flags) {}

    ShaderStageFlags& operator=(uint32_t rhs)
    {
        this->flags = rhs;
    }

    operator uint32_t() const
    {
        return flags;
    }

    static ShaderStageFlags SampledImage();
};

// -------------------------------------------------------------------------------------------------

struct VertexAttribute
{
    std::string           semanticName = "";                              // Semantic name (no effect in Vulkan currently)
    uint32_t              location     = 0;                               // @TODO: Find a way to handle between DX and VK
    grfx::Format          format       = grfx::FORMAT_UNDEFINED;          //
    uint32_t              binding      = 0;                               // Valid range is [0, 15]
    uint32_t              offset       = PPX_APPEND_OFFSET_ALIGNED;       // Use PPX_APPEND_OFFSET_ALIGNED to auto calculate offsets
    grfx::VertexInputRate inputRate    = grfx::VERTEX_INPUT_RATE_VERTEX;  //
    grfx::VertexSemantic  semantic     = grfx::VERTEX_SEMANTIC_UNDEFINED; // [OPTIONAL]
};

// -------------------------------------------------------------------------------------------------

class VertexBinding
{
public:
    VertexBinding(uint32_t binding = 0)
        : mBinding(binding) {}

    ~VertexBinding() {}

    uint32_t              GetBinding() const { return mBinding; }
    void                  SetBinding(uint32_t binding);
    const uint32_t&       GetStride() const { return mStride; }
    grfx::VertexInputRate GetInputRate() const { return mInputRate; }
    uint32_t              GetAttributeCount() const { return static_cast<uint32_t>(mAttributes.size()); }
    bool                  GetAttribute(uint32_t index, const grfx::VertexAttribute** ppAttribute) const;
    uint32_t              GetAttributeIndex(grfx::VertexSemantic semantic) const;
    void                  AppendAttribute(const grfx::VertexAttribute& attribute);

    VertexBinding& operator+=(const grfx::VertexAttribute& rhs);

private:
    static const grfx::VertexInputRate kInvalidVertexInputRate = static_cast<grfx::VertexInputRate>(~0);

    uint32_t                           mBinding   = 0;
    uint32_t                           mStride    = 0;
    grfx::VertexInputRate              mInputRate = kInvalidVertexInputRate;
    std::vector<grfx::VertexAttribute> mAttributes;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_helper_h
