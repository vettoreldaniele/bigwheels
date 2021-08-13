#ifndef ppx_grfx_dx12_query_h
#define ppx_grfx_dx12_query_h

#include "ppx/grfx/dx12/000_dx12_config.h"
#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {
namespace dx12 {

class Query
    : public grfx::Query
{
public:
    Query();
    virtual ~Query() {}

    typename D3D12QueryHeapPtr::InterfaceType* GetDxQueryHeap() const { return mHeap.Get(); }


    virtual void   Reset(uint32_t firstQuery, uint32_t queryCount) override;
    virtual void   Begin(grfx::CommandBuffer* pCommandBuffer, uint32_t index) override;
    virtual void End(grfx::CommandBuffer* pCommandBuffer, uint32_t index) override;
    virtual void WriteTimestamp(grfx::CommandBuffer* pCommandBuffer, grfx::PipelineStage pipelineStage, uint32_t index) override;
    virtual void   ResolveData(grfx::CommandBuffer* pCommandBuffer, uint32_t startIndex, uint32_t numQueries) override;
    virtual Result GetData(void* pDstData, uint64_t dstDataSize) override;

protected:
    virtual Result CreateApiObjects(const grfx::QueryCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;
    uint32_t GetQueryTypeSize(D3D12_QUERY_HEAP_TYPE type);
    D3D12_QUERY_TYPE GetQueryType() const { return mQueryType; }

private :
    D3D12QueryHeapPtr   mHeap;
    D3D12_QUERY_TYPE    mQueryType;
    grfx::BufferPtr     mBuffer;
};

} // namespace dx12
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx12_query_h
