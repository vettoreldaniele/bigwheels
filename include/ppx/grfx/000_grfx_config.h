#ifndef ppx_grfx_config_h
#define ppx_grfx_config_h

#include "ppx/000_config.h"
#include "ppx/grfx/grfx_constants.h"
#include "ppx/grfx/grfx_enums.h"
#include "ppx/grfx/grfx_format.h"
#include "ppx/grfx/grfx_helper.h"
#include "ppx/grfx/grfx_util.h"

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
class DrawPass;
class Fence;
class FullscreenQuad;
class Gpu;
class GraphicsPipeline;
class Image;
class ImageView;
class Instance;
class Model;
class PipelineInterface;
class Queue;
class QueryPool;
class RenderPass;
class Sampler;
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

namespace internal {

class ImageResourceView;

} // namespace internal

// -------------------------------------------------------------------------------------------------

using BufferPtr              = ObjPtr<Buffer>;
using CommandBufferPtr       = ObjPtr<CommandBuffer>;
using CommandPoolPtr         = ObjPtr<CommandPool>;
using ComputePipelinePtr     = ObjPtr<ComputePipeline>;
using DescriptorPoolPtr      = ObjPtr<DescriptorPool>;
using DescriptorSetPtr       = ObjPtr<DescriptorSet>;
using DescriptorSetLayoutPtr = ObjPtr<DescriptorSetLayout>;
using DevicePtr              = ObjPtr<Device>;
using DrawPassPtr            = ObjPtr<DrawPass>;
using FencePtr               = ObjPtr<Fence>;
using FullscreenQuadPtr      = ObjPtr<FullscreenQuad>;
using GraphicsPipelinePtr    = ObjPtr<GraphicsPipeline>;
using GpuPtr                 = ObjPtr<Gpu>;
using ImagePtr               = ObjPtr<Image>;
using InstancePtr            = ObjPtr<Instance>;
using ModelPtr               = ObjPtr<Model>;
using PipelineInterfacePtr   = ObjPtr<PipelineInterface>;
using QueuePtr               = ObjPtr<Queue>;
using QueryPoolPtr           = ObjPtr<QueryPool>;
using RenderPassPtr          = ObjPtr<RenderPass>;
using SamplerPtr             = ObjPtr<Sampler>;
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

    Rect() {}

    Rect(int32_t x_, int32_t y_, uint32_t width_, uint32_t height_)
        : x(x_), y(y_), width(width_), height(height_) {}
};

struct Viewport
{
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;

    Viewport() {}

    Viewport(float x_, float y_, float width_, float height_, float minDepth_ = 0, float maxDepth_ = 1)
        : x(x_), y(y_), width(width_), height(height_), minDepth(minDepth_), maxDepth(maxDepth_) {}
};

// -------------------------------------------------------------------------------------------------

//! @enum Ownership
//!
//! The purpose of this enum is to help grfx objects manage the lifetime
//! of their member objects. All grfx objects are created with ownership
//! set to OWNERSHIP_REFERENCE. This means that the object lifetime is
//! left up to either grfx::Device or grfx::Instance unless the application
//! explicitly destroys it.
//!
//! If a member object's ownership is set to OWNERSHIP_EXCLUSIVE or
//! OWNERSHIP_RESTRICTED, this means that the containing object must
//! destroy it durnig the destruction process.
//!
//! If the containing object fails to destroy OWNERSHIP_EXCLUSIVE and
//! OWNERSHIP_RESTRICTED objects, then either grfx::Device or grfx::Instance
//! will destroy it in their destruction proces.
//!
//! If an object's ownership is set to OWNERSHIP_RESTRICTED then its
//! ownership cannot be changed. Calling SetOwnership() will have no effect.
//!
//! Examples of objects with OWNERSHIP_EXCLUSIVE ownership:
//!   - Draw passes and render passes have create infos where only the
//!     format of the render target and/or depth stencil are known.
//!     In these cases draw passes and render passes will create the
//!     necessary backing images and views. These objects will be created
//!     with ownership set to EXCLUSIVE. The render pass will destroy
//!     these objects when it itself is destroyed.
//!
//!   - grfx::Model's buffers and textures typically have OWNERSHIP_REFERENCE
//!     ownership. However, the application is free to change ownership
//!     to EXCLUSIVE as it sees fit.
//!
//! Examples of objects with OWNERSHIP_EXCLUSIVE ownership:
//!   - Swapchain images and render passes are always OWNERSHIP_EXCLUSIVE.
//!
enum Ownership
{
    OWNERSHIP_REFERENCE  = 0,
    OWNERSHIP_EXCLUSIVE  = 1,
    OWNERSHIP_RESTRICTED = 2,
};

// -------------------------------------------------------------------------------------------------

class OwnershipTrait
{
public:
    grfx::Ownership GetOwnership() const { return mOwnership; }

    void SetOwnership(grfx::Ownership ownership)
    {
        // Cannot change to or from OWNERSHIP_RESTRICTED
        if ((ownership == grfx::OWNERSHIP_RESTRICTED) || (mOwnership == grfx::OWNERSHIP_RESTRICTED)) {
            return;
        }
        mOwnership = ownership;
    }

private:
    grfx::Ownership mOwnership = grfx::OWNERSHIP_REFERENCE;
};

template <typename CreatInfoT>
class CreateDestroyTraits
    : public OwnershipTrait
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
inline bool IsDx11(grfx::Api api)
{
    switch (api) {
        default: break;
        case grfx::API_DX_11_0:
        case grfx::API_DX_11_1: {
            return true;
        } break;
    }
    return false;
}

inline bool IsDx12(grfx::Api api)
{
    switch (api) {
        default: break;
        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            return true;
        } break;
    }
    return false;
}

inline bool IsDx(grfx::Api api)
{
    bool isDx11 = IsDx11(api);
    bool isDx12 = IsDx12(api);
    bool isDx   = isDx11 || isDx12;
    return isDx;
}

inline bool IsVk(grfx::Api api)
{
    switch (api) {
        default: break;
        case grfx::API_VK_1_1:
        case grfx::API_VK_1_2: {
            return true;
        } break;
    }
    return false;
}

} // namespace grfx
} // namespace ppx

#endif // grfx_config_h
