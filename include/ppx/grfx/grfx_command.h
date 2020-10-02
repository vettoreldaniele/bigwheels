#ifndef ppx_grfx_command_buffer_h
#define ppx_grfx_command_buffer_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

struct RenderPassBeginInfo
{
    const grfx::RenderPass*      pRenderPass                            = nullptr;
    grfx::Rect                   renderArea                             = {};
    uint32_t                     RTVClearCount                          = 0;
    grfx::RenderTargetClearValue RTVClearValues[PPX_MAX_RENDER_TARGETS] = {};
    grfx::DepthStencilClearValue DSVClearValue                          = {};
};

// -------------------------------------------------------------------------------------------------

namespace internal {

struct CommandBufferCreateInfo
{
    const grfx::CommandPool* pPool = nullptr;
};

} // namespace internal

//! @class CommandBuffer
//!
//!
class CommandBuffer
    : public grfx::DeviceObject<grfx::internal::CommandBufferCreateInfo>
{
public:
    CommandBuffer() {}
    virtual ~CommandBuffer() {}

    virtual Result Begin() = 0;
    virtual Result End()   = 0;

    virtual void BeginRenderPass(const grfx::RenderPassBeginInfo* pBeginInfo) = 0;
    virtual void EndRenderPass()                                              = 0;

    virtual void TransitionImageLayout(
        const grfx::Image*  pImage,
        uint32_t            mipLevel,
        uint32_t            mipLevelCount,
        uint32_t            arrayLayer,
        uint32_t            arrayLayerCount,
        grfx::ResourceState beforeState,
        grfx::ResourceState afterState) = 0;
};

// -------------------------------------------------------------------------------------------------

//! @struct CommandPoolCreateInfo
//!
//!
struct CommandPoolCreateInfo
{
    const grfx::Queue* pQueue = nullptr;
};

//! @class CommandPool
//!
//!
class CommandPool
    : public grfx::DeviceObject<grfx::CommandPoolCreateInfo>
{
public:
    CommandPool() {}
    virtual ~CommandPool() {}
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_command_buffer_h
