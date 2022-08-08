#ifndef ppx_grfx_dx11_query_h
#define ppx_grfx_dx11_query_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {
namespace dx11 {

using D3D11QueryHeap = std::vector<ID3D11Query*>;

class Query
    : public grfx::Query
{
public:
    Query();
    virtual ~Query() {}

    ID3D11Query* GetQuery(uint32_t queryIndex) const;
    void         SetResolveDataStartIndex(uint32_t index) { mResolveDataStartIndex = index; }
    void         SetResolveDataNumQueries(uint32_t numQueries) { mResolveDataNumQueries = numQueries; }

    virtual void   Reset(uint32_t firstQuery, uint32_t queryCount) override;
    virtual Result GetData(void* pDstData, uint64_t dstDataSize) override;

protected:
    virtual Result   CreateApiObjects(const grfx::QueryCreateInfo* pCreateInfo) override;
    virtual void     DestroyApiObjects() override;
    D3D11_QUERY      GetQueryType() const { return mQueryType; }

private:
    D3D11QueryHeap    mHeap;
    D3D11_QUERY       mQueryType;
    uint32_t          mResolveDataStartIndex;
    uint32_t          mResolveDataNumQueries;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx11_query_h
