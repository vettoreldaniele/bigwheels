#ifndef ppx_grfx_dx12_config_h
#define ppx_grfx_dx12_config_h

#include "ppx/grfx/000_grfx_config.h"
#include "ppx/grfx/dx12/dx12_util.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "D3D12MemAlloc.h"

#if defined(PPX_ENABLE_LOG_OBJECT_CREATION)
#define PPX_LOG_OBJECT_CREATION(TAG, ADDR) \
    PPX_LOG_INFO("DX OBJECT CREATED: addr=0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << reinterpret_cast<uintptr_t>(ADDR) << ", type="## #TAG)
#else
#define PPX_LOG_OBJECT_CREATION(TAG, ADDR)
#endif

namespace ppx {
namespace grfx {
namespace dx12 {

using DXGIAdapterPtr              = ComPtr<IDXGIAdapter4>;
using DXGIFactoryPtr              = ComPtr<IDXGIFactory7>;
using DXGIDebugPtr                = ComPtr<IDXGIDebug1>;
using DXGIInfoQueuePtr            = ComPtr<IDXGIInfoQueue>;
using DXGISwapChainPtr            = ComPtr<IDXGISwapChain4>;
using D3D12CommandAllocatorPtr    = ComPtr<ID3D12CommandAllocator>;
using D3D12CommandQueuePtr        = ComPtr<ID3D12CommandQueue>;
using D3D12DebugPtr               = ComPtr<ID3D12Debug>;
using D3D12DescriptorHeapPtr      = ComPtr<ID3D12DescriptorHeap>;
using D3D12DevicePtr              = ComPtr<ID3D12Device5>;
using D3D12FencePtr               = ComPtr<ID3D12Fence1>;
using D3D12GraphicsCommandListPtr = ComPtr<ID3D12GraphicsCommandList4>;
using D3D12PipelineStatePtr       = ComPtr<ID3D12PipelineState>;
using D3D12QueryHeapPtr           = ComPtr<ID3D12QueryHeap>;
using D3D12ResourcePtr            = ComPtr<ID3D12Resource1>;
using D3D12RootSignaturePtr       = ComPtr<ID3D12RootSignature>;

// -------------------------------------------------------------------------------------------------

class Buffer;
class CommandBuffer;
class CommandPool;
class ComputePipeline;
class DepthStencilView;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class Device;
class Fence;
class Gpu;
class GraphicsPipeline;
class Image;
class Instance;
class Pipeline;
class PipelineInterface;
class Queue;
class Query;
class RenderPass;
class RenderTargetView;
class Sampler;
class Semaphore;
class ShaderModule;
class Surface;
class Swapchain;

// -------------------------------------------------------------------------------------------------

template <typename GrfxTypeT>
struct ApiObjectLookUp
{
};

template <>
struct ApiObjectLookUp<grfx::Buffer>
{
    using GrfxType = grfx::Buffer;
    using ApiType  = dx12::Buffer;
};

template <>
struct ApiObjectLookUp<grfx::CommandBuffer>
{
    using GrfxType = grfx::CommandBuffer;
    using ApiType  = dx12::CommandBuffer;
};

template <>
struct ApiObjectLookUp<grfx::CommandPool>
{
    using GrfxType = grfx::CommandPool;
    using ApiType  = dx12::CommandPool;
};

template <>
struct ApiObjectLookUp<grfx::ComputePipeline>
{
    using GrfxType = grfx::ComputePipeline;
    using ApiType  = dx12::ComputePipeline;
};

template <>
struct ApiObjectLookUp<grfx::DescriptorPool>
{
    using GrfxType = grfx::DescriptorPool;
    using ApiType  = dx12::DescriptorPool;
};

template <>
struct ApiObjectLookUp<grfx::DescriptorSet>
{
    using GrfxType = grfx::DescriptorSet;
    using ApiType  = dx12::DescriptorSet;
};

template <>
struct ApiObjectLookUp<grfx::DescriptorSetLayout>
{
    using GrfxType = grfx::DescriptorSetLayout;
    using ApiType  = dx12::DescriptorSetLayout;
};

template <>
struct ApiObjectLookUp<grfx::DepthStencilView>
{
    using GrfxType = grfx::DepthStencilView;
    using ApiType  = dx12::DepthStencilView;
};

template <>
struct ApiObjectLookUp<grfx::Device>
{
    using GrfxType = grfx::Device;
    using ApiType  = dx12::Device;
};

template <>
struct ApiObjectLookUp<grfx::Fence>
{
    using GrfxType = grfx::Fence;
    using ApiType  = dx12::Fence;
};

template <>
struct ApiObjectLookUp<grfx::GraphicsPipeline>
{
    using GrfxType = grfx::GraphicsPipeline;
    using ApiType  = dx12::GraphicsPipeline;
};

template <>
struct ApiObjectLookUp<grfx::Image>
{
    using GrfxType = grfx::Image;
    using ApiType  = dx12::Image;
};

template <>
struct ApiObjectLookUp<grfx::Instance>
{
    using GrfxType = grfx::Instance;
    using ApiType  = Instance;
};

template <>
struct ApiObjectLookUp<grfx::Gpu>
{
    using GrfxType = grfx::Gpu;
    using ApiType  = dx12::Gpu;
};

template <>
struct ApiObjectLookUp<grfx::Queue>
{
    using GrfxType = grfx::Queue;
    using ApiType  = dx12::Queue;
};

template <>
struct ApiObjectLookUp<grfx::Query>
{
    using GrfxType = grfx::Query;
    using ApiType  = dx12::Query;
};

template <>
struct ApiObjectLookUp<grfx::PipelineInterface>
{
    using GrfxType = grfx::PipelineInterface;
    using ApiType  = dx12::PipelineInterface;
};

template <>
struct ApiObjectLookUp<grfx::RenderPass>
{
    using GrfxType = grfx::RenderPass;
    using ApiType  = dx12::RenderPass;
};

template <>
struct ApiObjectLookUp<grfx::RenderTargetView>
{
    using GrfxType = grfx::RenderTargetView;
    using ApiType  = dx12::RenderTargetView;
};

template <>
struct ApiObjectLookUp<grfx::Sampler>
{
    using GrfxType = grfx::Sampler;
    using ApiType  = dx12::Sampler;
};

template <>
struct ApiObjectLookUp<grfx::Semaphore>
{
    using GrfxType = grfx::Semaphore;
    using ApiType  = dx12::Semaphore;
};

template <>
struct ApiObjectLookUp<grfx::ShaderModule>
{
    using GrfxType = grfx::ShaderModule;
    using ApiType  = dx12::ShaderModule;
};

template <>
struct ApiObjectLookUp<grfx::Surface>
{
    using GrfxType = grfx::Surface;
    using ApiType  = dx12::Surface;
};

template <>
struct ApiObjectLookUp<grfx::Swapchain>
{
    using GrfxType = grfx::Swapchain;
    using ApiType  = dx12::Swapchain;
};
template <typename GrfxTypeT>
typename ApiObjectLookUp<GrfxTypeT>::ApiType* ToApi(GrfxTypeT* pGrfxObject)
{
    using ApiType       = typename ApiObjectLookUp<GrfxTypeT>::ApiType;
    ApiType* pApiObject = static_cast<ApiType*>(pGrfxObject);
    return pApiObject;
}

template <typename GrfxTypeT>
const typename ApiObjectLookUp<GrfxTypeT>::ApiType* ToApi(const GrfxTypeT* pGrfxObject)
{
    using ApiType             = typename ApiObjectLookUp<GrfxTypeT>::ApiType;
    const ApiType* pApiObject = static_cast<const ApiType*>(pGrfxObject);
    return pApiObject;
}

template <typename GrfxTypeT>
typename ApiObjectLookUp<GrfxTypeT>::ApiType* ToApi(ObjPtr<GrfxTypeT>& pGrfxObject)
{
    using ApiType       = typename ApiObjectLookUp<GrfxTypeT>::ApiType;
    ApiType* pApiObject = static_cast<ApiType*>(pGrfxObject.Get());
    return pApiObject;
}

} // namespace dx12
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx12_config_h
