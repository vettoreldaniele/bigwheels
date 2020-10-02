#ifndef ppx_grfx_descriptor_h
#define ppx_grfx_descriptor_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct DescriptorPoolCreateInfo
//!
//!
struct DescriptorPoolCreateInfo
{
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
