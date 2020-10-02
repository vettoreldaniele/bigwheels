#ifndef ppx_grfx_config_h
#define ppx_grfx_config_h

#include "ppx/000_config.h"
#include "ppx/grfx/grfx_helper.h"

#define PPX_MAX_RENDER_TARGETS 8

namespace ppx {
namespace grfx {

enum Api
{
    API_UNDEFINED = 0,
    API_VK,
    API_DX,
};

enum AttachmentLoadOp
{
    ATTACHMENT_LOAD_OP_LOAD = 0,
    ATTACHMENT_LOAD_OP_CLEAR,
    ATTACHMENT_LOAD_OP_DONT_CARE,
};

enum AttachmentStoreOp
{
    ATTACHMENT_STORE_OP_STORE = 0,
    ATTACHMENT_STORE_OP_DONT_CARE,
};

enum ComponentSwizzle
{
    COMPONENT_SWIZZLE_IDENTITY = 0,
    COMPONENT_SWIZZLE_ZERO     = 1,
    COMPONENT_SWIZZLE_ONE      = 2,
    COMPONENT_SWIZZLE_R        = 3,
    COMPONENT_SWIZZLE_G        = 4,
    COMPONENT_SWIZZLE_B        = 5,
    COMPONENT_SWIZZLE_A        = 6,
};

enum Format
{
    FORMAT_UNDEFINED = 0,

    // 8-bit signed normalized
    FORMAT_R8_SNORM,
    FORMAT_R8G8_SNORM,
    FORMAT_R8G8B8_SNORM,
    FORMAT_R8G8B8A8_SNORM,
    FORMAT_B8G8R8_SNORM,
    FORMAT_B8G8R8A8_SNORM,

    // 8-bit unsigned normalized
    FORMAT_R8_UNORM,
    FORMAT_R8G8_UNORM,
    FORMAT_R8G8B8_UNORM,
    FORMAT_R8G8B8A8_UNORM,
    FORMAT_B8G8R8_UNORM,
    FORMAT_B8G8R8A8_UNORM,

    // 8-bit signed integer
    FORMAT_R8_SINT,
    FORMAT_R8G8_SINT,
    FORMAT_R8G8B8_SINT,
    FORMAT_R8G8B8A8_SINT,
    FORMAT_B8G8R8_SINT,
    FORMAT_B8G8R8A8_SINT,

    // 8-bit unsigned integer
    FORMAT_R8_UINT,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8B8_UINT,
    FORMAT_R8G8B8A8_UINT,
    FORMAT_B8G8R8_UINT,
    FORMAT_B8G8R8A8_UINT,

    // 16-bit signed normalized
    FORMAT_R16_SNORM,
    FORMAT_R16G16_SNORM,
    FORMAT_R16G16B16_SNORM,
    FORMAT_R16G16B16A16_SNORM,

    // 16-bit unsigned normalized
    FORMAT_R16_UNORM,
    FORMAT_R16G16_UNORM,
    FORMAT_R16G16B16_UNORM,
    FORMAT_R16G16B16A16_UNORM,

    // 16-bit signed integer
    FORMAT_R16_SINT,
    FORMAT_R16G16_SINT,
    FORMAT_R16G16B16_SINT,
    FORMAT_R16G16B16A16_SINT,

    // 16-bit unsigned integer
    FORMAT_R16_UINT,
    FORMAT_R16G16_UINT,
    FORMAT_R16G16B16_UINT,
    FORMAT_R16G16B16A16_UINT,

    // 16-bit float
    FORMAT_R16_FLOAT,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16B16_FLOAT,
    FORMAT_R16G16B16A16_FLOAT,

    // 32-bit signed integer
    FORMAT_R32_SINT,
    FORMAT_R32G32_SINT,
    FORMAT_R32G32B32_SINT,
    FORMAT_R32G32B32A32_SINT,

    // 32-bit unsigned integer
    FORMAT_R32_UINT,
    FORMAT_R32G32_UINT,
    FORMAT_R32G32B32_UINT,
    FORMAT_R32G32B32A32_UINT,

    // 32-bit float
    FORMAT_R32_FLOAT,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32A32_FLOAT,

    // 8-bit unsigned integer stencil
    FORMAT_S8_UINT,

    // 16-bit unsigned normalized depth
    FORMAT_D16_UNORM,

    // 32-bit float depth
    FORMAT_D32_FLOAT,

    // Depth/stencil combinations
    FORMAT_D16_UNORM_S8_UINT,
    FORMAT_D24_UNORM_S8_UINT,
    FORMAT_D32_FLOAT_S8_UINT,

    // SRGB
    FORMAT_R8_SRGB,
    FORMAT_R8G8_SRGB,
    FORMAT_R8G8B8_SRGB,
    FORMAT_R8G8B8A8_SRGB,
    FORMAT_B8G8R8_SRGB,
    FORMAT_B8G8R8A8_SRGB,
};

enum ImageType
{
    IMAGE_TYPE_UNDEFINED = 0,
    IMAGE_TYPE_1D        = 1,
    IMAGE_TYPE_2D        = 2,
    IMAGE_TYPE_3D        = 3,
};

enum ImageViewType
{
    IMAGE_VIEW_TYPE_UNDEFINED  = 0,
    IMAGE_VIEW_TYPE_1D         = 1,
    IMAGE_VIEW_TYPE_2D         = 2,
    IMAGE_VIEW_TYPE_3D         = 3,
    IMAGE_VIEW_TYPE_CUBE       = 4,
    IMAGE_VIEW_TYPE_1D_ARRAY   = 5,
    IMAGE_VIEW_TYPE_2D_ARRAY   = 6,
    IMAGE_VIEW_TYPE_CUBE_ARRAY = 7,
};

enum PresentMode
{
    PRESENT_MODE_UNDEFINED = 0,
    PRESENT_MODE_FIFO,
    PRESENT_MODE_MAILBOX,
    PRESENT_MODE_IMMEDIATE,
};

enum ResourceState
{
    RESOURCE_STATE_UNDEFINED = 0,
    RESOURCE_STATE_GENERAL,
    RESOURCE_STATE_CONSTANT_BUFFER,
    RESOURCE_STATE_VERTEX_BUFFER,
    RESOURCE_STATE_INDEX_BUFFER,
    RESOURCE_STATE_RENDER_TARGET,
    RESOURCE_STATE_UNORDERED_ACCESS,
    RESOURCE_STATE_DEPTH_STENCIL_READ,
    RESOURCE_STATE_DEPTH_STENCIL_WRITE,
    RESOURCE_STATE_DEPTH_WRITE_STENCIL_READ,
    RESOURCE_STATE_DEPTH_READ_STENCIL_WRITE,
    RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
    RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
    RESOURCE_STATE_STREAM_OUT,
    RESOURCE_STATE_INDIRECT_ARGUMENT,
    RESOURCE_STATE_COPY_SRC,
    RESOURCE_STATE_COPY_DST,
    RESOURCE_STATE_RESOLVE_SRC,
    RESOURCE_STATE_RESOLVE_DST,
    RESOURCE_STATE_PRESENT,
    RESOURCE_STATE_PREDICATION,
    RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
};

enum SampleCount
{
    SAMPLE_COUNT_UNDEFINED = 0,
    SAMPLE_COUNT_1         = 1,
    SAMPLE_COUNT_2         = 2,
    SAMPLE_COUNT_4         = 4,
    SAMPLE_COUNT_8         = 8,
    SAMPLE_COUNT_16        = 16,
    SAMPLE_COUNT_32        = 32,
    SAMPLE_COUNT_64        = 64,
};

enum VendorId
{
    VENDOR_ID_UNKNOWN = 0x0000,
    VENDOR_ID_AMD     = 0x1002,
    VENDOR_ID_INTEL   = 0x8086,
    VENDOR_ID_NVIDIA  = 0x10DE,
};

// -------------------------------------------------------------------------------------------------

class Buffer;
class CommandBuffer;
class CommandPool;
class ComputePipeline;
class DescriptorPool;
class DescriptorSet;
class DescriptorSet;
class DescriptorSetLayout;
class Device;
class Fence;
class Gpu;
class GraphicsPipeline;
class Image;
class Instance;
class PipelineLayout;
class Queue;
class RenderPass;
class Semaphore;
class ShaderModule;
class ShaderProgram;
class Surface;
class Swapchain;
class Texture;

class DepthStencilView;
class RenderTargetView;
class SampledImageView;
class StorageImageView;

// -------------------------------------------------------------------------------------------------

using BufferPtr              = ObjPtr<Buffer>;
using CommandBufferPtr       = ObjPtr<CommandBuffer>;
using CommandPoolPtr         = ObjPtr<CommandPool>;
using ComputePipelinePtr     = ObjPtr<ComputePipeline>;
using DescriptorPoolPtr      = ObjPtr<DescriptorPool>;
using DescriptorSetPtr       = ObjPtr<DescriptorSet>;
using DescriptorSetLayoutPtr = ObjPtr<DescriptorSetLayout>;
using DevicePtr              = ObjPtr<Device>;
using FencePtr               = ObjPtr<Fence>;
using GraphicsPipelinePtr    = ObjPtr<GraphicsPipeline>;
using GpuPtr                 = ObjPtr<Gpu>;
using ImagePtr               = ObjPtr<Image>;
using InstancePtr            = ObjPtr<Instance>;
using PipelineLayoutPtr      = ObjPtr<PipelineLayout>;
using QueuePtr               = ObjPtr<Queue>;
using RenderPassPtr          = ObjPtr<RenderPass>;
using SemaphorePtr           = ObjPtr<Semaphore>;
using ShaderModulePtr        = ObjPtr<ShaderModule>;
using ShaderProgramPtr       = ObjPtr<ShaderProgram>;
using SurfacePtr             = ObjPtr<Surface>;
using SwapchainPtr           = ObjPtr<Swapchain>;
using TexturePtr             = ObjPtr<Texture>;

using DepthStencilViewPtr = ObjPtr<DepthStencilView>;
using RenderTargetViewPtr = ObjPtr<RenderTargetView>;
using SampledImageViewPtr = ObjPtr<SampledImageView>;
using StorageImageViewPtr = ObjPtr<StorageImageView>;

// -------------------------------------------------------------------------------------------------

struct ComponentMapping
{
    grfx::ComponentSwizzle r = grfx::COMPONENT_SWIZZLE_IDENTITY;
    grfx::ComponentSwizzle g = grfx::COMPONENT_SWIZZLE_IDENTITY;
    grfx::ComponentSwizzle b = grfx::COMPONENT_SWIZZLE_IDENTITY;
    grfx::ComponentSwizzle a = grfx::COMPONENT_SWIZZLE_IDENTITY;
};

struct DepthStencilClearValue
{
    float    depth;
    uint32_t stencil;
};

union RenderTargetClearValue
{
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };
    float rgba[4];
};

struct Rect
{
    int32_t  x;
    int32_t  y;
    uint32_t width;
    uint32_t height;
};

// -------------------------------------------------------------------------------------------------

template <typename CreatInfoT>
class CreateDestroyTraits
{
protected:
    virtual ppx::Result Create(const CreatInfoT* pCreateInfo)
    {
        // Copy create info
        mCreateInfo = *pCreateInfo;
        // Create API objects
        ppx::Result ppxres = CreateApiObjects(pCreateInfo);
        if (ppxres != ppx::SUCCESS) {
            DestroyApiObjects();
            return ppxres;
        }
        // Success
        return ppx::SUCCESS;
    }

    virtual void Destroy()
    {
        DestroyApiObjects();
    }

protected:
    virtual Result CreateApiObjects(const CreatInfoT* pCreateInfo) = 0;
    virtual void   DestroyApiObjects()                             = 0;
    friend class grfx::Instance;
    friend class grfx::Device;

protected:
    CreatInfoT mCreateInfo = {};
};

// -------------------------------------------------------------------------------------------------

template <typename CreatInfoT>
class InstanceObject
    : public CreateDestroyTraits<CreatInfoT>
{
public:
    grfx::Instance* GetInstance() const
    {
        grfx::Instance* ptr = mInstance;
        return ptr;
    }

private:
    void SetParent(grfx::Instance* pInstance)
    {
        mInstance = pInstance;
    }
    friend class grfx::Instance;

private:
    grfx::InstancePtr mInstance;
};

// -------------------------------------------------------------------------------------------------

template <typename CreatInfoT>
class DeviceObject
    : public CreateDestroyTraits<CreatInfoT>
{
public:
    grfx::Device* GetDevice() const
    {
        grfx::Device* ptr = mDevice;
        return ptr;
    }

private:
    void SetParent(grfx::Device* pDevice)
    {
        mDevice = pDevice;
    }
    friend class grfx::Device;

private:
    grfx::DevicePtr mDevice;
};

// -------------------------------------------------------------------------------------------------

const uint32_t kInvalidQueueFamilyIndex = UINT32_MAX;
const uint32_t kInvalidQueueIndex       = UINT32_MAX;

} // namespace grfx
} // namespace ppx

#endif // grfx_config_h
