#ifndef ppx_grfx_dx11_queue_h
#define ppx_grfx_dx11_queue_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/dx11/dx11_command.h"
#include "ppx/grfx/grfx_queue.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class Queue
    : public grfx::Queue
{
public:
    Queue() {}
    virtual ~Queue() {}

    typename D3D11DeviceContextPtr::InterfaceType* GetDxDeviceContext() const { return mDeviceContext.Get(); }

    virtual Result WaitIdle() override;

    virtual Result Submit(const grfx::SubmitInfo* pSubmitInfo) override;

    virtual Result GetTimestampFrequency(uint64_t* pFrequency) const override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::QueueCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    void ResetGraphicsState();
    void SetGraphicsState(const grfx::dx11::CommandBuffer::ActionCmd& cmd, const grfx::dx11::CommandBuffer* pCmdBuf);
    void ResetComputeState();
    void SetComputeState(const grfx::dx11::CommandBuffer::ActionCmd& cmd, const grfx::dx11::CommandBuffer* pCmdBuf);

private:
    D3D11DeviceContextPtr mDeviceContext;
    uint32_t              mViewportStateIndex           = kInvalidStateIndex;
    uint32_t              mScissorStateIndex            = kInvalidStateIndex;
    uint32_t              mIndexBufferStateIndex        = kInvalidStateIndex;
    uint32_t              mVertexBufferStateIndex       = kInvalidStateIndex;
    uint32_t              mGraphicsPipelineStateIndex   = kInvalidStateIndex;
    uint32_t              mGraphicsDescriptorStateIndex = kInvalidStateIndex;
    uint32_t              mComputePipelineStateIndex    = kInvalidStateIndex;
    uint32_t              mComputeDescriptorStateIndex  = kInvalidStateIndex;

    struct BoundResource
    {
        void* pResource = nullptr;
    };

    std::array<BoundResource, PPX_D3D11_COMPAT_MAX_UAV_SLOTS> mBoundUAVs;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_queue_h
