#ifndef ppx_grfx_dx_device_h
#define ppx_grfx_dx_device_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/dx/dx_descriptor_helper.h"
#include "ppx/grfx/grfx_device.h"

namespace ppx {
namespace grfx {
namespace dx {

class Device
    : public grfx::Device
{
public:
    Device() {}
    virtual ~Device() {}

    D3D12DevicePtr      GetDxDevice() const { return mDevice; }
    D3D12MA::Allocator* GetAllocator() const { return mAllocator; }

    UINT GetHandleIncrementSizeCBVSRVUAV() const { return mHandleIncrementSizeCBVSRVUAV; }
    UINT GetHandleIncrementSizeSampler() const { return mHandleIncrementSizeSampler; }

    Result AllocateRTVHandle(dx::DescriptorHandle* pHandle);
    void   FreeRTVHandle(const dx::DescriptorHandle* pHandle);

    Result AllocateDSVHandle(dx::DescriptorHandle* pHandle);
    void   FreeDSVHandle(const dx::DescriptorHandle* pHandle);

    HRESULT CreateRootSignatureDeserializer(
        LPCVOID    pSrcData,
        SIZE_T     SrcDataSizeInBytes,
        const IID& pRootSignatureDeserializerInterface,
        void**     ppRootSignatureDeserializer);

    HRESULT SerializeVersionedRootSignature(
        const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* pRootSignature,
        ID3DBlob**                                 ppBlob,
        ID3DBlob**                                 ppErrorBlob);

    HRESULT CreateVersionedRootSignatureDeserializer(
        LPCVOID    pSrcData,
        SIZE_T     SrcDataSizeInBytes,
        const IID& pRootSignatureDeserializerInterface,
        void**     ppRootSignatureDeserializer);

    virtual Result WaitIdle() override;

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
    virtual Result AllocateObject(grfx::RenderPass** ppObject) override;
    virtual Result AllocateObject(grfx::RenderTargetView** ppObject) override;
    virtual Result AllocateObject(grfx::SampledImageView** ppObject) override;
    virtual Result AllocateObject(grfx::Sampler** ppObject) override;
    virtual Result AllocateObject(grfx::Semaphore** ppObject) override;
    virtual Result AllocateObject(grfx::ShaderModule** ppObject) override;
    virtual Result AllocateObject(grfx::ShaderProgram** ppObject) override;
    virtual Result AllocateObject(grfx::Swapchain** ppObject) override;

protected:
    virtual Result CreateApiObjects(const grfx::DeviceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    void   LoadRootSignatureFunctions();
    Result CreateQueues(const grfx::DeviceCreateInfo* pCreateInfo);

private:
    D3D12DevicePtr             mDevice;
    ObjPtr<D3D12MA::Allocator> mAllocator;

    UINT                        mHandleIncrementSizeCBVSRVUAV = 0;
    UINT                        mHandleIncrementSizeSampler   = 0;
    dx::DescriptorHandleManager mRTVHandleManager;
    dx::DescriptorHandleManager mDSVHandleManager;

    PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER           mFnD3D12CreateRootSignatureDeserializer          = nullptr;
    PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE           mFnD3D12SerializeVersionedRootSignature          = nullptr;
    PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER mFnD3D12CreateVersionedRootSignatureDeserializer = nullptr;
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_device_h
