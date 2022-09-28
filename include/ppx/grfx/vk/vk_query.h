#ifndef ppx_grfx_vk_query_h
#define ppx_grfx_vk_query_h

#include "ppx/grfx/vk/vk_config.h"
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
    uint32_t       GetQueryTypeSize() const { return GetQueryTypeSize(mType, mMultiplier); }
    VkBufferPtr    GetReadBackBuffer() const;

    virtual void   Reset(uint32_t firstQuery, uint32_t queryCount) override;
    virtual Result GetData(void* pDstData, uint64_t dstDataSize) override;

protected:
    virtual Result CreateApiObjects(const grfx::QueryCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
    uint32_t       GetQueryTypeSize(VkQueryType type, uint32_t multiplier) const;
    VkQueryType    GetQueryType() const { return mType; }

private:
    VkQueryPoolPtr  mQueryPool;
    VkQueryType     mType;
    grfx::BufferPtr mBuffer;
    uint32_t        mMultiplier;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_query_h
