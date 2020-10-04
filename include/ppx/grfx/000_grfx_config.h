#ifndef ppx_grfx_config_h
#define ppx_grfx_config_h

#include "ppx/000_config.h"
#include "ppx/grfx/grfx_constants.h"
#include "ppx/grfx/grfx_enums.h"
#include "ppx/grfx/grfx_format.h"
#include "ppx/grfx/grfx_helper.h"

namespace ppx {
namespace grfx {

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
class PipelineInterface;
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
struct IndexBufferView;
struct VertexBufferView;

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
using PipelineInterfacePtr   = ObjPtr<PipelineInterface>;
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

struct Viewport
{
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
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

} // namespace grfx
} // namespace ppx

#endif // grfx_config_h
