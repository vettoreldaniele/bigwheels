#ifndef ppx_grfx_descriptor_h
#define ppx_grfx_descriptor_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct DescriptorBinding
//!
//! *** WARNING ***
//! 'DescriptorBinding::arrayCount' is *NOT* the same as 'VkDescriptorSetLayoutBinding::descriptorCount'.
//!
//!
struct DescriptorBinding
{
    uint32_t              binding         = PPX_VALUE_IGNORED;               //
    grfx::DescriptorType  type            = grfx::DESCRIPTOR_TYPE_UNDEFINED; //
    uint32_t              arrayCount      = 1;                               // WARNING: Not VkDescriptorSetLayoutBinding::descriptorCount
    grfx::ShaderStageBits shaderVisiblity = SHADER_STAGE_ALL;                // Single value not set of flags

    DescriptorBinding() {}

    DescriptorBinding(
        uint32_t              binding_,
        grfx::DescriptorType  type_,
        uint32_t              arrayCount_      = 1,
        grfx::ShaderStageBits shaderVisiblity_ = grfx::SHADER_STAGE_ALL)
        : binding(binding_),
          type(type_),
          arrayCount(arrayCount_),
          shaderVisiblity(shaderVisiblity_) {}
};

struct WriteDescriptor
{
    uint32_t               binding                 = PPX_VALUE_IGNORED;
    uint32_t               arrayIndex              = 0;
    grfx::DescriptorType   type                    = grfx::DESCRIPTOR_TYPE_UNDEFINED;
    uint32_t               bufferOffset            = 0;
    uint64_t               bufferRange             = 0;
    uint32_t               structuredElementCount  = 0;
    uint32_t               structuredElementStride = 0;
    const grfx::Buffer*    pBuffer                 = nullptr;
    const grfx::ImageView* pImageView              = nullptr;
    const grfx::Sampler*   pSampler                = nullptr;
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
    uint32_t structuredBuffer     = 0;
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

    grfx::DescriptorPoolPtr          GetPool() const { return mCreateInfo.pPool; }
    const grfx::DescriptorSetLayout* GetLayout() const { return mCreateInfo.pLayout; }

    virtual Result UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites) = 0;
};

// -------------------------------------------------------------------------------------------------

//! @struct DescriptorSetLayoutCreateInfo
//!
//!
struct DescriptorSetLayoutCreateInfo
{
    std::vector<grfx::DescriptorBinding> bindings;
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

protected:
    virtual Result Create(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo) override;
    friend class grfx::Device;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_descriptor_h
