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

    VkDescriptorPoolPtr GetVkDescriptorPool() const { return mDescriptorPool; }

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

    VkDescriptorSetPtr GetVkDescriptorSet() const { return mDescriptorSet; }

    virtual Result UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites) override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkDescriptorSetPtr  mDescriptorSet;
    VkDescriptorPoolPtr mDescriptorPool;

    // Reduce memory allocations during update process
    std::vector<VkWriteDescriptorSet>   mWriteStore;
    std::vector<VkDescriptorImageInfo>  mImageInfoStore;
    std::vector<VkBufferView>           mTexelBufferStore;
    std::vector<VkDescriptorBufferInfo> mBufferInfoStore;
    uint32_t                            mWriteCount       = 0;
    uint32_t                            mImageCount       = 0;
    uint32_t                            mTexelBufferCount = 0;
    uint32_t                            mBufferCount      = 0;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSetLayout
    : public grfx::DescriptorSetLayout
{
public:
    DescriptorSetLayout() {}
    virtual ~DescriptorSetLayout() {}

    VkDescriptorSetLayoutPtr GetVkDescriptorSetLayout() const { return mDescriptorSetLayout; }

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
