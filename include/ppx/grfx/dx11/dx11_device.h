#ifndef ppx_grfx_dx11_device_h
#define ppx_grfx_dx11_device_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_device.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class Device
    : public grfx::Device
{
public:
    Device() {}
    virtual ~Device() {}

    typename D3D11DevicePtr::InterfaceType*        GetDxDevice() const { return mDevice.Get(); }
    typename D3D11DeviceContextPtr::InterfaceType* GetDxDeviceContext() const { return mDeviceContext.Get(); }

    virtual Result WaitIdle() override;

    virtual Result ResolveQueryData(
        const grfx::QueryPool* pQueryPool,
        uint32_t               firstQuery,
        uint32_t               queryCount,
        uint64_t               dstDataSize,
        void*                  pDstData) override;

    Result GetStructuredBufferSRV(
        const grfx::Buffer*                                  pBuffer,
        UINT                                                 numElements,
        typename D3D11ShaderResourceViewPtr::InterfaceType** ppSRV);

protected:
    virtual Result AllocateObject(grfx::Buffer** ppObject) override;
    virtual Result AllocateObject(grfx::CommandBuffer** ppObject) override;
    virtual Result AllocateObject(grfx::CommandPool** ppObject) override;
    virtual Result AllocateObject(grfx::ComputePipeline** ppObject) override;
    virtual Result AllocateObject(grfx::DepthStencilView** ppObject) override;
    virtual Result AllocateObject(grfx::DescriptorPool** ppObject) override;
    virtual Result AllocateObject(grfx::DescriptorSet** ppObject) override;
    virtual Result AllocateObject(grfx::DescriptorSetLayout** ppObject) override;
    virtual Result AllocateObject(grfx::Fence** ppObject) override;
    virtual Result AllocateObject(grfx::GraphicsPipeline** ppObject) override;
    virtual Result AllocateObject(grfx::Image** ppObject) override;
    virtual Result AllocateObject(grfx::PipelineInterface** ppObject) override;
    virtual Result AllocateObject(grfx::Queue** ppObject) override;
    virtual Result AllocateObject(grfx::QueryPool** ppObject) override;
    virtual Result AllocateObject(grfx::RenderPass** ppObject) override;
    virtual Result AllocateObject(grfx::RenderTargetView** ppObject) override;
    virtual Result AllocateObject(grfx::SampledImageView** ppObject) override;
    virtual Result AllocateObject(grfx::Sampler** ppObject) override;
    virtual Result AllocateObject(grfx::Semaphore** ppObject) override;
    virtual Result AllocateObject(grfx::ShaderModule** ppObject) override;
    virtual Result AllocateObject(grfx::ShaderProgram** ppObject) override;
    virtual Result AllocateObject(grfx::StorageImageView** ppObject) override;
    virtual Result AllocateObject(grfx::Swapchain** ppObject) override;

protected:
    virtual Result CreateApiObjects(const grfx::DeviceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result CreateQueues(const grfx::DeviceCreateInfo* pCreateInfo);

private:
    D3D11DevicePtr        mDevice;
    D3D11DeviceContextPtr mDeviceContext;

    struct StructuredBufferSRV
    {
        const grfx::Buffer*        pBuffer     = nullptr;
        UINT                       numElements = 0;
        D3D11ShaderResourceViewPtr SRV;
    };

    std::vector<StructuredBufferSRV> mStructuredBufferSRVs;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_device_h
