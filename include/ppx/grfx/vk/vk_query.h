#ifndef ppx_grfx_vk_query_h
#define ppx_grfx_vk_query_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {
namespace vk {

class Query
    : public grfx::Query
{
public:
    Query();
    virtual ~Query() {}

    VkQueryPoolPtr GetVkQueryPool() const { return mQueryPool; }

    virtual void   Reset(uint32_t firstQuery, uint32_t queryCount) override;
    virtual void   Begin(grfx::CommandBuffer* pCommandBuffer, uint32_t index) override;
    virtual void   End(grfx::CommandBuffer* pCommandBuffer, uint32_t index) override;
    virtual void   WriteTimestamp(grfx::CommandBuffer* pCommandBuffer, grfx::PipelineStage pipelineStage, uint32_t index) override;
    virtual void   ResolveData(grfx::CommandBuffer* pCommandBuffer, uint32_t startIndex, uint32_t numQueries) override;
    virtual Result GetData(void* pDstData, uint64_t dstDataSize) override;

protected:
    virtual Result CreateApiObjects(const grfx::QueryCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
    uint32_t       GetQueryTypeSize(VkQueryType type, uint32_t multiplier);
    VkQueryType    GetQueryType() const { return mType; }

private:
    VkQueryPoolPtr      mQueryPool;
    VkQueryType         mType;
    grfx::BufferPtr     mBuffer;
    uint32_t            mMultiplier;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_query_h
