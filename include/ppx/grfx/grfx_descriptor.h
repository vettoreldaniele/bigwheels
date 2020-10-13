#ifndef ppx_grfx_descriptor_h
#define ppx_grfx_descriptor_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct DescriptorBinding
//!
//! NOTE: D3D12 controls shader visibility in the root paramter and not in the binding.
//!       All bindings in a set will use the same shader visibility that's determined by
//!       grfx::DescriptorLayoutCreateInfo::shaderStageFlags.
//!
//! *** WARNING ***
//! 'DescriptorBinding::count' is *NOT* the same as 'VkDescriptorSetLayoutBinding::descriptorCount'.
//!
//!
struct DescriptorBinding
{
    uint32_t             binding    = PPX_VALUE_IGNORED;               //
    grfx::DescriptorType type       = grfx::DESCRIPTOR_TYPE_UNDEFINED; //
    uint32_t             arrayCount = 1;                               // WARNING: Not VkDescriptorSetLayoutBinding::descriptorCount

    DescriptorBinding() {}

    DescriptorBinding(
        uint32_t               binding_,
        grfx::DescriptorType   type_,
        uint32_t               arrayCount_       = 1,
        grfx::ShaderStageFlags shaderStageFlags_ = grfx::SHADER_STAGE_ALL_GRAPHICS)
        : binding(binding_),
          type(type_),
          arrayCount(arrayCount_) {}
};

struct WriteDescriptor
{
    uint32_t               binding      = PPX_VALUE_IGNORED;
    uint32_t               arrayIndex   = 0;
    grfx::DescriptorType   type         = grfx::DESCRIPTOR_TYPE_UNDEFINED;
    uint32_t               bufferOffset = 0;
    uint64_t               bufferRange  = PPX_WHOLE_SIZE;
    const grfx::Buffer*    pBuffer      = nullptr;
    const grfx::ImageView* pImageView   = nullptr;
    const grfx::Sampler*   pSampler     = nullptr;
};

// -------------------------------------------------------------------------------------------------

//! @struct DescriptorPoolCreateInfo
//!
//!
struct DescriptorPoolCreateInfo
{
    uint32_t sampler              = 0;
    uint32_t combinedImageSampler = 0;
    uint32_t sampledImage         = 0;
    uint32_t storageImage         = 0;
    uint32_t uniformTexelBuffer   = 0;
    uint32_t storageTexelBuffer   = 0;
    uint32_t uniformBuffer        = 0;
    uint32_t storageBuffer        = 0;
    uint32_t uniformBufferDynamic = 0;
    uint32_t storageBufferDynamic = 0;
    uint32_t inputAttachment      = 0;
};

//! @class DescriptorPool
//!
//!
class DescriptorPool
    : public grfx::DeviceObject<grfx::DescriptorPoolCreateInfo>
{
public:
    DescriptorPool() {}
    virtual ~DescriptorPool() {}
};

// -------------------------------------------------------------------------------------------------

namespace internal {

//! @struct DescriptorPoolCreateInfo
//!
//!
struct DescriptorSetCreateInfo
{
    grfx::DescriptorPool*            pPool   = nullptr;
    const grfx::DescriptorSetLayout* pLayout = nullptr;
};

} // namespace internal

//! @class DescriptorSet
//!
//!
class DescriptorSet
    : public grfx::DeviceObject<grfx::internal::DescriptorSetCreateInfo>
{
public:
    DescriptorSet() {}
    virtual ~DescriptorSet() {}

    grfx::DescriptorPoolPtr GetPool() const { return mCreateInfo.pPool; }

    virtual Result UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites) = 0;
};

// -------------------------------------------------------------------------------------------------

//! @struct DescriptorSetLayoutCreateInfo
//!
//! NOTE: D3D12 and Vulkan have different ways of controlling shader visibility
//!       for descriptor bindings:
//!         - D3D12 controls visibility at the root parameter level
//!         - Vulkan controls visiblity at the binding level
//!       We'll use the D3D12 model because it allows use to cascade the vsilbity
//!       flags from the set to the bindings.
//!
struct DescriptorSetLayoutCreateInfo
{
    std::vector<grfx::DescriptorBinding> bindings;
    grfx::ShaderStageBits                shaderVisiblity = SHADER_STAGE_ALL_GRAPHICS; // Single value not set of flags
};

//! @class DescriptorSetLayout
//!
//!
class DescriptorSetLayout
    : public grfx::DeviceObject<grfx::DescriptorSetLayoutCreateInfo>
{
public:
    DescriptorSetLayout() {}
    virtual ~DescriptorSetLayout() {}

    const std::vector<grfx::DescriptorBinding>& GetBindings() const { return mCreateInfo.bindings; }
    grfx::ShaderStageBits                       GetShaderVisiblity() const { return mCreateInfo.shaderVisiblity; }

protected:
    virtual Result Create(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo) override;
    friend class grfx::Device;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_descriptor_h
