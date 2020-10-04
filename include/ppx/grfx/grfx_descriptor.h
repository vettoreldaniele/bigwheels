#ifndef ppx_grfx_descriptor_h
#define ppx_grfx_descriptor_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct DescriptorBinding
//!
//! *** WARNING ***
//! 'DescriptorBinding::count' is *NOT* the same as 'VkDescriptorSetLayoutBinding::descriptorCount'.
//!
//!
struct DescriptorBinding
{
    grfx::DescriptorType type  = grfx::DESCRIPTOR_TYPE_UNDEFINED;
    uint32_t             count = 0; // WARNING: Not the same as VkDescriptorSetLayoutBinding::descriptorCount
};

// -------------------------------------------------------------------------------------------------

//! @struct DescriptorPoolCreateInfo
//!
//!
struct DescriptorPoolCreateInfo
{
    uint32_t sampler              = 0;
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
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_descriptor_h
