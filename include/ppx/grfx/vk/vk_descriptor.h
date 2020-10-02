#ifndef ppx_grfx_vk_descriptor_h
#define ppx_grfx_vk_descriptor_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_descriptor.h"

namespace ppx {
namespace grfx {
namespace vk {

class DescriptorPool
    : public grfx::DescriptorPool
{
public:
    DescriptorPool() {}
    virtual ~DescriptorPool() {}

protected:
    virtual Result CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkDescriptorPoolPtr mDescriptorPool;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSet
    : public grfx::DescriptorSet
{
public:
    DescriptorSet() {}
    virtual ~DescriptorSet() {}

protected:
    virtual Result CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkDescriptorSetPtr mDescriptorSet;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSetLayout
    : public grfx::DescriptorSetLayout
{
public:
    DescriptorSetLayout() {}
    virtual ~DescriptorSetLayout() {}

protected:
    virtual Result CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkDescriptorSetLayoutPtr mDescriptorSetLayout;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_descriptor_h
