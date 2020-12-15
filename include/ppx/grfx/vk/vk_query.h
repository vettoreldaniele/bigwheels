#ifndef ppx_grfx_vk_query_h
#define ppx_grfx_vk_query_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {
namespace vk {

class QueryPool
    : public grfx::QueryPool
{
public:
    QueryPool() {}
    virtual ~QueryPool() {}

    VkQueryPoolPtr GetVkQueryPool() const { return mQueryPool; }

    virtual void Reset(uint32_t firstQuery, uint32_t queryCount) override;

protected:
    virtual Result CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    VkQueryPoolPtr mQueryPool;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_query_h
