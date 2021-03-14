#ifndef ppx_grfx_dx11_descriptor_h
#define ppx_grfx_dx11_descriptor_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_descriptor.h"

namespace ppx {
namespace grfx {
namespace dx11{

class DescriptorPool
    : public grfx::DescriptorPool
{
public:
    struct Allocation
    {
        const grfx::DescriptorSet* pSet    = nullptr;
        uint32_t                   binding = UINT32_MAX;
        uint32_t                   offset  = 0;
        uint32_t                   count   = 0;
    };

    DescriptorPool() {}
    virtual ~DescriptorPool() {}

    Result AllocateDescriptorSet(uint32_t numDescriptorsCBVSRVUAV, uint32_t numDescriptorsSampler);
    void   FreeDescriptorSet(uint32_t numDescriptorsCBVSRVUAV, uint32_t numDescriptorsSampler);

protected:
    virtual Result CreateApiObjects(const grfx::DescriptorPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSet
    : public grfx::DescriptorSet
{
public:
    DescriptorSet() {}
    virtual ~DescriptorSet() {}

    virtual Result UpdateDescriptors(uint32_t writeCount, const grfx::WriteDescriptor* pWrites) override;

protected:
    virtual Result CreateApiObjects(const grfx::internal::DescriptorSetCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
};

// -------------------------------------------------------------------------------------------------

class DescriptorSetLayout
    : public grfx::DescriptorSetLayout
{
public:
    struct DescriptorRange
    {
        uint32_t binding = UINT32_MAX;
        uint32_t count   = 0;
    };

    DescriptorSetLayout() {}
    virtual ~DescriptorSetLayout() {}

protected:
    virtual Result CreateApiObjects(const grfx::DescriptorSetLayoutCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif ppx_grfx_dx11_descriptor_h
